#include "hero.h"

Hero::Hero(int level, int myclass) {
    for(int i = 0; i < 100; ++i) m_xpNeeded.push_back(std::pow(i * 10, 2.5));
    m_level = level;
    m_class = myclass;

    switch (m_class) {
    case CLASS_NONE: // 18
        m_str       = 10;
        m_int       = 10;
        m_agi       = 10;
        break;
    case CLASS_WARRIOR: // 18
        m_str       = 8;
        m_int       = 4;
        m_agi       = 6;
        getSpells().push_back(Special::SPELL_HEROISM);
        break;
    case CLASS_WIZARD: // 18
        m_str       = 4;
        m_int       = 8;
        m_agi       = 6;
        getSpells().push_back(Special::SPELL_MISSILE);
        break;
    case CLASS_ROGUE: // 18
        m_str       = 6;
        m_int       = 4;
        m_agi       = 8;
        getSpells().push_back(Special::SPELL_EXAMINE);
        break;
    case CLASS_CLERIC:
        m_str       = 4;
        m_int       = 8;
        m_agi       = 6;
        getSpells().push_back(Special::SPELL_HEAL);
        break;
    }
    m_defPhys   = (m_agi * level) / 2;
    m_defMagi   = (m_agi * level) / 2;
    m_atkPhys   = m_str * level;
    m_atkMagi   = m_int * level;

    m_maxHP = (m_level + 1) * m_str;
    m_maxMP = (m_level + 1) * m_int;
    m_curHP = m_maxHP;
    m_curHP = m_maxMP;
}

void Hero::SetFacingByVector(sf::Vector2f mv) {
    if (mv.x == 0.0f && mv.y == 0.0f) return;

    if (mv.x == 0.0f) { // Moved on the Y axis.
        if(mv.y < 0.0f) m_facing = DIR_NORTH;
        else m_facing = DIR_SOUTH;
    } else { // Moved on the X axis.
        if(mv.x < 0.0f) m_facing = DIR_WEST;
        else m_facing = DIR_EAST;
    }
    m_sprite.setTextureRect(sf::IntRect(textureSheetBase.x + (m_animSet[m_animFrame] * TILE_SIZE_X), textureSheetBase.y + (m_facing * TILE_SIZE_Y), TILE_SIZE_X, TILE_SIZE_Y));
}

void Hero::AnimateWalk(float seconds) {
    m_AnimSecs += seconds;
    if (m_AnimSecs > 0.25f) {
        m_AnimSecs = 0.0f;
        m_animFrame++;
        if(m_animFrame == 4) m_animFrame = 0;
    }
}

std::string Hero::getClassName(int i) {
    if(i == CLASS_NONE)     return "None";
    if(i == CLASS_WARRIOR)  return "Warrior";
    if(i == CLASS_WIZARD)   return "Wizard";
    if(i == CLASS_ROGUE)    return "Rogue";
    if(i == CLASS_CLERIC)   return "Cleric";
    return "Unknown";
}

float Hero::getExpTNL() {
    return m_xpNeeded[(int)m_level] - m_exp;
}

void Hero::setAvatar(int type) {
    m_avatar = type;
    textureSheetBase.x = ((m_avatar % NUMER_OF_CHARACTERS_PER_ROW) * CHARACTER_CELLS_X * CHARACTER_TILE_X);
    textureSheetBase.y = ((m_avatar / NUMER_OF_CHARACTERS_PER_ROW) * CHARACTER_CELLS_Y * CHARACTER_TILE_Y);
    m_sprite.setTextureRect(sf::IntRect(textureSheetBase.x + (m_animSet[m_animFrame] * TILE_SIZE_X), textureSheetBase.y + (m_facing * TILE_SIZE_Y), TILE_SIZE_X, TILE_SIZE_Y));
}

float Hero::TakeDamage(Effect incoming) {
    float damage = 0.0f;
    if(incoming.physical - getDefPhys() > 0.0f) damage += incoming.physical - getDefPhys();
    if(incoming.magic    - getDefMagi() > 0.0f) damage += incoming.magic    - getDefMagi();
    if(incoming.heat     - getDefHeat() > 0.0f) damage += incoming.heat     - getDefHeat();
    if(incoming.cold     - getDefCold() > 0.0f) damage += incoming.cold     - getDefCold();
    if(incoming.electric - getDefElec() > 0.0f) damage += incoming.electric - getDefElec();
    if(incoming.acid     - getDefAcid() > 0.0f) damage += incoming.acid     - getDefAcid();

    // Damage cap
    if (damage > 999999) damage = 999999;
    m_curHP -= damage;
    if(m_curHP < 0) m_curHP = 0;
    return damage;
}

