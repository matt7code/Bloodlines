#ifndef MONSTER_H
#define MONSTER_H

#include <SFML/Graphics.hpp>
#include <string>

class Monster
{
    public:
        enum  {DAMAGE_NONE, DAMAGE_STATUS, DAMAGE_MAGIC, DAMAGE_RANGE, DAMAGE_MELEE, DAMAGE_HEAT, DAMAGE_COLD, DAMAGE_ELEC, DAMAGE_ACID};
        enum  {STATUS_NONE, STATUS_BLIND, STATUS_SLEEP, STATUS_POISON, STATUS_CONFUSION, STATUS_POSSESS, STATUS_SICK, STATUS_PETRIFY, STATUS_ROT, STATUS_DEATH};

        virtual ~Monster() {}
        Monster(int type, int level = 1, int hit_dice = 8, int atk = 1, int def = 1, std::string name = "");

        void            takeDamage(int amount = 0, int type = DAMAGE_MELEE, int bonus = 0);
        void            takeHeal(int amount = 0, int type = 0, int bonus = 0);
        void            gainStatus(int type = 0, int duration = 0, int bonus = 0);
        bool            isDead() { return m_hp == 0; }
        int             getMeleeAttackMin() { return m_atk / 2; }
        int             getMeleeAttackMax() { return m_atk; }


        std::string     getName()   { return m_name; }
        int             getType()   { return m_type; }
        int             getHp()     { return m_hp; }
        int             getAtk()    { return m_atk; }
        int             getDef()    { return m_def; }
        sf::Sprite&     getSprite() { return m_sprite; }

        void            setName(std::string val) { m_name = val; }
        void            setType(int val) { m_type = val; }
        void            setHp(int val) { m_hp = val; }
        void            setAtk(int val) { m_atk = val; }
        void            setDef(int val) { m_def = val; }
        void            setSprite(sf::Sprite val) { m_sprite = val; }

    private:
        int             m_level = 1;
        int             m_hp;
        int             m_maxHP;
        int             m_atk;
        int             m_def;
        int             m_type;
        std::string     m_name;
        sf::Sprite      m_sprite;
};

#endif // MONSTER_H
