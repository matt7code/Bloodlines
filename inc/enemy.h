#ifndef ENEMY_H
#define ENEMY_H

#include <string>
#include <vector>
#include <cmath>
#include <SFML/Graphics.hpp>
#include "status.h"

class Enemy {
public:
    Enemy() {};
    //Enemy(int id, std::string name, std::string tag, int avatar, int level, float bHp, float bAttack, float bDefence, float bXp, float bGold, std::initializer_list<int> abilitiesOffensive = {}, std::initializer_list<int> abilitiesDefensive = {});
    virtual ~Enemy() {};

//    void                setID(unsigned int val) { m_id = val; }
//    void                setName(std::string val) { m_name = val; }
//    void                setTag(std::string val) { m_tag = val; }
//    void                setAvatar(int val) { m_avatar = val; }
//    void                setLevel(int val) { m_level = val; }
//    void                setBonusAttack(float val) { m_bonusAttack = val; }
//    void                setBonusDefence(float val) { m_bonusDefence = val; }
//    void                setBonusXp(float val) { m_bonusXp = val; }
//    void                setBonusGold(float val) { m_bonusGold = val; }
//    void                setBonusHp(float val) { m_bonusHp = val; }
//    void                setColor(sf::Color val) { m_color = val; }
//    void                setScale(sf::Vector2f val) { m_scale = val; }
//
//    int                 getID() { return m_id; }
//    std::string         getName() { return m_name; }
//    std::string         getTag() { return m_tag; }
//    int                 getAvatar() { return m_avatar; }
//    int                 getLevel() { return m_level; }
//    float               getMaxHp() { return m_maxHp; }
//    float               getBonusAttack() { return m_bonusAttack; }
//    float               getBonusDefence() { return m_bonusDefence; }
//    float               getBonusXp() { return m_bonusXp; }
//    float               getBonusGold() { return m_bonusGold; }
//    float               getBonusHp() { return m_bonusHp; }
//    std::vector<int>&   getAbilitiesOffensive() { return m_abilitiesOffensive; }
//    std::vector<int>&   getAbilitiesDefensive() { return m_abilitiesDefensive; }
//    sf::Color           getColor() { return m_color; }
//    sf::Vector2f        getScale() { return m_scale; }
//    float               GetXPAward();

//    void                TakeDamage(float amount = 0);
//    void                TakeHeal(float amount = 0);
//    bool                IsDead() { return m_hp == 0; }
//    void                GainStatus(int type = 0, int duration = 1, float strength = 1.0f);
//    float               GetMeleeAttackMin() { return 2 * m_level * m_bonusAttack;   }
//    float               GetMeleeAttackMax() { return m_level * m_bonusAttack; }
//    float               GetDefense() { return m_level * m_bonusDefence; }

//    std::vector<Status>& GetStatuses() { return m_statuses; }

    int                 m_id            = 0;
    std::string         m_name          = "Unknown";
    std::string         m_tag           = "UNKNOWN";
    int                 m_avatar        = 0;
    int                 m_level         = 1;
    float               m_bonusHp       = 1.0f;
    float               m_bonusXp       = 1.0f;
    float               m_bonusGold     = 1.0f;
//    float               m_hp            = 1.0f;
//    float               m_maxHp         = 1.0f;

    std::vector<int>    m_abilitiesOffensive;
    std::vector<int>    m_abilitiesDefensive;
    sf::Color           m_color         = sf::Color::White;
    sf::Vector2f        m_scale         = sf::Vector2f(1.0f, 1.0f);

    const float         LOG_LEVEL_BASE  = 2.5f; // <-- The LOG_LEVEL_BASE will determine the rate at which monsters increase in health and xp reward

    /// Bonuses:
    float m_bonusAPhys;
    float m_bonusAMagi;
    float m_bonusAHeat;
    float m_bonusACold;
    float m_bonusAElec;
    float m_bonusAAcid;

    float m_bonusDPhys;
    float m_bonusDMagi;
    float m_bonusDHeat;
    float m_bonusDCold;
    float m_bonusDElec;
    float m_bonusDAcid;


//    float               m_bonusAttack   = 1.0f;
//    float               m_bonusDefence  = 1.0f;
//    std::vector<Status> m_statuses;
};

#endif // ENEMY_H