float Hero::TakeHeal(float amount) {
    if (isActive() && isAlive()) m_curHP += amount;
    if(m_curHP > getMaxHP()) {
        amount -= m_curHP - getMaxHP();
        m_curHP = getMaxHP();
    }
    return amount;
}

void Hero::setLevel(int val) {
    m_level = val;
    m_maxHP = (m_level + 1) * m_str;
    m_maxMP = (m_level + 1) * m_int;
}

/// @brief Apply a status effect from power of 1-1000
///     most of the effects are multiplied by 0.1f
///
/// @param type A STATUS_TYPE
/// @param duration Duration in rounds
/// @param power The power (1-1000, generally)
void Hero::TakeStatus(Status incoming) {
    m_Statuses.push_back(incoming);
    std::cerr << "\tStatus Applied: " << incoming.getName() << " (" << incoming.type << ") Power: " << incoming.power << " Duration: " << incoming.duration << std::endl;

    switch(incoming.type) {
    case Status::STATUS_HASTED:
        m_isHasted = true;
        break;
    case Status::STATUS_SLOWED:
        m_isSlowed = true;
        break;
    case Status::STATUS_SLEEP:
        isSleeping = true;
        break;
    case Status::STATUS_CONFUSION:
        isConfused = true;
        break;
    case Status::STATUS_POSSESS:
        isPossessed = true;
        break;
    case Status::STATUS_GUARD: // General
        m_bonusDPhys += GUARD_EFFECT;
        break;
    case Status::STATUS_FLEE:
        m_bonusDPhys -= FLEE_EFFECT;
        break;
    case Status::STATUS_PETRIFY:
        isPetrified = true;
        m_bonusDPhys += PETRIFY_EFFECT;
        break;
    case Status::STATUS_BLIND:
        m_bonusStr -= BLIND_EFFECT * incoming.power;
        m_bonusInt -= BLIND_EFFECT * incoming.power;
        m_bonusAgi -= BLIND_EFFECT * incoming.power;
        break;
    case Status::STATUS_CURSE:
        m_bonusStr -= CURSE_EFFECT * incoming.power;
        m_bonusInt -= CURSE_EFFECT * incoming.power;
        m_bonusAgi -= CURSE_EFFECT * incoming.power;
        break;
    case Status::STATUS_BLESS: // Cleric
        m_bonusStr += BLESS_EFFECT * incoming.power;
        m_bonusInt += BLESS_EFFECT * incoming.power;
        m_bonusAgi += BLESS_EFFECT * incoming.power;
        break;
    case Status::STATUS_HEROISM: // Warrior Shouts // increase hp
        m_bonusMaxHP += HEROISM_EFFECT * incoming.power;
        break;
    case Status::STATUS_BRACE: // reduce damage taken
        m_bonusDPhys += BRACE_EFFECT * incoming.power;
        m_bonusDMagi += BRACE_EFFECT * incoming.power;
        break;
    case Status::STATUS_FOCUS: // increase attack
        m_bonusStr += FOCUS_EFFECT * incoming.power;
        break;
    case Status::STATUS_SNEAKING: // Rogue
        m_bonusDPhys += SNEAK_EFFECT * incoming.power;
        m_bonusStr   += 4.0f * SNEAK_EFFECT * incoming.power;
        break;
    default:
        break;
    }
}

