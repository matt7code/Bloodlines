#ifndef INTROSTATE_H
#define INTROSTATE_H

#include <SFML/Graphics.hpp>
#include "gamestate.h"

class CIntroState : public GameState {
public:
    void Init(Engine* e);
    void Cleanup(Engine* e) {};
    void Pause(Engine* e) {};
    void Resume(Engine* e) {};
    void HandleEvents(Engine* e);
    void Update(Engine* e) {};
    void Draw(Engine* e);

    static CIntroState* Instance() {
        return &m_IntroState;
    }

protected:
    CIntroState() {}

private:
    static CIntroState  m_IntroState;
    sf::Texture         m_introTexture;
    sf::Sprite          m_introScreen;
};

#endif

