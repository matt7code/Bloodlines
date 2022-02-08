#ifndef Hero_H
#define Hero_H

#include <iostream>
#include <SFML/Graphics.hpp>
#include "constants.h"
#include "math.h"
#include "special.h"
#include "status.h"
#include "effect.h"
#include "item.h"

const float BLIND_EFFECT    = -5.0f;
const float CURSE_EFFECT    = -1.0f;
const float BLESS_EFFECT    = +1.0f;
const float HEROISM_EFFECT  = +0.20f;
const float FLEE_EFFECT     = +0.50f;
const float BRACE_EFFECT    = +0.25f;
const float GUARD_EFFECT    = -0.50f; // "-50% damage"
const float FOCUS_EFFECT    = +0.20f;
const float SNEAK_EFFECT    = +0.25f;
const float PETRIFY_EFFECT  = +1.00f; // "+100% resistence"

const float WARRIOR_PHYSICAL_ATTACK_BONUS  = 2.0;
const float WARRIOR_MAGICAL_ATTACK_BONUS   = 0.5;
const float WIZARD_PHYSICAL_ATTACK_BONUS   = 0.5;
const float WIZARD_MAGICAL_ATTACK_BONUS    = 2.0;
const float ROGUE_PHYSICAL_ATTACK_BONUS    = 1.5;
const float ROGUE_MAGICAL_ATTACK_BONUS     = 0.5;
const float CLERIC_PHYSICAL_ATTACK_BONUS   = 1.0;
const float CLERIC_MAGICAL_ATTACK_BONUS    = 1.5;

struct CarriedItem {
    int type;
    int amount;
};

class Hero {
public:
    Hero(int level = 1, int myclass = CLASS_NONE);
    virtual ~Hero() {};

    /* Header */
    void                            setName(std::string val) { m_name = val; }
    void                            setAvatar(int type);
    void                            setExp(int val) { m_exp = val; }
    void                            setLevel(int val);
    void                            setClass(int val) { m_class = val; }

    /* Implementation Crap */
    void                            SetFacingByVector(sf::Vector2f mv);
    void                            setFacing(int val) { m_facing = val; }
    void                            setActive(bool val) { m_active = val; }
    void                            setSpecial(std::string val) { m_special = val; }

    int                             getClass() { return m_class; }
    int                             getFacing() { return m_facing; }
    int                             getAvatar() { return m_avatar; }
    sf::Sprite&                     getSprite() { return m_sprite; }
    sf::Vector2i&                   getTextureSheetBase() { return textureSheetBase; }
    std::string                     getName() { return m_name; }
    static std::string              getClassName(int i);
    std::vector<int>&               getSpells() { return m_Spells; }
    std::vector<CarriedItem>&       getInventory() { return m_Inventory; }
    std::string                     getSpecial() { return m_special; }
    float                           getExp() { return m_exp; }
    int                             getLevel() { return m_level; }
    float                           getExpTNL();
    void                            AnimateWalk(float seconds);
    void                            UpdateStatus();
    void                            UpdateAppearance();
    bool                            UpdateLevel();

    /* Combat */
    float                           TakeDamage(Effect incoming);
    float                           TakeHeal(float amount);
    void                            TakeStatus(Status incoming);

    /* Effects */
    Status                          DealStatus();
    Effect                          DealHeal();
    Effect                          DealMagicDamage(Effect spell);
    Effect                          DealMeleeDamage();


    /* Status */
    bool                            isDead() { return (m_curHP == 0.0f); }
    bool                            isAlive() { return (m_curHP > 0.0f); }
    bool                            isActive() { return m_active; }
    bool                            isInactive() { return !m_active; }

    bool                            isHasted() { return m_isHasted; }
    bool                            isSlowed() { return m_isSlowed; }
    bool                            m_DeathProcessed = false;