void Hero::UpdateStatus() {
    for(auto it = m_Statuses.begin(); it != m_Statuses.end();) {
        // Periodic Effects
        if(it->duration > 0) {
            Effect periodic_damage;
            std::cout << "\tStatus Update: " << getName() << "'s " << it->getName() << " has " << it->duration << " rounds remaining." << std::endl;
            switch(it->type) {
            case Status::STATUS_POISON:
                periodic_damage.physical = 1.0f * it->power;
                TakeDamage(periodic_damage);
                break;
            case Status::STATUS_ROT:
                periodic_damage.magic = 10.0f * it->power;
                TakeDamage(periodic_damage);
                break;
            case Status::STATUS_REGEN:
                std::cout << "Regen: " << getName() << " heals for " << (4.0f * it->power) << std::endl;
                TakeHeal(4.0f * it->power);
                break;
            default:
                std::cerr << "\t[Hero::UpdateStatus] Unhandled Status (Periodic Effects): " << it->type << " (" << it->getName() << ")" << std::endl;
                break;
            }
            it->duration -= 1;
            ++it;
        } else { // Wore off
            std::cerr << "\tStatus Removed: " << it->getName() << " (" << it->type << ")" << std::endl;
            switch(it->type) {
            case Status::STATUS_HASTED:
                m_isHasted = false;
                break;
            case Status::STATUS_SLOWED:
                m_isSlowed = false;
                break;
            case Status::STATUS_SLEEP:
                isSleeping = false;
                break;
            case Status::STATUS_CONFUSION:
                isConfused = false;
                break;
            case Status::STATUS_POSSESS:
                isPossessed = false;
                break;
            case Status::STATUS_PETRIFY:
                isPetrified = false;
                m_bonusDPhys -= PETRIFY_EFFECT;
                break;
            case Status::STATUS_GUARD: // General
                m_bonusDPhys -= GUARD_EFFECT;
                break;
            case Status::STATUS_FLEE:
                m_bonusDPhys += FLEE_EFFECT;
                break;
            case Status::STATUS_BLIND:
                m_bonusStr += BLIND_EFFECT * it->power;
                m_bonusInt += BLIND_EFFECT * it->power;
                m_bonusAgi += BLIND_EFFECT * it->power;
                break;
            case Status::STATUS_CURSE:
                m_bonusStr += CURSE_EFFECT * it->power;
                m_bonusInt += CURSE_EFFECT * it->power;
                m_bonusAgi += CURSE_EFFECT * it->power;
                break;
            case Status::STATUS_BLESS: // Cleric
                m_bonusDPhys -= BLESS_EFFECT * it->power;
                m_bonusDMagi -= BLESS_EFFECT * it->power;
                break;
            case Status::STATUS_HEROISM: // Warrior Shouts // increase hp
                m_bonusMaxHP -= HEROISM_EFFECT * it->power;
                break;
            case Status::STATUS_BRACE: // reduce damage taken
                m_bonusDPhys -= BRACE_EFFECT * it->power;
                m_bonusDMagi -= BLESS_EFFECT * it->power;
                break;
            case Status::STATUS_FOCUS: // increase attack
                m_bonusStr     -= FOCUS_EFFECT * it->power;
                break;
            case Status::STATUS_SNEAKING: // Rogue
                m_bonusDPhys -= SNEAK_EFFECT * it->power;
                m_bonusStr -= 4.0f * SNEAK_EFFECT * it->power;
                break;
            default:
                std::cerr << "\t[Hero::UpdateStatus] Unhandled Status (Wore off): " << it->type << " (" << it->getName() << ")" << std::endl;
                break;
            }
            m_Statuses.erase(it);
        }
    }
}

void Hero::UpdateAppearance() {
    textureSheetBase.x = ((m_avatar % NUMER_OF_CHARACTERS_PER_ROW) * CHARACTER_CELLS_X * CHARACTER_TILE_X);
    textureSheetBase.y = ((m_avatar / NUMER_OF_CHARACTERS_PER_ROW) * CHARACTER_CELLS_Y * CHARACTER_TILE_Y);
    m_sprite.setTextureRect(sf::IntRect(textureSheetBase.x + (m_animSet[m_animFrame] * TILE_SIZE_X), textureSheetBase.y + (m_facing * TILE_SIZE_Y), TILE_SIZE_X, TILE_SIZE_Y));
}

int Hero::getInventoryAmount(int item) {
    // Carried Items
    for (auto it = m_Inventory.begin(); it != m_Inventory.end(); ++it) {
        if (it->type == item) return it->amount;
    }
    return 0;
}

void Hero::invTake(int item, int amount) {
    for (auto it = m_Inventory.begin(); it != m_Inventory.end();) {
        if (it->type != item) ++it;
        else {
            it->amount -= amount;
            if (it->amount <= 0) m_Inventory.erase(it);
            else ++it;
        }
    }
}

void Hero::invGive(int item, int amount) {
    bool found = false;
    for (auto it = m_Inventory.begin(); it != m_Inventory.end(); ++it) {
        if (it->type == item) {
            it->amount += amount;
            found = true;
        }
    }
    if (!found) {
        std::cout << "Adding new entry to local inventory for item #" << item << std::endl;
        CarriedItem newItem;
        newItem.type = item;
        newItem.amount = amount;
        m_Inventory.push_back(newItem);
    }
}

void Hero::setInventoryAmount(int item, int amount) {
    bool found = false;
    for (auto it = m_Inventory.begin(); it != m_Inventory.end(); ++it) {
        if (it->type == item) {
            found = true;
            it->amount = amount;
        }
    }
    if (!found) {
        CarriedItem newItem;
        newItem.type = item;
        newItem.amount = amount;
        m_Inventory.push_back(newItem);
    }
}

