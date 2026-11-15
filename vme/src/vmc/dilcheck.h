#pragma once

#include <functional>
#include <string>
#include <vector>

/**
 * Warnings vmc gives while compiling a DIL template, for mistakes that compile but don't do what
 * they say. The parser (dilpar.y) calls these as it reduces the template, so they follow the
 * text. Each branch of an if starts from the state before the if; after the if the two ways are
 * joined, leaning towards no warning when they differ (a way that jumps away doesn't count).
 * Labels and loops forget what was known, since code may arrive there from further down. That
 * keeps false alarms down.
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

    /// Built-in variables passed to use() and assign(); real variables are their index (0 and up).
    static constexpr int ACTIVATOR = -2;
    static constexpr int MEDIUM = -3;
    static constexpr int TARGET = -4;
    static constexpr int ARGUMENT = -5;

    using Report = std::function<void(int line, const std::string &msg)>;

    explicit DilCheck(Report report);

    /// A new template: its variables (the first argc are its arguments).
    void begin(const std::vector<Var> &vars, int argc);
    /// The template is done: reports what was held back until the whole template was seen.
    void end();
    /// var := from (from is the plain variable or built-in on the right, or -1).
    void assign(int var, int from);
    /// A read of var (field: through a field, as in pc.name).
    void use(int var, bool field, int line);
    /// A wait statement; tick is true when it waits for ticks only (a pause).
    void wait(bool tick, int line);
    /// Reads while this is on are in a wait (wait true) or interrupt condition: they are checked at
    /// the next wait, which is when the condition is first tested after the program wakes up.
    void setCondition(bool on, bool wait = false)
    {
        condition_ = on;
        waitCondition_ = on && wait;
    }
    /// a == b in a condition, each a variable or built-in (-1 when neither).
    void equal(int a, int b);
    /// Reads while this is on are not checked (the argument of unsecure()).
    void setQuiet(bool on) { quiet_ = on; }
    void secure(int var, int line);
    void unsecure(int var);
    /// goto, break, continue, quit or return: the code right after it is not reached this way.
    void jump();
    /// A label: code may arrive here from anywhere.
    void label();
    /// The start (where the condition is tested again) and the end of a while or foreach loop.
    void loopBegin();
    void loopEnd();
    /// An if statement: the else (if any) and the end of the if.
    void ifBegin();
    void elseBegin();
    void ifEnd();

    /// act(): chNull/victNull say whether those arguments are the constant null; needsVictim is
    /// true for a constant TO_VICT or TO_NOTVICT, typeKnown false when the type isn't a constant.
    void act(bool chNull, bool victNull, bool typeKnown, bool needsVictim, int line);
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

    /// What is known about a variable at a point of the code (copied for each if).
    struct VarState
    {
        State state;
        bool secured;    ///< secured, or, after a label or loop, secured somewhere above
        bool warned;     ///< warned about since its state last changed
        bool staleVague; ///< a unitptr that was only Unknown before the wait that made it stale
        int staleLine;   ///< the wait that made it stale
    };

    /// What is known at a point of the code.
    struct Flow
    {
        std::vector<VarState> vars;
        int tickLine{0};       ///< the pause the program last woke from, 0 when the last wait wasn't a pause
        bool reachable{true}; ///< false right after a jump
        bool eventWarned[4]{}; ///< activator, medium, target, argument warned about since that pause
        std::vector<int> sameAsActivator; ///< variables the last wait's condition required == activator & co.
    };

    struct Held
    {
        int line;
        std::string msg;
        int unlessSecured;
    };

    VarState *at(int var);
    /// A warning; with unlessSecured >= 0 it is dropped if that variable is secured anywhere.
    void warn(int line, const std::string &msg, int unlessSecured = -1);
    /// flow_ becomes what holds after either flow_ or other.
    void merge(const Flow &other);
    /// Code may arrive here from further down: forget what was known (label: from anywhere).
    void forget(bool label);

    Report report_;
    std::vector<Var> vars_;          ///< names and kinds
    std::vector<bool> securedAbove_; ///< secure() seen so far in the text (on any way): "above" at a
                                     ///< label, "anywhere in the template" at end()
    Flow flow_;
    std::vector<Flow> ifs_; ///< per open if: the other way (no branch yet, or the then-branch once in the else)
    std::vector<Held> held_;
    std::vector<bool> loops_; ///< per open loop: whether its start is reachable
    std::vector<std::pair<int, int>> condReads_; ///< (variable, line) read in a condition since the last wait
    std::vector<int> condSame_;                  ///< variables compared == activator & co. in this wait's condition
    bool condition_{false};
    bool waitCondition_{false};
    bool quiet_{false};
};
