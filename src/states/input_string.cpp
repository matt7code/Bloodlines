#include "states/input_string.h"

Input_String Input_String::m_Input_String;

void Input_String::Init(Engine* e) {
    m_text.clear();
    ProcessInputString(e->getInputString());
    m_Boxes.getBoxes().clear();
    m_Boxes.addBox("Label", sf::Vector2f(1820, 32), sf::Vector2f(50, 50));
    m_Boxes.addBox("Input", sf::Vector2f(1820, 141), sf::Vector2f(50, 121));
    m_Boxes.addBox("Help", sf::Vector2f(1820, 730), sf::Vector2f(50, 300));
    m_Boxes.getBox("Label").addText(m_label, sf::Vector2f(10, -5), 36);
    m_Boxes.getBox("Help").addText(m_help, sf::Vector2f(10, -5), 36);
}

void Input_String::HandleEvents(Engine* e) {
    // Process events
    sf::Event event;
    while (e->m_RenderWindow.pollEvent(event)) {
        // Close window : exit
        if (event.type == sf::Event::Closed) {
            e->Quit();
        } else if (event.type == sf::Event::TextEntered) {
            if(event.text.unicode == 27) { /// Escape
                e->setInputString("CaNcElEd");
                e->PopState();
            } else if(event.text.unicode == 13) { /// Enter
                e->setInputString(m_text);
                e->PopState();
            } else if(event.text.unicode == 8) { /// Backspace
                if (!m_text.empty()) m_text.pop_back();
            } else { /// Add a character to the input
                m_text += static_cast<char>(event.text.unicode);
            }
        }
    }
}

void Input_String::Update(Engine* e) {
    m_Boxes.getBox("Input").getText().clear();
    std::string blink_text = m_text;

    if ((e->getTimeRunning().getElapsedTime().asMilliseconds() % 1000) > 500) {
        blink_text += '_';
    }
    m_Boxes.getBox("Input").addText(Tool::FormatLine(blink_text, 86), sf::Vector2f(10, -5), 32);
}

void Input_String::Draw(Engine* e) {
    m_Boxes.DrawBoxes(e->m_RenderWindow);
}

void Input_String::ProcessInputString(std::string incoming) {
    std::size_t pos;
    pos = incoming.find(":");
    m_label = incoming.substr(0, pos);
    m_help = incoming.substr(pos + 1);
}
