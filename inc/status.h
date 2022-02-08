#ifndef STATUS_H
#define STATUS_H

#include <iostream>
#include <sstream>
#include "special.h"

struct Status {
    enum {
        // General
        STATUS_NONE,
        STATUS_GUARD,
        STATUS_FLEE,

        // Monster to player
        STATUS_SLEEP,
        STATUS_CONFUSION,
        STATUS_POSSESS,
        STATUS_ROT,
        // Mage
        STATUS_PETRIFY,
        STATUS_SLOWED,
        STATUS_HASTED,
        // Cleric
        STATUS_BLIND, // Harmful statuses
        STATUS_POISON,
        STATUS_CURSE,
        STATUS_BLESS,
        STATUS_REGEN,
        // Warrior
        STATUS_HEROISM, // Warrior Shouts // increase hp
        STATUS_BRACE, // reduce damage taken
        STATUS_FOCUS, // increase attack
         // Rogue
        STATUS_SNEAKING,
         // End
        STATUS_END
    };

    Status() {};
    Status(int type, int duration = 1, float power = 1.0f);
    virtual ~Status() {};

    std::string getName();
    int type;
    float power;
    int duration;
};

#endif // STATUS_H
