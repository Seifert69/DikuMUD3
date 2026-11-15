#include "dilcheck.h"

#include <algorithm>
#include <cctype>

DilCheck::DilCheck(Report report)
    : report_(std::move(report))
{
}

void DilCheck::begin(const std::vector<Var> &vars, int argc)
{
    vars_ = vars;
    flow_ = Flow{};
    for (size_t i = 0; i < vars_.size(); i++)
    {
        flow_.vars.push_back(VarState{static_cast<int>(i) < argc ? State::Unknown : State::Never, false, false, false, 0});
    }
    securedAbove_.assign(vars_.size(), false);
    ifs_.clear();
    held_.clear();
    condReads_.clear();
    lastCondVars_.clear();
    for (bool &w : eventWarned_)
    {
        w = false;
    }
    condition_ = false;
    quiet_ = false;
}

void DilCheck::end()
{
    for (const Held &h : held_)
    {
        if (h.unlessSecured < 0 || !securedAbove_[h.unlessSecured])
        {
            report_(h.line, h.msg);
        }
    }
    held_.clear();
}

void DilCheck::warn(int line, const std::string &msg, int unlessSecured)
{
    held_.push_back({line, msg, unlessSecured});
}

DilCheck::VarState *DilCheck::at(int var)
{
    return (var >= 0 && var < static_cast<int>(flow_.vars.size())) ? &flow_.vars[var] : nullptr;
}

void DilCheck::assign(int var, int from)
{
    VarState *v = at(var);
    if (!v)
    {
        return;
    }
    v->state = State::Fresh;
    v->warned = false;
    if (from <= ACTIVATOR)
    {
        // wait(..., activator == pc); pc := activator: the same unit, so it stays secured
        if (std::find(lastCondVars_.begin(), lastCondVars_.end(), var) == lastCondVars_.end())
        {
            v->secured = false;
        }
    }
    else
    {
        // secure() protects a unit, not a variable: a copy of a secured variable survives waits too
        VarState *f = at(from);
        v->secured = f && f->secured;
    }
}

void DilCheck::use(int var, bool field, int line)
{
    static const char *const eventNames[] = {"activator", "medium", "target", "argument"};

    if (quiet_ || !flow_.reachable)
    {
        return;
    }
    if (var < 0)
    {
        // activator, medium, target, argument after a pause belong to the tick
        int i = ACTIVATOR - var;
        if (flow_.tickLine && !condition_ && i >= 0 && i < 4 && !eventWarned_[i])
        {
            eventWarned_[i] = true;
            warn(line,
                 std::string(eventNames[i]) + (var == ARGUMENT ? " is empty here" : " is null here") + ": after the pause on line " +
                     std::to_string(flow_.tickLine) + " the program was woken by a tick. Keep what you need in a variable before the pause");
        }
        return;
    }

    VarState *v = at(var);
    if (!v || vars_[var].kind == VarKind::Other || v->warned)
    {
        return;
    }
    bool extra = (vars_[var].kind == VarKind::Extra);

    if (condition_)
    {
        condReads_.emplace_back(var, line);
    }
    else if (v->state == State::Stale)
    {
        v->warned = true;
        // when it was set on some way only, or above a label, a secure() further down may cover it
        warn(line,
             vars_[var].name + " is null here: it was set before the wait on line " + std::to_string(v->staleLine) +
                 (extra ? ", and a wait clears every extraptr variable" : " and isn't secured, and a wait clears unsecured unitptr variables"),
             v->staleVague ? var : -1);
    }
    else if (v->state == State::Never && field)
    {
        v->warned = true;
        warn(line, vars_[var].name + " is used before it is set: it is still null here");
    }
}