    /* Basic Stats */
    float                           getStr() { return m_str + (m_str * m_bonusStr); }
    float                           getInt() { return m_int + (m_int * m_bonusInt); }
    float                           getAgi() { return m_agi + (m_agi * m_bonusAgi); }

    float                           getCurHP() { return m_curHP; }
    float                           getCurMP() { return m_curMP; }
    float                           getMaxHP() { return m_maxHP + (m_maxHP * m_bonusMaxHP); }
    float                           getMaxMP() { return m_maxMP + (m_maxMP * m_bonusMaxMP); }

    void                            setCurHP(float val) { m_curHP = val; }
    void                            setCurMP(float val) { m_curMP = val; }
    void                            setMaxHP(float val) { m_maxHP = val; }
    void                            setMaxMP(float val) { m_maxHP = val; }

    // Defensive strength
    float                           getDefPhys() { return m_defPhys + (m_defPhys * m_bonusDPhys); }
    float                           getDefMagi() { return m_defMagi + (m_defMagi * m_bonusDMagi); }
    float                           getDefHeat() { return m_defHeat + (m_defHeat * m_bonusDHeat); }
    float                           getDefCold() { return m_defCold + (m_defCold * m_bonusDCold); }
    float                           getDefElec() { return m_defElec + (m_defElec * m_bonusDElec); }
    float                           getDefAcid() { return m_defAcid + (m_defAcid * m_bonusDAcid); }

    // Attack strength
    float                           getAtkPhys() { return m_atkPhys + (m_atkPhys * m_bonusAPhys); }
    float                           getAtkMagi() { return m_atkMagi + (m_atkMagi * m_bonusAMagi); }
    float                           getAtkHeat() { return m_atkHeat + (m_atkHeat * m_bonusAHeat); }
    float                           getAtkCold() { return m_atkCold + (m_atkCold * m_bonusACold); }
    float                           getAtkElec() { return m_atkElec + (m_atkElec * m_bonusAElec); }
    float                           getAtkAcid() { return m_atkAcid + (m_atkAcid * m_bonusAAcid); }

    // Combat variables
    int                             lastCommand = 0;
    int                             lastSubCommand = 0;

    /* Inventory */
    int                             getInventoryAmount(int item);
    int                             getItemInSlot(int slot);
    void                            setInventoryAmount(int item, int amount);
    void                            invTake(int item, int amount);
    void                            invGive(int item, int amount);
    void                            invEquip(int item, int slot, bool incomingTwoHanded = false, bool outgoingTwoHanded = false);
    void                            invRemove(int slot, bool outgoingTwoHanded = false);
    std::vector<int>&               getEquipment() { return m_Equipped; }
    std::string                     getTag() { return m_tag; }
    void                            setTag(std::string val) { m_tag = val; }

    /// For Monsters Only, because
    /// <Combatant> doesn't exist yet
    /// These have to exist to create
    /// the HERO used in combat
    void                            setStr(float val) { m_str = val; }
    void                            setInt(float val) { m_int = val; }
    void                            setAgi(float val) { m_agi = val; }

    void                            setm_bonusAPhys(float val) { m_bonusAPhys = val; }
    void                            setm_bonusAMagi(float val) { m_bonusAMagi = val; }
    void                            setm_bonusAHeat(float val) { m_bonusAHeat = val; }
    void                            setm_bonusACold(float val) { m_bonusACold = val; }
    void                            setm_bonusAElec(float val) { m_bonusAElec = val; }
    void                            setm_bonusAAcid(float val) { m_bonusAAcid = val; }

    void                            setm_bonusDPhys(float val) { m_bonusDPhys = val; }
    void                            setm_bonusDMagi(float val) { m_bonusDMagi = val; }
    void                            setm_bonusDHeat(float val) { m_bonusDHeat = val; }
    void                            setm_bonusDCold(float val) { m_bonusDCold = val; }
    void                            setm_bonusDElec(float val) { m_bonusDElec = val; }
    void                            setm_bonusDAcid(float val) { m_bonusDAcid = val; }

