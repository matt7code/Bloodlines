#include "special.h"

Special::Special( int id ) {
    m_id = id;
    switch (m_id) {
    case SPELL_NONE:
        m_tag  = "NONE";
        m_name = "None";
        m_desc = "None.";
        m_cost = 0.0f;
        break;
    case SPELL_HEAL:
        m_tag  = "HEAL";
        m_name = "Heal";
        m_desc = "Heals a single target.";
        m_cost = MP_COST::SPELL_HEAL;
        break;
    case SPELL_BLESS:
        m_tag  = "BLESS";
        m_name = "Bless";
        m_desc = "Blesses the target, increasing their defensive and offensive abilities.";
        m_cost = MP_COST::SPELL_BLESS;
        break;
    case SPELL_CURSE:
        m_tag  = "CURSE";
        m_name = "Curse";
        m_desc = "Curses the target, lowering their defense and offense.";
        m_cost = MP_COST::SPELL_CURSE;
        break;
    case SPELL_SICK:
        m_tag  = "SICK";
        m_name = "Sick";
        m_desc = "Causes the target to fall ill, weakening them, and causing periodic damage.";
        m_cost = MP_COST::SPELL_SICK;
        break;
    case SPELL_BLIND:
        m_tag  = "BLIND";
        m_name = "Blind";
        m_desc = "Blinds the target, causing their attacks to miss.";
        m_cost = MP_COST::SPELL_BLIND;
        break;
    case SPELL_CURE:
        m_tag  = "CURE";
        m_name = "Cure";
        m_desc = "Cures illness, poison, and blindness, but not curses.";
        m_cost = MP_COST::SPELL_CURE;
        break;
    case SPELL_RESTORE:
        m_tag  = "RESTORE";
        m_name = "Restore";
        m_desc = "Cures all status ailments and heals a small amount of health.";
        m_cost = MP_COST::SPELL_RESTORE;
        break;
    case SPELL_REMOVECURSE:
        m_tag  = "REMOVE_CURSE";
        m_name = "Remove Curse";
        m_desc = "Removes curses. A powerful curse needs a powerful removal.";
        m_cost = MP_COST::SPELL_REMOVECURSE;
        break;
    case SPELL_REGEN:
        m_tag  = "REGEN";
        m_name = "Regen";
        m_desc = "Periodically restores health over time.";
        m_cost = MP_COST::SPELL_REGEN;
        break;
    case SPELL_POISON:
        m_tag  = "POISON";
        m_name = "Poison";
        m_desc = "Poisons the target, causing periodic damage.";
        m_cost = MP_COST::SPELL_POISON;
        break;
    case SPELL_RESURRECT:
        m_tag  = "RESURRECT";
        m_name = "Raise Dead";
        m_desc = "Restore life to a dead player.";
        m_cost = MP_COST::SPELL_RESURRECT;
        break;
    case SPELL_GUSH_WATER:
        m_tag  = "GUSH_WATER";
        m_name = "Surge";
        m_desc = "Cause a massive wave to hit a single target causing a good deal of water damage.";
        m_cost = MP_COST::SPELL_GUSH_WATER;
        break;
    case SPELL_BLESS_ALL:
        m_tag  = "BLESS_ALL";
        m_name = "Bless All";
        m_desc = "Bless the entire group.";
        m_cost = MP_COST::SPELL_BLESS_ALL;
        break;
    case SPELL_HEAL_ALL:
        m_tag  = "HEAL_ALL";
        m_name = "Heal All";
        m_desc = "A direct heal for each living member of the party.";
        m_cost = MP_COST::SPELL_HEAL_ALL;
        break;
    case SPELL_REGEN_ALL:
        m_tag  = "REGEN_ALL";
        m_name = "Regen All";
        m_desc = "Restores health periodically to all party members.";
        m_cost = MP_COST::SPELL_REGEN_ALL;
        break;
    case SPELL_RES_ALL:
        m_tag  = "RAISE_ALL";
        m_name = "Raise All";
        m_desc = "Restore life to all deceased party members.";
        m_cost = MP_COST::SPELL_RES_ALL;
        break;
    case SPELL_RESTORE_ALL:
        m_tag  = "RESTORE_ALL";
        m_name = "Restore All";
        m_desc = "A large group heal applied to each party member.";
        m_cost = MP_COST::SPELL_RESTORE_ALL;
        break;
    // Wizard
    case SPELL_MISSILE:
        m_tag  = "MISSLE";
        m_name = "Missile";
        m_desc = "A bolt of magical energy.";
        m_cost = MP_COST::SPELL_MISSILE;
        break;
    case SPELL_LIGHTNING_BOLT:
        m_tag  = "LIGHTNING_BOLT";
        m_name = "Shock";
        m_desc = "A direct attack causing a single target a large amount of electrical damage.";
        m_cost = MP_COST::SPELL_LIGHTNING_BOLT;
        break;
    case SPELL_BURN:
        m_tag  = "BURN";
        m_name = "Burn";
        m_desc = "Severe single target fire attack.";
        m_cost = MP_COST::SPELL_BURN;
        break;
    case SPELL_FIREBALL:
        m_tag  = "FIREBALL";
        m_name = "Fireball";
        m_desc = "An fiery explosion damaging every enemy in the area.";
        m_cost = MP_COST::SPELL_FIREBALL;
        break;
    case SPELL_SLOW:
        m_tag  = "SLOW";
        m_name = "Slow";
        m_desc = "Slows the speed of an enemies attacks.";
        m_cost = MP_COST::SPELL_SLOW;
        break;
    case SPELL_HASTE:
        m_tag  = "HASTE";
        m_name = "Haste";
        m_desc = "Increase the attack speed of the target.";
        m_cost = MP_COST::SPELL_HASTE;
        break;
    case SPELL_EARTHQUAKE:
        m_tag  = "EARTHQUAKE";
        m_name = "Earthquake";
        m_desc = "A large amount of physical damage to all enemies.";
        m_cost = MP_COST::SPELL_EARTHQUAKE;
        break;
    case SPELL_FIRE_STORM:
        m_tag  = "FIRESTORM";
        m_name = "Firestorm";
        m_desc = "A cloud of burning embers blankets the enemy party, causing considerable fire damage.";
        m_cost = MP_COST::SPELL_FIRE_STORM;
        break;
    case SPELL_LIGHTNING_STORM:
        m_tag  = "STORM";
        m_name = "Storm";
        m_desc = "A severe storm strikes the enemy party, causing massive electrical damage.";
        m_cost = MP_COST::SPELL_LIGHTNING_STORM;
        break;
    case SPELL_SNOW_STORM:
        m_tag  = "BLIZZARD";
        m_name = "Blizzard";
        m_desc = "Intense cold freezes the enemies.";
        m_cost = MP_COST::SPELL_SNOW_STORM;
        break;
    case SPELL_FLOOD:
        m_tag  = "FLOOD";
        m_name = "Flood";
        m_desc = "Water floods the area, causing drowning damage to all enemies.";
        m_cost = MP_COST::SPELL_FLOOD;
        break;
    case SPELL_PETRIFY:
        m_tag  = "PETRIFY";
        m_name = "Petrify";
        m_desc = "Turn an enemy to stone.";
        m_cost = MP_COST::SPELL_PETRIFY;
        break;
    case SPELL_VOLCANO:
        m_tag  = "VOLCANO";
        m_name = "Volcano";
        m_desc = "Causes the the ground to spew lava, ash, smoke, rocks - burning and crushing the enemy.";
        m_cost = MP_COST::SPELL_VOLCANO;
        break;
    case SPELL_ASTEROID:
        m_tag  = "FALLING_STAR";
        m_name = "Falling Star";
        m_desc = "Summon a massive boulder from the sky, to crash down on the enemy party.";
        m_cost = MP_COST::SPELL_ASTEROID;
        break;
    // Shouts
    case SPELL_HEROISM:
        m_tag  = "HEROISM";
        m_name = "Heroism";
        m_desc = "Bolster the partys physical attack power with a heroic battle cry.";
        m_cost = MP_COST::SPELL_HEROISM;
        break;
    case SPELL_BRACE:
        m_tag  = "BRACE";
        m_name = "Brace";
        m_desc = "Rally the parties physical defenses with well timed orders and alerts.";
        m_cost = MP_COST::SPELL_BRACE;
        break;
    case SPELL_FOCUS:
        m_tag  = "FOCUS";
        m_name = "Focus";
        m_desc = "Calling out situational advice which will increase both physical and magical attacks.";
        m_cost = MP_COST::SPELL_FOCUS;
        break;
    case SPELL_CHALLENGE:
        m_tag  = "CHALLENGE";
        m_name = "Challenge";
        m_desc = "Force the enemy to target you.";
        m_cost = MP_COST::SPELL_CHALLENGE;
        break;
    case SPELL_BASH:
        m_tag  = "BASH";
        m_name = "Bash";
        m_desc = "Rush an enemy, causing additional physical damage.";
        m_cost = MP_COST::SPELL_BASH;
        break;
    // Rogue Abilities
    case SPELL_STEAL:
        m_tag  = "STEAL_ITEM";
        m_name = "Steal Item";
        m_desc = "Steal a random item from the target.";
        m_cost = MP_COST::SPELL_STEAL;
        break;
    case SPELL_SNEAK:
        m_tag  = "SNEAK";
        m_name = "Sneak";
        m_desc = "Sink into the shadows, adding a massive bonus to physical damage.";
        m_cost = MP_COST::SPELL_SNEAK;
        break;
    case SPELL_SNARE:
        m_tag  = "SNARE";
        m_name = "Snare";
        m_desc = "Pin an enemy to the ground, preventing the enemies from fleeing.";
        m_cost = MP_COST::SPELL_SNARE;
        break;
    case SPELL_PEEK:
        m_tag  = "PEEK";
        m_name = "Peek";
        m_desc = "Check the enemy's inventory, see what they are carrying.";
        m_cost = MP_COST::SPELL_PEEK;
        break;
    case SPELL_EXAMINE:
        m_tag  = "EXAMINE";
        m_name = "Examine";
        m_desc = "Discover information about the target, possibly including current and maximum health.";
        m_cost = MP_COST::SPELL_EXAMINE;
        break;
    case SPELL_EXPOSE_WEAKNESS:
        m_tag  = "EXPOSE_WEAKNESS";
        m_name = "Expose Weakness";
        m_desc = "Reveal any vulnerabilities the target may have.";
        m_cost = MP_COST::SPELL_EXPOSE_WEAKNESS;
        break;
    default:
        m_tag  = "UNKNOWN";
        m_name = "Unknown";
        m_desc = "No description available.";
        m_cost = 0.0f;
        break;
    }
}
