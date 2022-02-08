#ifndef EditEnemy_H
#define EditEnemy_H

#include <iostream>
#include <SFML/Graphics.hpp>
#include "gamestate.h"
#include "boxes.h"
#include "tool.h"

/*
    Reminders:
    Change the header guard.
    Are you Pushing a State, or Changing a state?
*/

class EditEnemy : public GameState {
public:
    void Init(Engine* e);
    void Cleanup(Engine* e);
    void Pause(Engine* e);
    void Resume(Engine* e);
    void HandleEvents(Engine* e);
    void Update(Engine* e);
    void Draw(Engine* e);

    static EditEnemy* Instance() {
        return &m_EditEnemy;
    }

protected:

    EditEnemy() { }

private:
    Boxes               m_Boxes;
    sf::View            m_View;
    static EditEnemy    m_EditEnemy;
    //sf::Texture         m_EditEnemyTexture;
    //sf::Sprite          m_Sprite;
};

#endif
