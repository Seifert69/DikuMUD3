/*
 $Author: tperry $
 $RCSfile: spec_assign.h,v $
 $Date: 2001/04/29 03:46:07 $
 $Revision: 2.1 $
 */
#pragma once

#include "interpreter.h"

// For use in spec_assign.c's g_unit_function_array[]
#define SD_NEVER 0 // Never save this function
#define SD_NULL 1  // Ignore fptr->data (save as 0 ptr)
#define SD_ASCII 2 // If pointer, then it's ascii char *

extern unit_function_array_type g_unit_function_array[];
