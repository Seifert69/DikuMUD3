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
}

DilCheck::Track *DilCheck::at(int var)
{
    return (var >= 0 && var < static_cast<int>(vars_.size())) ? &vars_[var] : nullptr;
}

void DilCheck::assign(int var)
{
    if (Track *v = at(var))
    {
        v->state = State::Fresh;
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
            report_(line,
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

    if (condition_ && (extra || !v->secured))
    {
        v->warned = true;
        report_(line,
                v->name + " is null in this condition: the condition is tested after the program wakes up, and waking up clears " +
                    (extra ? "every extraptr variable" : "unsecured unitptr variables (secure it first)"));
    }
    else if (v->state == State::Stale)
    {
        v->warned = true;
        report_(line,
                v->name + " is null here: it was set before the wait on line " + std::to_string(v->staleLine) +
                    (extra ? ", and a wait clears every extraptr variable" : " and isn't secured, and a wait clears unsecured unitptr variables"));
    }
    else if (v->state == State::Never && field)
    {
        v->warned = true;
        report_(line, v->name + " is used before it is set: it is still null here");
    }
}

void DilCheck::wait(bool tick, int line)
{
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
            report_(line, "secure(" + v->name + ") comes before " + v->name + " is set, so it secures nothing");
        }
        v->secured = true;
        v->everSecured = true;
    }
}

void DilCheck::unsecure(int var)
{
    if (Track *v = at(var))
    {
        v->secured = false;
    }
}

void DilCheck::boundary()
{
    for (Track &v : vars_)
    {
        if (v.state == State::Stale || v.state == State::Fresh)
        {
            v.state = State::Unknown;
        }
        v.secured = v.secured || v.everSecured; // it may arrive from a path where it is secured
        v.warned = false;
    }
    tickLine_ = 0;
}

void DilCheck::act(bool chNull, bool victNull, bool toVict, int line)
{
    if (toVict && victNull)
    {
        report_(line, "act() TO_VICT with null as the victim: nobody sees it");
    }
    else if (!toVict && chNull)
    {
        report_(line, "act() with null as the character: nobody sees it");
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
    report_(line, "exec(\"" + literal + "\"+...) has no space after \"" + literal + "\", so the command and what follows run together as one word");
}
