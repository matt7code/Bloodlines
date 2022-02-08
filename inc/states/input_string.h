#ifndef Input_String_H
#define Input_String_H

#include <iostream>
#include <SFML/Graphics.hpp>
#include "gamestate.h"
#include "boxes.h"

class Input_String : public GameState {
public:
    void Init(Engine* e);
    void Cleanup(Engine* e) {};
    void Pause(Engine* e) {};
    void Resume(Engine* e) {};
    void HandleEvents(Engine* e);
    void Update(Engine* e);
    void Draw(Engine* e);

    static Input_String* Instance() {
        return &m_Input_String;
    }

protected:
    Input_String() { }

private:
    Boxes               m_Boxes;
    sf::View            m_View;
    static Input_String m_Input_String;

    void                ProcessInputString(std::string incoming);

    std::string         m_text;
    std::string         m_label;
    std::string         m_help;
};

#endif
