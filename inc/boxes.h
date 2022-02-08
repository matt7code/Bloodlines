#ifndef BOXES_H
#define BOXES_H

#include <iostream>
#include <initializer_list>

#include <SFML/Graphics.hpp>
#include <SFML/Audio.hpp>

#include "box.h"
#include "tool.h"

class Boxes {
public:
    Boxes();
    virtual ~Boxes() {}
    Box&                            addBox(std::string _name = "Unnamed Box",
                                           sf::Vector2f _size = sf::Vector2f(400, 100),
                                           sf::Vector2f _pos = sf::Vector2f(200, 200),
                                           int style = Box::BOX_ROUNDED,
                                           sf::Color _color = sf::Color::Blue);
    Box&                            getBox(std::string _name);
    void                            DrawBoxes(sf::RenderWindow& m_RenderWindow);
    void                            UpdateBoxes(sf::Clock& clock);
    void                            HandleEventsBoxes();
    void                            SaveBoxes();
    void                            LoadBoxes();
    void                            setStyle(int style);
    void                            removeBox(int id);
    void                            removeBox(std::string _name);
    void                            removeBox();
    std::vector<Box>&               getBoxes();
    void                            clear() { m_Boxes.clear(); }
    void                            drawArrow(sf::Vector2f pos, int facing, sf::Vector2f scale, sf::RenderWindow& window, bool center = false, sf::Color color = sf::Color::White);

    bool                            empty() { return m_Boxes.empty(); }
    sf::Texture&                    getTexture() { return m_BoxTexture; }
    void                            PlaySelect(float pitch = 1.0f);
    void                            PlayCancel(float pitch = 1.0f);
    void                            PlayConfirm(float pitch = 1.0f);
    sf::Font&                       getFont(int font);
    int                             Menu(std::initializer_list<std::string> choices, sf::Vector2f pos = sf::Vector2f(100.0f,100.0f));
    bool                            hasBox(const std::string& boxname);
    int                             getNumberOfMenuChoices();

private:
    sf::Texture                     m_BoxTexture;       //!< Member variable "texture"

    int                             m_MenuChoices = -1;
    sf::Font                        m_FontSuperThin;    //!< A super thin font.
    sf::Font                        m_FontMessy;        //!< A messy hurried font.
    sf::Font                        m_FontNice;         //!< A nice, standard font.
    sf::Font                        m_FontRoman;        //!< A roman/german font for dialog.
    sf::Font                        m_FontOutline;      //!< An outlined font, maybe for titles.

    sf::Font                        m_FontZombie;       //!< A zombie font, no digits
    sf::Font                        m_FontNinja;        //!< A Ninja Naruto Font
    sf::Font                        m_FontWonderland;   //!< A wonderland inspired font


    sf::Sound                       m_BoxSoundConfirm;  //!< Confirming a selection
    sf::Sound                       m_BoxSoundCancel;   //!< Canceling a menu
    sf::Sound                       m_BoxSoundSelect;   //!< Moving around a menu

    sf::SoundBuffer                 m_BufferSoundConfirm;
    sf::SoundBuffer                 m_BufferSoundCancel;
    sf::SoundBuffer                 m_BufferSoundSelect;

    std::vector<Box>                m_Boxes;
};

#endif // BOXES_H
