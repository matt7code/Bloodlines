#include "monster.h"

Monster::Monster(int type, int level, int hit_dice, int atk, int def, std::string name)
{
    if(name == "")
    {
        switch(type)
        {
        case 0:
            name = "Dinobite";
            break;
        case 1:
            name = "Chimera";
            break;
        case 2:
            name = "Hopper";
            break;
        case 3:
            name = "Armorok";
            break;
        case 4:
            name = "Nightmare";
            break;
        case 5:
            name = "Lionark";
            break;
        case 6:
            name = "Skizzler";
            break;
        case 7:
            name = "Cluck";
            break;
        case 8:
            name = "Demog";
            break;
        case 9:
            name = "Smashtail";
            break;
        case 10:
            name = "Crabby";
            break;
        case 11:
            name = "Tentagrel";
            break;
        default:
            name = "Unknown";
            break;

        }
    }
    m_name = name;
    m_type = type;
    m_level = level;
    m_hp = m_maxHP = hit_dice * m_level;
    m_atk = atk * hit_dice;
    m_def = def * hit_dice;
}

void Monster::takeDamage(int amount, int type, int bonus)
{
    m_hp -= amount;
    if(m_hp < 0) m_hp = 0;
}

void Monster::takeHeal(int amount, int type, int bonus)
{
    m_hp += amount;
    if(m_hp > m_maxHP) m_hp = m_maxHP;
}
