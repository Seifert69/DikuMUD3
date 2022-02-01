/*
 $Author: All $
 $RCSfile: money.h,v $
 $Date: 2003/12/28 22:02:44 $
 $Revision: 2.2 $
 */
#pragma once

#include "structs.h"
#include "utils.h"

typedef sbit16 currency_t;
typedef sbit32 amount_t;

struct money_type
{
    currency_t currency;     /* Which currency for money */
    char **strings;          /* The money-strings */
    char *tails;             /* What to print on the dark side of the coin */
    char *abbrev;            /* Small string for lists */
    ubit16 pl_idx;           /* The index in above to first plural string */
    sbit32 relative_value;   /* Relative to the internal value */
    sbit32 min_value;        /* Minimum internal value of the currency */
    ubit16 coins_per_weight; /* How many coins per weight unit */
    file_index_type *fi;     /* Where is coin object in file */
};

#define money_round_up(a, c, t) (money_round(TRUE, (a), (c), (t)))
#define money_round_down(a, c, t) (money_round(FALSE, (a), (c), (t)))
#define money_pluralis_type(type) (g_money_types[(type)].strings[g_money_types[(type)].pl_idx])
#define money_pluralis(unit) (money_pluralis_type(MONEY_TYPE(unit)))

#define money_singularis_type(type) (g_money_types[(type)].strings[0])
#define money_singularis(unit) (money_singularis_type(MONEY_TYPE(unit)))

#define money_from_unit(unit, amt, currency) (money_transfer((unit), NULL, (amt), (currency)))

#define money_to_unit(unit, amt, currency) (money_transfer(NULL, (unit), (amt), (currency)))

#define IS_MONEY(unit) (IS_OBJ(unit) && OBJ_TYPE(unit) == ITEM_MONEY)

#define MONEY_AMOUNT(unit) ((amount_t)OBJ_PRICE(unit))

/* Index into money-array */
#define MONEY_TYPE(obj) (OBJ_VALUE((obj), 0))

#define MONEY_CURRENCY(obj) (g_money_types[MONEY_TYPE(obj)].currency)

#define MONEY_RELATIVE(obj) (g_money_types[MONEY_TYPE(obj)].relative_value)

#define MONEY_MIN_VALUE(obj) (g_money_types[MONEY_TYPE(obj)].min_value)

#define MONEY_WEIGHT(obj) (g_money_types[MONEY_TYPE(obj)].coins_per_weight)

#define MONEY_VALUE(obj) (MONEY_AMOUNT(obj) * MONEY_RELATIVE(obj))

#define MONEY_WEIGHT_SUM(obj1, obj2) ((MONEY_AMOUNT(obj1) + MONEY_AMOUNT(obj2)) / MONEY_WEIGHT(obj1))

/* Local currency of unit, or DEF_CURRENCY if not defined.
 */
currency_t local_currency(unit_data *unit);

/* Print out representation of money-object with the amount amt .
 * (amt == 0 means all)
 */
char *obj_money_string(unit_data *obj, amount_t amt);

/* Print out optimal representation of amt in currency
 *
 * Impossible amounts are converted automagically
 */
char *money_string(amount_t amt, currency_t currency, ubit1 verbose);

/* How many `coins' of given money-object can char carry, resp. unit contain
 *   (Naturally the amount of money is an upper bound)
 */
amount_t char_can_carry_amount(unit_data *ch, unit_data *money);
amount_t unit_can_hold_amount(unit_data *unit, unit_data *money);

/* Give a number of coins (of a specific type) to unit
 * (type == -1 means money_to_unit with local_currency(unit)
 *  Used for database backwards compatibility...)
 */
void coins_to_unit(unit_data *, amount_t amt, int type);

/* Use this if an amount needs to be physically transfered.
 * Money is created/deleted if either unit is NULL.
 *
 * Impossible amounts are converted automagically
 */
void money_transfer(unit_data *from, unit_data *to, amount_t amt, currency_t currency);

/*  Counts up what amount of a given currency a unit holds recursively in
 *  inventory.
 *  Use ANY_CURRENCY as currency-type to count up ALL money...
 */
amount_t unit_holds_total(unit_data *u, currency_t currency);

/*  Counts up what amount of a given currency char holds in inventory.
 *  Use ANY_CURRENCY as currency-type to count up ALL money...
 */
amount_t char_holds_amount(unit_data *ch, currency_t currency);

/* Can char afford amt in currency?
 *
 * Impossible amounts are converted automagically
 */
ubit1 char_can_afford(unit_data *ch, amount_t amt, currency_t currency);

/* Does unit contain any money of type?
 */
unit_data *unit_has_money_type(unit_data *unit, ubit16 type);

/* Split `money' into two objects.  Return object with amount `amt'
 */
unit_data *split_money(unit_data *money, amount_t amt);

/* Set all the values on money correctly according to amount - return money
 * In general: DON'T use, as the db handles this correctly...
 */
unit_data *set_money(unit_data *money, amount_t amt);

/* Check to see if UNIT_IN(money) contains any money of same type, and
 * if so, merge the piles
 */
void pile_money(unit_data *money);

/*  Round amount down/up to nearest `types' number of coins
 */
amount_t money_round(ubit1 up, amount_t amt, currency_t currency, int types);

#ifdef VMC_SRC
void boot_money(char *moneyfile);
#else
void boot_money();
#endif

/* You shouldn't have to refer to this array yourself.
 * Use the macros instead.
 */
extern money_type g_money_types[];