    // Modifiers
    void mod_bonus_str(float val) { m_bonusStr += val; }
    void mod_bonus_int(float val) { m_bonusInt += val; }
    void mod_bonus_agi(float val) { m_bonusAgi += val; }

    void mod_bonus_max_HP(float val) { m_bonusMaxHP += val; }

    void mod_bonus_APhys(float val) { m_bonusAPhys += val; }
    void mod_bonus_AMagi(float val) { m_bonusAMagi += val; }
    void mod_bonus_AHeat(float val) { m_bonusAHeat += val; }
    void mod_bonus_ACold(float val) { m_bonusACold += val; }
    void mod_bonus_AElec(float val) { m_bonusAElec += val; }
    void mod_bonus_AAcid(float val) { m_bonusAAcid += val; }

    void mod_bonus_DPhys(float val) { m_bonusDPhys += val; }
    void mod_bonus_DMagi(float val) { m_bonusDMagi += val; }
    void mod_bonus_DHeat(float val) { m_bonusDHeat += val; }
    void mod_bonus_DCold(float val) { m_bonusDCold += val; }
    void mod_bonus_DElec(float val) { m_bonusDElec += val; }
    void mod_bonus_DAcid(float val) { m_bonusDAcid += val; }

    bool isMonster = false;
private:

    /// Basic Stats:
    float m_curHP       = 0;
    float m_curMP       = 0;
    float m_maxHP       = 0;
    float m_maxMP       = 0;
    float m_str         = 0;
    float m_int         = 0;
    float m_agi         = 0;

    /// Basic Stat Bonuses:
    float m_bonusMaxHP  = 0;
    float m_bonusMaxMP  = 0;
    float m_bonusStr    = 0;
    float m_bonusInt    = 0;
    float m_bonusAgi    = 0;

    /// Combat stats:
    float m_atkPhys     = 0; // m_str * level
    float m_atkMagi     = 0; // m_int * level;
    float m_defPhys     = 0; // (m_agi * level) / 2;
    float m_defMagi     = 0; // (m_agi * level) / 2;

    float m_atkHeat     = 0;
    float m_atkCold     = 0;
    float m_atkElec     = 0;
    float m_atkAcid     = 0;

    float m_defHeat     = 0;
    float m_defCold     = 0;
    float m_defElec     = 0;
    float m_defAcid     = 0;

    /// Bonuses:
    float m_bonusAPhys  = 0;
    float m_bonusAMagi  = 0;
    float m_bonusAHeat  = 0;
    float m_bonusACold  = 0;
    float m_bonusAElec  = 0;
    float m_bonusAAcid  = 0;

    float m_bonusDPhys  = 0;
    float m_bonusDMagi  = 0;
    float m_bonusDHeat  = 0;
    float m_bonusDCold  = 0;
    float m_bonusDElec  = 0;
    float m_bonusDAcid  = 0;

    std::vector<int>    m_Spells;
    std::vector<CarriedItem> m_Inventory;
    std::vector<int>    m_Equipped = {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0}; // 0 slot is for NONE_SLOT
    std::vector<Status> m_Statuses;
    sf::Vector2i        textureSheetBase;
    sf::Sprite          m_sprite;
    std::vector<int>    m_xpNeeded;

    bool isSleeping     = false;
    bool isPetrified    = false;
    bool isPossessed    = false;
    bool isConfused     = false;
    bool m_isHasted       = false;
    bool m_isSlowed       = false;

    std::string m_name  = "Unknown Hero";
    std::string m_tag   = "UnknownTag";
    int  m_class        = CLASS_NONE;
    int  m_level        = 0;
    int  m_avatar       = 0;
    float m_exp         = 0;

    bool m_active       = true;
    std::string m_special = "Error!";
    int  m_facing       = 0;
    int  m_animFrame    = 0;
    int  m_animSet[4]   = {0, 1, 2, 1};
    float m_AnimSecs    = 0;
};

#endif // Hero_H
