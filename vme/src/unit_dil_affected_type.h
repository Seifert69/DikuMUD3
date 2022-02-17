#pragma once

#include "essential.h"
#include "namelist.h"

class unit_dil_affected_type
{
    // TODO This is completely unused anywhere - but it must have been used for something
    //  once so I am leaving it here all private and uncreatable.
public:
    unit_dil_affected_type() = delete;

private:
    ubit16 beat{0};
    ubit16 duration{0};
    ubit16 transfer{0};
    cNamelist data;
    char *apply{nullptr};  // Apply DIL
    char *remove{nullptr}; // Remove DIL
    unit_dil_affected_type *next{nullptr};
};