void Hero::invEquip(int item, int slot, bool incomingTwoHanded, bool outgoingTwoHanded) {
    if(slot == Item::Slot_Hand_R || slot == Item::Slot_Hand_L) {
        if(incomingTwoHanded) {
            if(outgoingTwoHanded) {
                invRemove(4, true);
            } else {
                invRemove(4);
                invRemove(5);
            }
            m_Equipped[4] = item;
            m_Equipped[5] = item;
            invTake(item, 1);
        } else {
            if(outgoingTwoHanded) {
                invRemove(4, true);
                m_Equipped[4] = item;
            } else {
                if(m_Equipped[4] == 0) m_Equipped[4] = item;
                else if(m_Equipped[5] == 0) m_Equipped[5] = item;
                else {
                    if (m_Equipped[4] != 0) invGive(m_Equipped[4], 1);
                    m_Equipped[4] = item;
                }
            }
            invTake(item, 1);
        }
    } else if (slot == Item::Slot_Ring_R || slot == Item::Slot_Ring_L) {
        //std::cout << "Equipping Ring" << std::endl;
        //if(item == 0) m_Equipped[slot] =
        if(m_Equipped[6] == item) {

            if(m_Equipped[7] != 0) invGive(m_Equipped[7], 1);
            m_Equipped[7] = item;
        } else if(m_Equipped[6] == 0) m_Equipped[6] = item;
        else if(m_Equipped[7] == 0) m_Equipped[7] = item;
        else {
            if (m_Equipped[6] != 0) invGive(m_Equipped[6], 1);
            m_Equipped[6] = item;
        }
        invTake(item, 1);
    } else {
        if (m_Equipped[slot] != 0) invGive(m_Equipped[slot], 1);
        m_Equipped[slot] = item;
        invTake(item, 1);
    }
}

void Hero::invRemove(int slot, bool outgoingTwoHanded) {
    if(outgoingTwoHanded) {
        invGive(m_Equipped[4], 1);
        m_Equipped[4] = 0;
        m_Equipped[5] = 0;
    } else {
        if (m_Equipped[slot] != 0) invGive(m_Equipped[slot], 1);
        m_Equipped[slot] = 0;
    }
}

int Hero::getItemInSlot(int slot) {
    return m_Equipped[slot];
}

Effect Hero::DealMeleeDamage() {
    Effect a;
    a.physical  = m_atkPhys + (m_atkPhys * m_bonusAPhys);
    a.magic     = m_atkMagi + (m_atkMagi * m_bonusAMagi);
    a.heat      = m_atkHeat + (m_atkHeat * m_bonusAHeat);
    a.cold      = m_atkCold + (m_atkCold * m_bonusACold);
    a.electric  = m_atkElec + (m_atkElec * m_bonusAElec);
    a.acid      = m_atkAcid + (m_atkAcid * m_bonusAAcid);
    return a;
}

Effect Hero::DealMagicDamage(Effect spell) {
    Effect temp;
    temp.physical  = (m_atkPhys + (m_atkPhys * m_bonusAPhys)) * spell.physical;
    temp.magic     = (m_atkMagi + (m_atkMagi * m_bonusAMagi)) * spell.magic;
    temp.heat      = (m_atkHeat + (m_atkHeat * m_bonusAHeat)) * spell.heat;
    temp.cold      = (m_atkCold + (m_atkCold * m_bonusACold)) * spell.cold;
    temp.electric  = (m_atkElec + (m_atkElec * m_bonusAElec)) * spell.electric;
    temp.acid      = (m_atkAcid + (m_atkAcid * m_bonusAAcid)) * spell.acid;
    return temp;
}

Effect Hero::DealHeal() {
    Effect a;
    a.physical  = m_atkPhys + (m_atkPhys * m_bonusAPhys);
    a.magic     = m_atkMagi + (m_atkMagi * m_bonusAMagi);
    a.heat      = m_atkHeat + (m_atkHeat * m_bonusAHeat);
    a.cold      = m_atkCold + (m_atkCold * m_bonusACold);
    a.electric  = m_atkElec + (m_atkElec * m_bonusAElec);
    a.acid      = m_atkAcid + (m_atkAcid * m_bonusAAcid);
    return a;
}

