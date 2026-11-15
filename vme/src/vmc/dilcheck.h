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

    struct Var
    {
        std::string name;
        VarKind kind;
    };

    /// Built-in variables passed to use(); real variables are their index (0 and up).
    static constexpr int ACTIVATOR = -2;
    static constexpr int MEDIUM = -3;
    static constexpr int TARGET = -4;
    static constexpr int ARGUMENT = -5;

    using Report = std::function<void(int line, const std::string &msg)>;

    explicit DilCheck(Report report);

    /// A new template: its variables (the first argc are its arguments).
    void begin(const std::vector<Var> &vars, int argc);
    void assign(int var);
    /// A read of var (field: through a field, as in pc.name).
    void use(int var, bool field, int line);
    /// A wait statement; tick is true when it waits for ticks only (a pause).
    void wait(bool tick, int line);
    /// Reads while this is on are in a wait or interrupt condition.
    void setCondition(bool on) { condition_ = on; }
    /// Reads while this is on are not checked (the argument of unsecure()).
    void setQuiet(bool on) { quiet_ = on; }
    void secure(int var, int line);
    void unsecure(int var);
    /// A block end or a label: code may arrive here from elsewhere.
    void boundary();

    /// act(): chNull/victNull say whether those arguments are the constant null.
    void act(bool chNull, bool victNull, bool toVict, int line);
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

    struct Track
    {
        std::string name;
        VarKind kind;
        State state;
        bool secured;     ///< secured, or (after a block end or label) secured somewhere above
        bool everSecured; ///< secure() seen above
        bool warned;      ///< warned about since its state last changed
        int staleLine;    ///< the wait that made it stale
    };

    Track *at(int var);

    Report report_;
    std::vector<Track> vars_;
    int tickLine_{0}; ///< the pause the program last woke from, 0 when the last wait wasn't a pause
    bool eventWarned_[4]{};
    bool condition_{false};
    bool quiet_{false};
};
