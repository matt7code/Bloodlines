#include "status.h"

Status::Status(int type, int duration, float power)
    : type(type)
    , power(power)
    , duration(duration) {
}

std::string Status::getName() {
    std::string name;
    switch(type) {
    case Status::STATUS_BLIND:
        name = "Blind";
        break; // Harmful statuses
    case Status::STATUS_SLEEP:
        name = "Asleep";
        break;
    case Status::STATUS_POISON:
        name = "Poisoned";
        break;
    case Status::STATUS_CONFUSION:
        name = "Confused";
        break;
    case Status::STATUS_POSSESS:
        name = "Possessed";
        break;
    case Status::STATUS_CURSE:
        name = "Cursed";
        break;
    case Status::STATUS_PETRIFY:
        name = "Petrified";
        break;
    case Status::STATUS_ROT:
        name = "Rotting";
        break;
    case Status::STATUS_BLESS:
        name = "Blessed";
        break; // Cleric
    case Status::STATUS_REGEN:
        name = "Regenerating";
        break;
    case Status::STATUS_HEROISM:
        name = "Heroic";
        break; // Warrior Shouts // increase hp
    case Status::STATUS_BRACE:
        name = "Braced";
        break; // reduce damage taken
    case Status::STATUS_FOCUS:
        name = "Focused";
        break; // increase attack
    case Status::STATUS_SNEAKING:
        name = "Sneaking";
        break; // Rogue
    case Status::STATUS_GUARD:
        name = "Guarding";
        break; // General
    case Status::STATUS_FLEE:
        name = "Fleeing";
        break;
    }
    return name;
}
