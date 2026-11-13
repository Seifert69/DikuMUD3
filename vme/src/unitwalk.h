#pragma once

#include "unit_data.h"

/// What a visit of walk_contents() / walk_surroundings() asks for next
enum class eWalk
{
    on,   ///< go on, and look inside the unit if it is transparent
    skip, ///< go on, but don't look inside the unit
    stop  ///< end the walk at this unit
};

/// The units in room and, for each transparent unit there that isn't skip, the units inside it,
/// in list order. visit(u, inside) is called for each: inside is the transparent unit u was found
/// in, nullptr at the room's own level (where its answer decides whether to look inside u; inside
/// a unit only stop counts). The next unit is taken before u is visited, so a visit may move u.
/// Returns the unit the walk stopped at, or nullptr when it went through.
template<typename VISIT>
unit_data *walk_contents(const unit_data *room, const unit_data *skip, VISIT &&visit)
{
    unit_data *next = nullptr;
    for (unit_data *u = const_cast<unit_data *>(room->getUnitContains()); u; u = next)
    {
        next = u->getNext();
        eWalk w = visit(u, static_cast<unit_data *>(nullptr));
        if (w == eWalk::stop)
        {
            return u;
        }
        if ((w == eWalk::on) && (u != skip) && u->isTransparent())
        {
            unit_data *nextt = nullptr;
            for (unit_data *uu = u->getUnitContains(); uu; uu = nextt)
            {
                nextt = uu->getNext();
                if (visit(uu, u) == eWalk::stop)
                {
                    return uu;
                }
            }
        }
    }
    return nullptr;
}

/// The units around ch, as the game sees them: walk_contents() of the room ch is in, then, when
/// that room is a transparent unit inside another, walk_contents() of the room around it (where
/// ch's room itself is visited, but not looked inside again). visit(u, inside, outer): outer is
/// true for the room around. Returns the unit the walk stopped at, or nullptr.
template<typename VISIT>
unit_data *walk_surroundings(const unit_data *ch, VISIT &&visit)
{
    const unit_data *room = ch->getUnitIn();
    if (room == nullptr)
    {
        return nullptr;
    }

    unit_data *at = walk_contents(room, nullptr, [&](unit_data *u, unit_data *inside) { return visit(u, inside, false); });
    if (at || !room->isTransparent() || (room->getUnitIn() == nullptr))
    {
        return at;
    }
    return walk_contents(room->getUnitIn(), room, [&](unit_data *u, unit_data *inside) { return visit(u, inside, true); });
}