void DilCheck::wait(bool tick, int line)
{
    // the conditions read since the last wait are tested from now on, after every wake-up
    lastCondVars_.clear();
    for (const auto &r : condReads_)
    {
        VarState *v = at(r.first);
        bool extra = (vars_[r.first].kind == VarKind::Extra);
        lastCondVars_.push_back(r.first);
        if (v && !v->warned && (extra || !v->secured))
        {
            v->warned = true;
            warn(r.second,
                 vars_[r.first].name + " is null in this condition: the condition is tested after the program wakes up, and waking up clears " +
                     (extra ? "every extraptr variable" : "unsecured unitptr variables (secure it first)"),
                 extra ? -1 : r.first);
        }
    }
    condReads_.clear();
    flow_.tickLine = tick ? line : 0;
    if (tick)
    {
        for (bool &w : eventWarned_)
        {
            w = false;
        }
    }
    for (size_t i = 0; i < flow_.vars.size(); i++)
    {
        VarState &v = flow_.vars[i];
        bool unit = (vars_[i].kind == VarKind::Unit);
        bool cleared = (vars_[i].kind == VarKind::Extra) || (unit && !v.secured);
        if (cleared && (v.state == State::Fresh || v.state == State::Unknown))
        {
            v.staleVague = unit && (v.state == State::Unknown);
            v.state = State::Stale;
            v.staleLine = line;
            v.warned = false;
        }
    }
}

void DilCheck::secure(int var, int line)
{
    VarState *v = at(var);
    if (!v)
    {
        return;
    }
    if (flow_.reachable && v->state == State::Never)
    {
        warn(line, "secure(" + vars_[var].name + ") comes before " + vars_[var].name + " is set, so it secures nothing");
    }
    else if (flow_.reachable && v->state == State::Stale)
    {
        warn(line,
             "secure(" + vars_[var].name + ") secures nothing: " + vars_[var].name + " was cleared by the wait on line " +
                 std::to_string(v->staleLine));
    }
    v->secured = true;
    securedAbove_[var] = true;
}

void DilCheck::unsecure(int var)
{
    if (VarState *v = at(var))
    {
        v->secured = false;
    }
}

void DilCheck::jump()
{
    flow_.reachable = false;
}

void DilCheck::boundary(bool label)
{
    for (size_t i = 0; i < flow_.vars.size(); i++)
    {
        VarState &v = flow_.vars[i];
        // a label can be reached from further down, where it may be set
        if (v.state == State::Stale || v.state == State::Fresh || (label && v.state == State::Never))
        {
            v.state = State::Unknown;
        }
        v.secured = v.secured || securedAbove_[i]; // it may arrive from a way where it is secured
        v.warned = false;
    }
    flow_.tickLine = 0;
    flow_.reachable = true;
}

void DilCheck::ifBegin()
{
    ifs_.push_back(flow_); // the way past the if when there is no else
}

void DilCheck::elseBegin()
{
    if (!ifs_.empty()) // the grammar always begins the if first
    {
        std::swap(flow_, ifs_.back()); // the else starts from the state before the if
    }
}

void DilCheck::ifEnd()
{
    if (!ifs_.empty())
    {
        merge(ifs_.back());
        ifs_.pop_back();
    }
}

void DilCheck::merge(const Flow &other)
{
    if (!other.reachable)
    {
        return;
    }
    if (!flow_.reachable)
    {
        flow_ = other;
        return;
    }
    for (size_t i = 0; i < flow_.vars.size(); i++)
    {
        VarState &v = flow_.vars[i];
        const VarState &o = other.vars[i];
        if (v.state != o.state)
        {
            v.state = State::Unknown;
        }
        v.secured = v.secured || o.secured;
        v.warned = v.warned && o.warned;
    }
    if (flow_.tickLine != other.tickLine)
    {
        flow_.tickLine = 0;
    }
}

void DilCheck::act(bool chNull, bool victNull, bool typeKnown, bool needsVictim, int line)
{
    // dil_act sends nothing when the unit the message is about is null
    if (!flow_.reachable || !typeKnown)
    {
        return;
    }
    if (needsVictim && victNull)
    {
        warn(line, "act() TO_VICT/TO_NOTVICT with null as the victim: nobody sees it");
    }
    else if (!needsVictim && chNull)
    {
        warn(line, "act() with null as the character: nobody sees it");
    }
}

void DilCheck::exec(const std::string &literal, bool concatenated, int line)
{
    if (!flow_.reachable || !concatenated || literal.empty())
    {
        return;
    }
    for (char c : literal)
    {
        if (!std::isalpha(static_cast<unsigned char>(c)))
        {
            return;
        }
    }
    warn(line, "exec(\"" + literal + "\"+...) has no space after \"" + literal + "\", so the command and what follows run together as one word");
}
