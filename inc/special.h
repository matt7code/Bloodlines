#ifndef HEADER_SPELL_H
#define HEADER_SPELL_H

#include <SFML/Graphics.hpp>

/// See: specials.db

//namespace MP_COST {
//    const float SPELL_MISSILE           =  4.0f;
//    const float SPELL_LIGHTING_BOLT     =  8.0f;
//    const float SPELL_BURN              = 15.0f;
//    const float SPELL_FIREBALL          = 21.0f;
//    const float SPELL_GUSH_WATER        = 24.0f;
//    const float SPELL_SLOW              = 27.0f;
//    const float SPELL_HASTE             = 30.0f;
//    const float SPELL_EARTHQUAKE        = 33.0f;
//    const float SPELL_FIRE_STORM        = 36.0f;
//    const float SPELL_LIGHTING_STORM    = 39.0f;
//    const float SPELL_SNOW_STORM        = 42.0f;
//    const float SPELL_FLOOD             = 45.0f;
//    const float SPELL_PETRIFY           = 48.0f;
//    const float SPELL_VOLCANO           = 51.0f;
//    const float SPELL_ASTEROID          = 54.0f;
//}
namespace MP_COST {

    const float SPELL_NONE              = 0.0f;

    const float SPELL_HEROISM           = 3.0f;
    const float SPELL_BRACE             = 3.0f;
    const float SPELL_FOCUS             = 3.0f;
    const float SPELL_CHALLENGE         = 3.0f;
    const float SPELL_BASH              = 3.0f;

    const float SPELL_EXAMINE           = 3.0f;
    const float SPELL_EXPOSE_WEAKNESS   = 3.0f;
    const float SPELL_PEEK              = 3.0f;
    const float SPELL_SNARE             = 3.0f;
    const float SPELL_SNEAK             = 3.0f;
    const float SPELL_STEAL             = 3.0f;

    const float SPELL_MISSILE           = 3.0f;
    const float SPELL_LIGHTNING_BOLT     = 3.0f;
    const float SPELL_BURN              = 3.0f;
    const float SPELL_FIREBALL          = 3.0f;
    const float SPELL_GUSH_WATER        = 3.0f;
    const float SPELL_SLOW              = 3.0f;
    const float SPELL_HASTE             = 3.0f;
    const float SPELL_EARTHQUAKE        = 3.0f;
    const float SPELL_FIRE_STORM        = 3.0f;
    const float SPELL_LIGHTNING_STORM    = 3.0f;
    const float SPELL_SNOW_STORM        = 3.0f;
    const float SPELL_FLOOD             = 3.0f;
    const float SPELL_PETRIFY           = 3.0f;
    const float SPELL_VOLCANO           = 3.0f;
    const float SPELL_ASTEROID          = 3.0f;

    const float SPELL_HEAL              = 3.0f;
    const float SPELL_BLESS             = 3.0f;
    const float SPELL_CURSE             = 3.0f;
    const float SPELL_SICK              = 3.0f;
    const float SPELL_BLIND             = 3.0f;
    const float SPELL_CURE              = 3.0f;
    const float SPELL_RESTORE           = 3.0f;
    const float SPELL_REMOVECURSE       = 3.0f;
    const float SPELL_REGEN             = 3.0f;
    const float SPELL_POISON            = 3.0f;
    const float SPELL_RESURRECT         = 3.0f;
    const float SPELL_BLESS_ALL         = 3.0f;
    const float SPELL_HEAL_ALL          = 3.0f;
    const float SPELL_REGEN_ALL         = 3.0f;
    const float SPELL_RES_ALL           = 3.0f;
    const float SPELL_RESTORE_ALL       = 3.0f;
}

struct Special {

    enum {
        // Monster spells?
        SPELL_NONE = 0,
        // Warrior
        SPELL_HEROISM = 100,
        SPELL_BRACE,
        SPELL_FOCUS,
        SPELL_CHALLENGE,
        SPELL_BASH,
        // Rogue
        SPELL_EXAMINE = 200,
        SPELL_EXPOSE_WEAKNESS,
        SPELL_PEEK,
        SPELL_SNARE,
        SPELL_SNEAK,
        SPELL_STEAL,
        // Wizard
        SPELL_MISSILE = 300,
        SPELL_LIGHTNING_BOLT,
        SPELL_BURN,
        SPELL_FIREBALL,
        SPELL_GUSH_WATER,
        SPELL_SLOW,
        SPELL_HASTE,
        SPELL_EARTHQUAKE,
        SPELL_FIRE_STORM,
        SPELL_LIGHTNING_STORM,
        SPELL_SNOW_STORM,
        SPELL_FLOOD,
        SPELL_PETRIFY,
        SPELL_VOLCANO,
        SPELL_ASTEROID,
        // Cleric
        SPELL_HEAL = 400,
        SPELL_BLESS,
        SPELL_CURSE,
        SPELL_SICK,
        SPELL_BLIND,
        SPELL_CURE,
        SPELL_RESTORE,
        SPELL_REMOVECURSE,
        SPELL_REGEN,
        SPELL_POISON,
        SPELL_RESURRECT,
        SPELL_BLESS_ALL,
        SPELL_HEAL_ALL,
        SPELL_REGEN_ALL,
        SPELL_RES_ALL,
        SPELL_RESTORE_ALL,
        // End of spells
        SPELL_END = 1000
    };

    Special(int spell_num = 0);
    virtual ~Special() {};

    int             m_id            = 0;
    bool            m_areaOfEffect  = false;
    unsigned        m_damage        = 0;
    std::string     m_tag           = "UNKNOWN";
    std::string     m_name          = "Unknown";
    std::string     m_desc          = "Unknown";
    unsigned        m_level         = 0;
    float           m_cost          = 0.0f;
};

#endif // SPELL_H
