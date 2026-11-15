#include "dilcheck.h"

#include <cctype>

namespace
{
constexpr int TO_VICT_ = 1; // vme.h: TO_VICT

const char *eventName(int var)
{
    switch (var)
    {
        case DilCheck::ACTIVATOR:
            return "activator";
        case DilCheck::MEDIUM:
            return "medium";
        case DilCheck::TARGET:
            return "target";
        default:
            return "argument";
    }
}
} // namespace

DilCheck::DilCheck(Report report)
    : report_(std::move(report))
{
}

void DilCheck::begin(const std::vector<std::string> &names, const std::vector<VarKind> &kinds, int argc)
{
    names_ = names;
    kinds_ = kinds;
    kinds_.resize(names_.size(), VarKind::Other);
    state_.assign(names_.size(), State::Never);
    for (int i = 0; i < argc && i < static_cast<int>(state_.size()); i++)
    {
        state_[i] = State::Unknown;
    }
    secured_.assign(names_.size(), Secured::No);
    everSecured_.assign(names_.size(), false);
    warned_.assign(names_.size(), false);
    staleLine_.assign(names_.size(), 0);
    afterTick_ = false;
    tickLine_ = 0;
    for (bool &w : eventWarned_)
    {
        w = false;
    }
    condition_ = 0;
    quiet_ = 0;
}

std::string DilCheck::name(int var) const
{
    return (var >= 0 && var < static_cast<int>(names_.size())) ? names_[var] : std::string("?");
}

void DilCheck::assign(int var)
{
    if (var < 0 || var >= static_cast<int>(state_.size()))
    {
        return;
    }
    state_[var] = State::Fresh;
    warned_[var] = false;
}

void DilCheck::use(int var, bool field, int line)
{
    if (quiet_ > 0)
    {
        return;
    }
    if (var < 0)
    {
        // activator, medium, target, argument after a pause belong to the tick
        int i = ACTIVATOR - var;
        if (afterTick_ && condition_ == 0 && i >= 0 && i < 4 && !eventWarned_[i])
        {
            eventWarned_[i] = true;
            report_(line,
                    std::string(eventName(var)) + (var == ARGUMENT ? " is empty here" : " is null here") +
                        ": after the pause on line " + std::to_string(tickLine_) +
                        " the program was woken by a tick. Keep what you need in a variable before the pause");
        }
        return;
    }
    if (var >= static_cast<int>(state_.size()) || kinds_[var] == VarKind::Other || warned_[var])
    {
        return;
    }

    if (condition_ > 0 && (kinds_[var] == VarKind::Extra || secured_[var] == Secured::No))
    {
        warned_[var] = true;
        report_(line,
                name(var) + " is null in this condition: the condition is tested after the program wakes up, and waking up clears " +
                    (kinds_[var] == VarKind::Extra ? std::string("every extraptr variable") : std::string("unsecured unitptr variables")) +
                    (kinds_[var] == VarKind::Extra ? "" : " (secure it first)"));
        return;
    }

    if (state_[var] == State::Stale)
    {
        warned_[var] = true;
        report_(line,
                name(var) + " is null here: it was set before the wait on line " + std::to_string(staleLine_[var]) +
                    (kinds_[var] == VarKind::Extra ? ", and a wait clears every extraptr variable"
                                                   : " and isn't secured, and a wait clears unsecured unitptr variables"));
    }
    else if (state_[var] == State::Never && field)
    {
        warned_[var] = true;
        report_(line, name(var) + " is used before it is set: it is still null here");
    }
}

void DilCheck::wait(bool tick, int line)
{
    afterTick_ = tick;
    if (tick)
    {
        tickLine_ = line;
        for (bool &w : eventWarned_)
        {
            w = false;
        }
    }
    for (size_t i = 0; i < state_.size(); i++)
    {
        bool cleared = (kinds_[i] == VarKind::Extra) || (kinds_[i] == VarKind::Unit && secured_[i] == Secured::No);
        if (cleared && (state_[i] == State::Fresh || state_[i] == State::Unknown))
        {
            state_[i] = State::Stale;
            staleLine_[i] = line;
            warned_[i] = false;
        }
    }
}

void DilCheck::conditionBegin()
{
    condition_++;
}

void DilCheck::conditionEnd()
{
    if (condition_ > 0)
    {
        condition_--;
    }
}

void DilCheck::secure(int var, int line)
{
    if (var < 0 || var >= static_cast<int>(state_.size()))
    {
        return;
    }
    if (state_[var] == State::Never)
    {
        report_(line, "secure(" + name(var) + ") comes before " + name(var) + " is set, so it secures nothing");
    }
    secured_[var] = Secured::Yes;
    everSecured_[var] = true;
}

void DilCheck::unsecure(int var)
{
    if (var >= 0 && var < static_cast<int>(secured_.size()))
    {
        secured_[var] = Secured::No;
    }
}

void DilCheck::quietBegin()
{
    quiet_++;
}

void DilCheck::quietEnd()
{
    if (quiet_ > 0)
    {
        quiet_--;
    }
}

void DilCheck::boundary()
{
    for (size_t i = 0; i < state_.size(); i++)
    {
        if (state_[i] == State::Stale || state_[i] == State::Fresh)
        {
            state_[i] = State::Unknown;
        }
        if (everSecured_[i] && secured_[i] == Secured::No)
        {
            secured_[i] = Secured::Maybe;
        }
        warned_[i] = false;
    }
    afterTick_ = false;
}

void DilCheck::act(bool chNull, bool victNull, int type, int line)
{
    if (type == TO_VICT_ && victNull)
    {
        report_(line, "act() TO_VICT with null as the victim: nobody sees it");
    }
    else if (type != TO_VICT_ && chNull)
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
