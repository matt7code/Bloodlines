#ifndef COMBAT_ACTION_H_INCLUDED
#define COMBAT_ACTION_H_INCLUDED

struct Action {
    enum action {
        COMMAND_ATTACK,
        COMMAND_SPECIAL,
        COMMAND_USE,
        COMMAND_GUARD,
        COMMAND_STANCE,
        COMMAND_FLEE
    };

    unsigned actor          = 0; // Who's doing it.
    unsigned action         = 0; // What did they do?
    unsigned sub_action     = 0; // A spell or item id.
    unsigned target         = 0; // And to whom?
    std::vector<unsigned> targets; // And to whom(s)?

    std::string getName() {
        std::string name = "Unknown Action";
        if(action == COMMAND_ATTACK) name = "Melee Attack";
        else if(action == COMMAND_SPECIAL) name = "Shout/Spell/Prayer/Trick";
        else if(action == COMMAND_USE) name = "Use";
        else if(action == COMMAND_GUARD) name = "Guard";
        else if(action == COMMAND_STANCE) name = "Stance";
        else if(action == COMMAND_FLEE) name = "Flee";
        return name;
    }
};

#endif // COMBAT_ACTION_H_INCLUDED
