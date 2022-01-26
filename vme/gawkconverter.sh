#!/bin/bash
#
# This expression will find all WEAPON_DEF() and replace the
# values with new BONUS_ macros as a first step in cleaning up
# zones.
#
# If you want to see the whole file, just remove the grep at the 
# end of this script.
#
gawk  -i inplace '
    # ...
    function getFieldValue(value) {
        if (value <= -7)
            return "BONUS_JUNK";
        if (value <= -6)
            return "BONUS_JUNK_ALMOST";
        if (value <= -5)
            return "BONUS_TERRIBLE_MINUS";
        if (value <= -4)
            return "BONUS_TERRIBLE";
        if (value <= -3)
            return "BONUS_BAD_MINUS";
        if (value <= -2)
            return "BONUS_BAD";
        if (value <= -1)
            return "BONUS_AVERAGE_MINUS";
        if (value == 0)
            return "BONUS_AVERAGE";
        if (value <= 1)
            return "BONUS_AVERAGE_PLUS";
        if (value <= 4)
            return "BONUS_GOOD";
        if (value <= 5)
            return "BONUS_GOOD_PLUS";
        if (value <= 7)
            return "BONUS_EXCELLENT";
        if (value <= 10)
            return "BONUS_EXCELLENT_PLUS";
        if (value <= 15)
            return "BONUS_SUPERIOR";
        if (value >= 16)
            return "BONUS_ARTIFACT";
        return "ERRORXXX";
    }

    {
        if (match($0, /^[ ]*(WEAPON_DEF|SHIELD_DEF)[ ]*\([ ]*([A-Z0-9_]+)[ ]*,[ ]*([-]?[+]?[0-9]+)[ ]*,[ ]*([-]?[+]?[0-9]+)[ ]*\)([.]*)/, a))
        {
            print a[1] "(" a[2]", " getFieldValue(a[3])", " getFieldValue(a[4])")"a[5] "  // Xyzzy: " a[3] "," a[4];
        }
        else
        {
            print $0;
        }
    }

' *.zon


gawk  -i inplace '
    # ...
    function getFieldValue(value) {
        if (value <= -7)
            return "BONUS_JUNK";
        if (value <= -6)
            return "BONUS_JUNK_ALMOST";
        if (value <= -5)
            return "BONUS_TERRIBLE_MINUS";
        if (value <= -4)
            return "BONUS_TERRIBLE";
        if (value <= -3)
            return "BONUS_BAD_MINUS";
        if (value <= -2)
            return "BONUS_BAD";
        if (value <= -1)
            return "BONUS_AVERAGE_MINUS";
        if (value == 0)
            return "BONUS_AVERAGE";
        if (value <= 1)
            return "BONUS_AVERAGE_PLUS";
        if (value <= 4)
            return "BONUS_GOOD";
        if (value <= 5)
            return "BONUS_GOOD_PLUS";
        if (value <= 7)
            return "BONUS_EXCELLENT";
        if (value <= 10)
            return "BONUS_EXCELLENT_PLUS";
        if (value <= 15)
            return "BONUS_SUPERIOR";
        if (value >= 16)
            return "BONUS_ARTIFACT";
        return "ERRORXXX";
    }  

    {
        if (match($0, /^[ ]*(ARMOUR_LEATHER|ARMOUR_PLATE|ARMOUR_CLOTHES|ARMOUR_HRD_LEATHER|ARMOUR_CHAIN)[ ]*\([ ]*([-]?[+]?[0-9]+)[ ]*,[ ]*([-]*[+]*[0-9]+)[ ]*\)([.]*)/, a))
        {
            print a[1] "(" getFieldValue(a[2])", " getFieldValue(a[3])")"a[4] "  // Xyzzy: " a[2] "," a[3];
        }
        else
        {
            print $0;
        }
    }

' *.zon


gawk  -i inplace '
    # ...
    function getFieldValue(value) {
        if (value <= -7)
            return "BONUS_JUNK";
        if (value <= -6)
            return "BONUS_JUNK_ALMOST";
        if (value <= -5)
            return "BONUS_TERRIBLE_MINUS";
        if (value <= -4)
            return "BONUS_TERRIBLE";
        if (value <= -3)
            return "BONUS_BAD_MINUS";
        if (value <= -2)
            return "BONUS_BAD";
        if (value <= -1)
            return "BONUS_AVERAGE_MINUS";
        if (value == 0)
            return "BONUS_AVERAGE";
        if (value <= 1)
            return "BONUS_AVERAGE_PLUS";
        if (value <= 2)
            return "BONUS_GOOD";
        if (value <= 3)
            return "BONUS_GOOD_PLUS";
        if (value <= 4)
            return "BONUS_EXCELLENT";
        if (value <= 5)
            return "BONUS_EXCELLENT_PLUS";
        if (value <= 9)
            return "BONUS_SUPERIOR";
        if (value >= 10)
            return "BONUS_ARTIFACT";
        return "ERRORXXX";
    }  

    {
        if (match($0, /^[ ]*(SKILL_TRANSFER|WEAPON_TRANSFER|SPELL_TRANSFER)[ ]*\([ ]*([A-Z0-9_]+)[ ]*,[ ]*([-]?[+]?[0-9]+)[ ]*\)([.]*)/, a))
        {
            print a[1] "(" a[2] ", " getFieldValue(a[3])")"a[4] "  // Xyzzy: " a[3];
        }
        else
        {
            print $0;
        }
    }

' *.zon

gawk  -i inplace '
    # ...
    function getFieldValue(value) {
        if (value <= -7)
            return "BONUS_JUNK";
        if (value <= -6)
            return "BONUS_JUNK_ALMOST";
        if (value <= -5)
            return "BONUS_TERRIBLE_MINUS";
        if (value <= -4)
            return "BONUS_TERRIBLE";
        if (value <= -3)
            return "BONUS_BAD_MINUS";
        if (value <= -2)
            return "BONUS_BAD";
        if (value <= -1)
            return "BONUS_AVERAGE_MINUS";
        if (value == 0)
            return "BONUS_AVERAGE";
        if (value <= 1)
            return "BONUS_AVERAGE_PLUS";
        if (value <= 2)
            return "BONUS_GOOD";
        if (value <= 3)
            return "BONUS_GOOD_PLUS";
        if (value <= 4)
            return "BONUS_EXCELLENT";
        if (value <= 6)
            return "BONUS_EXCELLENT_PLUS";
        if (value <= 9)
            return "BONUS_SUPERIOR";
        if (value >= 10)
            return "BONUS_ARTIFACT";
        return "ERRORXXX";
    }  

    {
        if (match($0, /^[ ]*(DEX_TRANSFER|STR_TRANSFER|CON_TRANSFER|CHA_TRANSFER|BRA_TRANSFER|MAG_TRANSFER|DIV_TRANSFER|HIT_TRANSFER)[ ]*\([ ]*([-]?[+]?[0-9]+)[ ]*\)([.]*)/, a))
        {
            print a[1] "(" getFieldValue(a[2])")"a[3] "  // Xyzzy: " a[2];
        }
        else
        {
            print $0;
        }
    }

' *.zon
