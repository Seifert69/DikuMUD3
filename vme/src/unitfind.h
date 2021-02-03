/*
 $Author: All $
 $RCSfile: unitfind.h,v $
 $Date: 2003/12/28 22:02:45 $
 $Revision: 2.3 $
 */

#ifndef _MUD_UNITFIND_H
#define _MUD_UNITFIND_H


#define UVI(i) (unit_vector.units[i])
#define UVITOP (unit_vector.top)

struct unit_vector_data
{
    class unit_data **units;
    int size;
    int top;
};

extern struct unit_vector_data unit_vector;

int follower_count (class unit_data * u);
class unit_data *get_follower (class unit_data * u, int num);
ubit1 same_surroundings (class unit_data * u1, class unit_data * u2);
void scan4_unit_room (class unit_data * room, ubit8 type);
void scan4_unit (class unit_data * ch, ubit8 type);
class unit_data *scan4_ref (class unit_data * ch, class unit_data * fu);


class unit_data *find_unit (const class unit_data * ch, char **arg,
                                const class unit_data * list,
                                const ubit32 bitvector);
class unit_data *find_unit_dil (const class unit_data * ch, char **arg,
                                    const class unit_data * list,
                                    const ubit32 bitvector, ubit8 type = 15);
class unit_data *find_unit_general(const class unit_data * viewer,
                                        const class unit_data * ch,
                                        char **arg,
                                        const class unit_data * list,
                                        const ubit32 bitvector,
                                        ubit8 type = FIND_UNIT);
class unit_data *find_symbolic(const char *zone, const char *name);
class unit_data *find_symbolic_idx(const char *zone, const char *name, int idx);
class unit_data *random_unit (class unit_data * ref, int sflags, int tflags);


#endif