bool Hero::UpdateLevel() {
    if (m_exp > m_xpNeeded[m_level]) {
        m_level++;
        setMaxHP(m_str * (m_level + 1));
        setMaxMP(m_int * (m_level + 1));

        switch (m_class) {
        case CLASS_WARRIOR:

            m_atkPhys   = m_str * m_level * WARRIOR_PHYSICAL_ATTACK_BONUS;
            m_atkMagi   = m_int * m_level * WARRIOR_MAGICAL_ATTACK_BONUS;
            if(m_level == 1) getSpells().push_back(Special::SPELL_HEROISM);
            else if(m_level == 1) getSpells().push_back(Special::SPELL_BRACE);
            else if(m_level == 1) getSpells().push_back(Special::SPELL_FOCUS);
            else if(m_level == 1) getSpells().push_back(Special::SPELL_CHALLENGE);
            else if(m_level == 1) getSpells().push_back(Special::SPELL_BASH);
            break;
        case CLASS_WIZARD:
            m_atkPhys   = m_str * m_level * WIZARD_PHYSICAL_ATTACK_BONUS;
            m_atkMagi   = m_int * m_level * WIZARD_MAGICAL_ATTACK_BONUS;
            if(m_level == 1) getSpells().push_back(Special::SPELL_MISSILE);
            else if(m_level == 2) getSpells().push_back(Special::SPELL_BURN);
            else if(m_level == 3) getSpells().push_back(Special::SPELL_LIGHTNING_BOLT);
            else if(m_level == 5) getSpells().push_back(Special::SPELL_GUSH_WATER);
            else if(m_level == 7) getSpells().push_back(Special::SPELL_PETRIFY);
            else if(m_level == 8) getSpells().push_back(Special::SPELL_FIREBALL);
            else if(m_level == 9) getSpells().push_back(Special::SPELL_SLOW);
            else if(m_level == 10) getSpells().push_back(Special::SPELL_FLOOD);
            else if(m_level == 11) getSpells().push_back(Special::SPELL_HASTE);
            else if(m_level == 13) getSpells().push_back(Special::SPELL_EARTHQUAKE);
            else if(m_level == 14) getSpells().push_back(Special::SPELL_FIRE_STORM);
            else if(m_level == 15) getSpells().push_back(Special::SPELL_LIGHTNING_STORM);
            else if(m_level == 16) getSpells().push_back(Special::SPELL_SNOW_STORM);
            else if(m_level == 17) getSpells().push_back(Special::SPELL_VOLCANO);
            else if(m_level == 18) getSpells().push_back(Special::SPELL_ASTEROID);
            break;

        case CLASS_ROGUE:
            m_atkPhys   = m_str * m_level * ROGUE_PHYSICAL_ATTACK_BONUS;
            m_atkMagi   = m_int * m_level * ROGUE_MAGICAL_ATTACK_BONUS;
            if(m_level == 1) m_Spells.push_back(Special::SPELL_EXAMINE);
            else if(m_level == 4) m_Spells.push_back(Special::SPELL_SNEAK);
            else if(m_level == 8) m_Spells.push_back(Special::SPELL_EXPOSE_WEAKNESS);
            else if(m_level == 12) m_Spells.push_back(Special::SPELL_PEEK);
            else if(m_level == 15) m_Spells.push_back(Special::SPELL_SNARE);
            else if(m_level == 18) m_Spells.push_back(Special::SPELL_STEAL);

            break;

        case CLASS_CLERIC:
            m_atkPhys   = m_str * m_level * CLERIC_PHYSICAL_ATTACK_BONUS;
            m_atkMagi   = m_int * m_level * CLERIC_MAGICAL_ATTACK_BONUS;
            if(m_level == 1) m_Spells.push_back(Special::SPELL_HEAL);
            else if(m_level == 2) m_Spells.push_back(Special::SPELL_BLESS);
            else if(m_level == 3) m_Spells.push_back(Special::SPELL_CURSE);
            else if(m_level == 4) m_Spells.push_back(Special::SPELL_SICK);
            else if(m_level == 5) m_Spells.push_back(Special::SPELL_BLIND);
            else if(m_level == 6) m_Spells.push_back(Special::SPELL_CURE);
            else if(m_level == 7) m_Spells.push_back(Special::SPELL_RESTORE);
            else if(m_level == 8) m_Spells.push_back(Special::SPELL_REMOVECURSE);
            else if(m_level == 9) m_Spells.push_back(Special::SPELL_REGEN);
            else if(m_level == 10) m_Spells.push_back(Special::SPELL_POISON);
            else if(m_level == 11) m_Spells.push_back(Special::SPELL_RESURRECT);
            else if(m_level == 12) m_Spells.push_back(Special::SPELL_BLESS_ALL);
            else if(m_level == 14) m_Spells.push_back(Special::SPELL_HEAL_ALL);
            else if(m_level == 16) m_Spells.push_back(Special::SPELL_REGEN_ALL);
            else if(m_level == 18) m_Spells.push_back(Special::SPELL_RES_ALL);
            else if(m_level == 20) m_Spells.push_back(Special::SPELL_RESTORE_ALL);
            break;
        }
        return true;
    }
    return false;
}
