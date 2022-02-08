#ifndef EditBeacon_H
#define EditBeacon_H

#include <iostream>
#include <sstream>
#include <map>
#include <SFML/Graphics.hpp>
#include "gamestate.h"
#include "boxes.h"
#include "tool.h"
#include "beacon.h"

class EditBeacon : public GameState {
public:
    void Init(Engine* e);
    void Cleanup(Engine* e);
    void Pause(Engine* e) {};
    void Resume(Engine* e) {};
    void HandleEvents(Engine* e);
    void Update(Engine* e);
    void Draw(Engine* e);

    static EditBeacon* Instance() {
        return &m_EditBeacon;
    }

    sf::Vector2i ConvertClickToScreenPos(sf::Vector2i click, sf::RenderWindow& window);
    int GetChoice(sf::Vector2i click);
    void HandleChoice(Engine* e);
    void UpdateBoxes();
    Beacon* getNearestBeacon(Engine* e);
    void SaveBeacon(Engine* e);

protected:

    EditBeacon() { }

private:
    Boxes               m_Boxes;
    sf::View            m_View;
    static EditBeacon   m_EditBeacon;
    sf::Vector3i        m_clickPos;
    std::map<int, sf::IntRect>
    m_screenMap;
    int                 m_choice = -1;
    int                 currType = -1;

    bool                m_5;
    bool                m_6;
    bool                m_7;
    bool                m_8;
    bool                m_9;
    bool                m_10;
    bool                m_11;

    std::string         m_t5;
    std::string         m_t6;
    std::string         m_t7;
    std::string         m_t8;
    std::string         m_t9;

    std::string         m_t10;
    std::string         m_t11;

    Beacon*             BeaconPtr = nullptr;
};

#endif
