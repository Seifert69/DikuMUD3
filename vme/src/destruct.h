#pragma once

#include "essential.h"

#define DR_UNIT 0
#define DR_AFFECT 1
#define DR_FUNC 2

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
