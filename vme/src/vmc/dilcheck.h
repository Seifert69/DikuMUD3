#pragma once

#include <functional>
#include <string>
#include <vector>

/**
 * Warnings vmc gives while compiling a DIL template, for mistakes that compile but don't do what
 * they say. The parser (dilpar.y) calls these as it reduces the template, so they follow the
 * text: straight-line code between a wait and the next block end or label. Anything that
 * depends on a jump is forgotten at a block end or a label, which keeps false alarms down.
 *
 * What the server does that the checks rely on (dilrun.cpp, dil_clear_non_secured): every time a
 * program wakes up, before even its wait condition is tested, unitptr variables that don't point
 * at a secured unit and all extraptr variables are set to null; after a pause (a wait on ticks
 * only) activator, medium, target and argument belong to the tick, so they are null or empty.
 */
class DilCheck
{
public:
    enum class VarKind
    {
        Other,
        Unit,
        Extra
    };

    /// Built-in variables passed to use(); real variables are their index (0 and up).
    static constexpr int ACTIVATOR = -2;
    static constexpr int MEDIUM = -3;
    static constexpr int TARGET = -4;
    static constexpr int ARGUMENT = -5;

    using Report = std::function<void(int line, const std::string &msg)>;

    explicit DilCheck(Report report);

    /// A new template: its variables (the first argc are its arguments).
    void begin(const std::vector<std::string> &names, const std::vector<VarKind> &kinds, int argc);
    void assign(int var);
    /// A read of var (field: through a field, as in pc.name).
    void use(int var, bool field, int line);
    /// A wait statement; tick is true when it waits for ticks only (a pause).
    void wait(bool tick, int line);
    /// Reads between these are in a wait or interrupt condition.
    void conditionBegin();
    void conditionEnd();
    void secure(int var, int line);
    void unsecure(int var);
    /// Reads between these are not checked (the argument of unsecure()).
    void quietBegin();
    void quietEnd();
    /// A block end or a label: code may arrive here from elsewhere.
    void boundary();

    /// act(): ch/vict say whether those arguments are the constant null; type is the TO_ value or -1.
    void act(bool chNull, bool victNull, int type, int line);
    /// exec(): the first argument, when it starts with a string literal and adds something to it.
    void exec(const std::string &literal, bool concatenated, int line);

private:
    enum class State
    {
        Never,   ///< not set anywhere above (a local, so null)
        Unknown, ///< set somewhere above, or an argument
        Fresh,   ///< set since the last wait
        Stale    ///< set before a wait and not secured: null now
    };

    std::string name(int var) const;

    Report report_;
    std::vector<std::string> names_;
    std::vector<VarKind> kinds_;
    std::vector<State> state_;
    enum class Secured
    {
        No,
        Yes,
        Maybe ///< after a label or block end, when it was secured somewhere above
    };
    std::vector<Secured> secured_;
    std::vector<bool> everSecured_;
    std::vector<bool> warned_;
    std::vector<int> staleLine_;
    bool afterTick_{false};
    int tickLine_{0};
    bool eventWarned_[4]{};
    int condition_{0};
    int quiet_{0};
};
