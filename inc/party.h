#ifndef PARTY_H
#define PARTY_H

#include <SFML/Graphics.hpp>
#include <iostream>
#include <deque>
#include <string>
#include <sstream>
#include "hero.h"
#include "quest.h"
#include "constants.h"
#include "map_level.h"
#include "tool.h"

class Party {
public:

    Party() {};
    virtual ~Party() {};

    void                                Update(float delta);
    void                                Draw(sf::RenderWindow& window);
    int                                 getSize() const {return party.size();}
    void                                AddMember(int avatar, int level, int my_class, std::string name);
    void                                RemoveMember();
    sf::Vector2f                        getPos(int mem_num = 0);
    bool                                validPartySlot(int mem_num);
    std::vector<Hero>&                  getMembers() {return party;}
    void                                MovePartyTo(sf::Vector2f pos);
    sf::Vector2f                        getLastStepLocation() const { return LastStepLocation; }
    void                                setLastStepLocation(sf::Vector2f pos) {LastStepLocation = pos; }
    Hero&                               getMember(int slot) {return party[slot];}
    sf::Texture&                        getTexture() {return texture;}
    int                                 getPartySteps() {return m_steps;}
    int                                 getPartyGold() {return m_gold;}
    void                                setPartySteps(int val) { m_steps = val;}
    void                                setPartyGold(int val) { m_gold = val; }
    void                                modPartyGold(int val);
    std::string                         getSavedValue(const std::string& key);
    void                                setSavedValue(const std::string& key, const std::string& val);
    bool                                areSavedValuesEmpty() { return m_saved_values.empty(); }
    void                                debugSavedValues();
    void                                deleteQuestValue(const std::string& key);
    std::map<std::string, std::string>& getSavedValues() { return m_saved_values; }
    float                               getCurrentPartySpeed() { return m_curSpeed; }
    void                                setCurrentPartySpeed(float val) { m_curSpeed = val; }
    void                                recordMove(sf::Vector2f mov);
    std::deque<sf::Vector2f>&           getMoveRecord() { return m_moveRecord; }
    std::vector<int>&                   getChests() { return m_chests; }
    std::vector<Quest>&                 getQuests() { return m_quests; }
    bool                                safe = false;

private:
    sf::Texture                         texture;
    int                                 m_steps = 0;
    int                                 m_gold  = 0;
    int                                 m_Facing = DIR_SOUTH;
    float                               m_curSpeed = BASE_WALKING_SPEED;
    sf::Vector2f                        LastStepLocation;
    std::deque<sf::Vector2f>            m_moveRecord;
    std::vector<Hero>                   party;
    std::map<std::string, std::string>  m_saved_values;
    std::vector<int>                    m_chests;
    std::vector<Quest>                  m_quests;
};

#endif // PARTY_H
