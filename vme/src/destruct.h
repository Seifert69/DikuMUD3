#pragma once

#include "essential.h"

class basedestruct
{
public:
    basedestruct();
    virtual ~basedestruct();
    // virtual void clear_destruct(void);
    void register_destruct();
    int is_destructed();
    void undo_destruct();

private:
    virtual int destruct_classindex() = 0;
    ubit1 m_bDestructed;
};

void clear_destructed();
