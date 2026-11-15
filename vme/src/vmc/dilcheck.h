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
    /// The template is done: reports what was held back until the whole template was seen.
    void end();
    /// var := from (from is the plain variable on the right, or -1).
    void assign(int var, int from);
    /// A read of var (field: through a field, as in pc.name).
    void use(int var, bool field, int line);
    /// A wait statement; tick is true when it waits for ticks only (a pause).
    void wait(bool tick, int line);
    /// An if statement: its branches start from the state before it, and below it a variable keeps
    /// only what both ways agree on (ifEnd's hasElse: false when the other way is no branch).
    void ifBegin();
    void elseBegin();
    void ifEnd(bool hasElse);
    /// Reads while this is on are in a wait or interrupt condition: they are checked at the next
    /// wait, which is when the condition is first tested after the program wakes up.
    void setCondition(bool on) { condition_ = on; }
    /// Reads while this is on are not checked (the argument of unsecure()).
    void setQuiet(bool on) { quiet_ = on; }
    void secure(int var, int line);
    void unsecure(int var);
    /// goto, break, continue, quit or return: the path ends here, so its unsecure()s since the last wait don't
    /// reach the code below (`if (...) { unsecure(pc); goto start; }`).
    void jump();
    /// The end of a loop, or (label) a label or loop start: code may arrive here from elsewhere.
    void boundary(bool label = false);

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

    struct Track
    {
        std::string name;
        VarKind kind;
        State state;
        bool secured;     ///< secured, or (after a block end or label) secured somewhere above
        bool warned;      ///< warned about since its state last changed
        int staleLine;    ///< the wait that made it stale
        bool staleVague;  ///< it was only Unknown before that wait (set on some way, or above a label)
    };

    struct Snapshot
    {
        std::vector<Track> vars;
        int tickLine;
        bool jumped; ///< the way ended in a goto/quit/return/break/continue: it doesn't reach below the if
    };
    void merge(const Snapshot &other);

    Track *at(int var);
    /// A warning; with unlessSecured >= 0 it is dropped if that variable is secured anywhere.
    void warn(int line, const std::string &msg, int unlessSecured = -1);

    struct Held
    {
        int line;
        std::string msg;
        int unlessSecured;
    };

    Report report_;
    std::vector<Held> held_;
    std::vector<std::pair<int, int>> condReads_; ///< (variable, line) read in a condition
    std::vector<Track> vars_;
    std::vector<bool> securedAbove_; ///< secure() seen above in the text (on any way)
    std::vector<std::pair<Snapshot, Snapshot>> ifs_; ///< per open if: the state before it, and at the end of its then-branch
    std::vector<int> unsecured_; ///< unsecure()d since the last wait or label
    bool jumped_{false};         ///< the code since the last if/else start ended in a jump
    int tickLine_{0}; ///< the pause the program last woke from, 0 when the last wait wasn't a pause
    bool eventWarned_[4]{};
    bool condition_{false};
    bool quiet_{false};
};
