#pragma once

#include "essential.h"

class basedestruct
{
public:
    basedestruct(void);
    virtual ~basedestruct();
    // virtual void clear_destruct(void);
    void register_destruct(void);
    int is_destructed(void);
    void undo_destruct(void);

private:
    virtual int destruct_classindex(void) = 0;
    ubit1 m_bDestructed;
};

void clear_destructed(void);
