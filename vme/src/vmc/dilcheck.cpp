#include "dilcheck.h"

#include <cctype>

DilCheck::DilCheck(Report report)
    : report_(std::move(report))
{
}

void DilCheck::begin(const std::vector<Var> &vars, int argc)
{
    vars_.clear();
    for (const auto &v : vars)
    {
        State state = static_cast<int>(vars_.size()) < argc ? State::Unknown : State::Never;
        vars_.push_back(Track{v.name, v.kind, state, false, false, 0, false});
    }
    securedAbove_.assign(vars_.size(), false);
    tickLine_ = 0;
    for (bool &w : eventWarned_)
    {
        w = false;
    }
    condition_ = false;
    quiet_ = false;
    held_.clear();
    condReads_.clear();
    ifs_.clear();
    unsecured_.clear();
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

DilCheck::Track *DilCheck::at(int var)
{
    return (var >= 0 && var < static_cast<int>(vars_.size())) ? &vars_[var] : nullptr;
}

void DilCheck::assign(int var, int from)
{
    if (Track *v = at(var))
    {
        // secure() protects a unit, not a variable: a copy of a secured variable survives waits
        // too, a variable set to anything else doesn't
        Track *f = at(from);
        v->state = State::Fresh;
        // from activator & co. it is often the same unit again (wait(..., activator == pc);
        // pc := activator), so a secured variable is taken to stay secured
        if (from >= -1)
        {
            v->secured = f && f->secured;
        }
        v->warned = false;
    }
}

void DilCheck::use(int var, bool field, int line)
{
    static const char *const eventNames[] = {"activator", "medium", "target", "argument"};

    if (quiet_)
    {
        return;
    }
    if (var < 0)
    {
        // activator, medium, target, argument after a pause belong to the tick
        int i = ACTIVATOR - var;
        if (tickLine_ && !condition_ && i >= 0 && i < 4 && !eventWarned_[i])
        {
            eventWarned_[i] = true;
            warn(line,
                 std::string(eventNames[i]) + (var == ARGUMENT ? " is empty here" : " is null here") + ": after the pause on line " +
                        std::to_string(tickLine_) + " the program was woken by a tick. Keep what you need in a variable before the pause");
        }
        return;
    }

    Track *v = at(var);
    if (!v || v->kind == VarKind::Other || v->warned)
    {
        return;
    }
    bool extra = (v->kind == VarKind::Extra);

    if (condition_)
    {
        condReads_.emplace_back(var, line);
    }
    else if (v->state == State::Stale)
    {
        v->warned = true;
        // when it was set on some way only, or above a label, a secure() further down may cover it
        warn(line,
             v->name + " is null here: it was set before the wait on line " + std::to_string(v->staleLine) +
                 (extra ? ", and a wait clears every extraptr variable" : " and isn't secured, and a wait clears unsecured unitptr variables"),
             (v->staleVague && !extra) ? var : -1);
    }
    else if (v->state == State::Never && field)
    {
        v->warned = true;
        warn(line, v->name + " is used before it is set: it is still null here");
    }
}

void DilCheck::ifBegin()
{
    ifs_.push_back({Snapshot{vars_, tickLine_, jumped_}, Snapshot{}});
    unsecured_.clear();
    jumped_ = false;
}

void DilCheck::elseBegin()
{
    if (ifs_.empty())
    {
        return;
    }
    ifs_.back().second = Snapshot{vars_, tickLine_, jumped_};
    vars_ = ifs_.back().first.vars;
    tickLine_ = ifs_.back().first.tickLine;
    unsecured_.clear();
    jumped_ = false;
}

void DilCheck::ifEnd(bool hasElse)
{
    if (ifs_.empty())
    {
        return;
    }
    Snapshot before = ifs_.back().first;
    Snapshot other = hasElse ? ifs_.back().second : before; // the then-branch, or no branch
    if (!hasElse)
    {
        other.jumped = false;
    }
    ifs_.pop_back();
    if (jumped_ && !other.jumped)
    {
        // this way jumped away: below the if only the other way arrives
        vars_ = other.vars;
        tickLine_ = other.tickLine;
        unsecured_.clear();
    }
    else if (!jumped_ && other.jumped)
    {
        unsecured_.clear();
    }
    else
    {
        merge(other);
    }
    jumped_ = before.jumped;
}

void DilCheck::merge(const Snapshot &other)
{
    for (size_t i = 0; i < vars_.size() && i < other.vars.size(); i++)
    {
        Track &v = vars_[i];
        const Track &o = other.vars[i];
        if (v.state != o.state)
        {
            v.state = State::Unknown;
        }
        v.secured = v.secured || o.secured;
        v.warned = v.warned && o.warned;
    }
    if (tickLine_ != other.tickLine)
    {
        tickLine_ = 0;
    }
    unsecured_.clear();
}

void DilCheck::wait(bool tick, int line)
{
    // the conditions read since the last wait are tested from now on, after every wake-up
    for (const auto &r : condReads_)
    {
        Track *v = at(r.first);
        if (v && !v->warned && (v->kind == VarKind::Extra || !v->secured))
        {
            v->warned = true;
            warn(r.second,
                 v->name + " is null in this condition: the condition is tested after the program wakes up, and waking up clears " +
                     (v->kind == VarKind::Extra ? "every extraptr variable" : "unsecured unitptr variables (secure it first)"),
                 v->kind == VarKind::Extra ? -1 : r.first);
        }
    }
    condReads_.clear();
    unsecured_.clear();
    tickLine_ = tick ? line : 0;
    if (tick)
    {
        for (bool &w : eventWarned_)
        {
            w = false;
        }
    }
    for (Track &v : vars_)
    {
        bool cleared = (v.kind == VarKind::Extra) || (v.kind == VarKind::Unit && !v.secured);
        if (cleared && (v.state == State::Fresh || v.state == State::Unknown))
        {
            v.staleVague = (v.state == State::Unknown);
            v.state = State::Stale;
            v.staleLine = line;
            v.warned = false;
        }
    }
}

void DilCheck::secure(int var, int line)
{
    if (Track *v = at(var))
    {
        if (v->state == State::Never)
        {
            warn(line, "secure(" + v->name + ") comes before " + v->name + " is set, so it secures nothing");
        }
        else if (v->state == State::Stale)
        {
            warn(line, "secure(" + v->name + ") secures nothing: " + v->name + " was cleared by the wait on line " + std::to_string(v->staleLine));
        }
        v->secured = true;
        securedAbove_[var] = true;
    }
}

void DilCheck::unsecure(int var)
{
    if (Track *v = at(var))
    {
        if (v->secured)
        {
            unsecured_.push_back(var);
        }
        v->secured = false;
    }
}

void DilCheck::jump()
{
    jumped_ = true;
    for (int var : unsecured_)
    {
        vars_[var].secured = true;
    }
    unsecured_.clear();
}

void DilCheck::boundary(bool label)
{
    for (Track &v : vars_)
    {
        // a label can be reached from further down, where it may be set
        if (v.state == State::Stale || v.state == State::Fresh || (label && v.state == State::Never))
        {
            v.state = State::Unknown;
        }
        v.secured = v.secured || securedAbove_[&v - &vars_[0]]; // it may arrive from a way where it is secured
        v.warned = false;
    }
    unsecured_.clear();
    tickLine_ = 0;
    jumped_ = false;
}

void DilCheck::act(bool chNull, bool victNull, bool typeKnown, bool needsVictim, int line)
{
    // dil_act sends nothing when the unit the message is about is null
    if (typeKnown && needsVictim && victNull)
    {
        warn(line, "act() TO_VICT/TO_NOTVICT with null as the victim: nobody sees it");
    }
    else if (typeKnown && !needsVictim && chNull)
    {
        warn(line, "act() with null as the character: nobody sees it");
    }
}

void DilCheck::exec(const std::string &literal, bool concatenated, int line)
{
    if (!concatenated || literal.empty())
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
