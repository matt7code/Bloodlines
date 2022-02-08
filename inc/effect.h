#ifndef EFFECT_H_INCLUDED
#define EFFECT_H_INCLUDED

struct Effect {
    Effect() { reset(); }
    enum {
        EFFECT_NONE,
        EFFECT_HEAL,
        EFFECT_STATUS,
        EFFECT_MAGIC,
        EFFECT_MELEE,
        EFFECT_HEAT,
        EFFECT_COLD,
        EFFECT_ELEC,
        EFFECT_ACID
    };

    float physical;
    float magic;
    float heat;
    float cold;
    float electric;
    float acid;

    void reset()
    {
        physical  = 0.0f;
        magic     = 0.0f;
        heat      = 0.0f;
        cold      = 0.0f;
        electric  = 0.0f;
        acid      = 0.0f;
    }
    float getTotal()
    {
        return physical + magic + heat + cold + electric + acid;
    }
};

#endif // EFFECT
