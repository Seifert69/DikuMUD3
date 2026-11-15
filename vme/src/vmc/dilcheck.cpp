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
        vars_.push_back(Track{v.name, v.kind, state, false, false, false, 0});
    }
    tickLine_ = 0;
    for (bool &w : eventWarned_)
    {
        w = false;
    }
    condition_ = false;
    quiet_ = false;
    held_.clear();
    condReads_.clear();
    waitAddrs_.clear();
    unsecured_.clear();
}

void DilCheck::end()
{
    for (const Held &h : held_)
    {
        Track *v = at(h.unlessSecured);
        if (!v || !v->everSecured)
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
        v->secured = f && f->secured;
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
        warn(line,
                v->name + " is null here: it was set before the wait on line " + std::to_string(v->staleLine) +
                    (extra ? ", and a wait clears every extraptr variable" : " and isn't secured, and a wait clears unsecured unitptr variables"));
    }
    else if (v->state == State::Never && field)
    {
        v->warned = true;
        warn(line, v->name + " is used before it is set: it is still null here");
    }
}

void DilCheck::branches(unsigned from, unsigned to)
{
    for (unsigned a : waitAddrs_)
    {
        if (a >= from && a < to)
        {
            boundary();
            return;
        }
    }
}

void DilCheck::wait(bool tick, int line, unsigned addr)
{
    waitAddrs_.push_back(addr);
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
        v->everSecured = true;
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
        v.secured = v.secured || v.everSecured; // it may arrive from a path where it is secured
        v.warned = false;
    }
    unsecured_.clear();
    tickLine_ = 0;
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
