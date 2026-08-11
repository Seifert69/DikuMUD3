#pragma once

extern int g_nDilPrg;
extern int g_nDilVal;

int dilvalPoolFree();     // Blocks currently on the dilval pool's free-stack
int dilvalPoolCapacity(); // Allocated capacity of the pool's free-stack

DilVarType_e DilVarTypeIntToEnum(int n);
