#pragma once

extern int g_nDilPrg;
extern int g_nDilVal;

int dilvalPoolFree();     // Blocks currently on the dilval pool's free-stack
int dilvalPoolCapacity(); // Allocated capacity of the pool's free-stack

bool DilVarTypeIsValid(int n); // Whether n is a DilVarType_e that a variable can have
DilVarType_e DilVarTypeIntToEnum(int n);
