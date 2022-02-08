#ifndef BOX_H
#define BOX_H

#include <SFML/Graphics.hpp>
#include <SFML/Audio.hpp>
#include <string>
#include <iostream>

//#include "tool.h"

class Box {
public:
    enum                        {FONT_SUPERTHIN, FONT_MESSY, FONT_NORMAL, FONT_ROMAN, FONT_OUTLINE, FONT_ZOMBIE, FONT_NINJA, FONT_WONDER};
    enum                        {BOX_NONE, BOX_PLAIN, BOX_ROUNDED, BOX_CONVO, BOX_INFO};
    enum                        {ARROW_LEFT, ARROW_UP, ARROW_RIGHT, ARROW_DOWN};

    Box() :
        mp_FontSuperThin(nullptr),
        mp_FontMessy(nullptr),
        mp_FontNice(nullptr),
        mp_FontRoman(nullptr),
        mp_FontOutline(nullptr),

        mp_Texture(nullptr),

        mp_SoundConfirm(nullptr),
        mp_SoundCancel(nullptr),
        mp_SoundSelect(nullptr) {
    }
    virtual ~Box() {}

    void                        setStyle(int style);
    void                        HandleEvents();
    void                        Update(float delta);
    void                        Draw(sf::RenderWindow& m_RenderWindow);

    void                        Load();
    void                        Save();

    sf::Vector2f                getPos() {
        return m_pos;
    }
    void                        setPos(sf::Vector2f val);
    sf::Vector2f                getSize() {
        return m_size;
    }
    void                        setSize(sf::Vector2f val);
    sf::Texture*                getTexture() {
        return mp_Texture;
    }
    void                        setTexture(sf::Texture* val) {
        mp_Texture = val;
    }
    sf::Font*                   getFontDialog() {
        return mp_FontRoman;
    }
    void                        setFontDialog(sf::Font* val) {
        mp_FontRoman = val;
    }
    sf::Font*                   getFontOutline() {
        return mp_FontOutline;
    }
    void                        setFontOutline(sf::Font* val) {
        mp_FontOutline = val;
    }
    sf::Font*                   getFontNice() {
        return mp_FontNice;
    }
    void                        setFontNice(sf::Font* val) {
        mp_FontNice = val;
    }
    sf::Font*                   getFontSuperThin() {
        return mp_FontSuperThin;
    }
    void                        setFontSuperThin(sf::Font* val) {
        mp_FontSuperThin = val;
    }
    sf::Font*                   getFontMessy() { return mp_FontMessy; }
    void                        setFontMessy(sf::Font* val) { mp_FontMessy = val; }
    sf::Font*                   getFontZombie() { return mp_FontZombie; }
    void                        setFontZombie(sf::Font* val) { mp_FontZombie = val; }
    sf::Font*                   getFontNinja() { return mp_FontNinja; }
    void                        setFontNinja(sf::Font* val) { mp_FontNinja = val; }
    sf::Font*                   getFontWonder() { return mp_FontWonder; }
    void                        setFontWonder(sf::Font* val) { mp_FontWonder = val; }



    sf::Sound*                  getSoundConfirm() {
        return mp_SoundConfirm;
    }
    void                        setSoundConfirm(sf::Sound* val) {
        mp_SoundConfirm = val;
    }
    sf::Sound*                  getSoundCancel() {
        return mp_SoundCancel;
    }
    void                        setSoundCancel(sf::Sound* val) {
        mp_SoundCancel = val;
    }
    sf::Sound*                  getSoundSelect() {
        return mp_SoundSelect;
    }
    void                        setSoundSelect(sf::Sound* val) {
        mp_SoundSelect = val;
    }

    //void                        addText(std::string text, sf::Vector2f pos, int charsize, int font, sf::Color color = sf::Color::White);
    void                        addText(std::string text = "Blank Text", sf::Vector2f pos = sf::Vector2f(10.0f, -0.5f), int charsize = 24, int font = Box::FONT_NORMAL, sf::Color text_color = sf::Color::White, int shadow_thickness = 2, sf::Color shadow_color = sf::Color::Black);

    std::string                 getName() {
        return m_name;
    }
    void                        setName(std::string val) {
        m_name = val;
    }
    int                         getID() {
        return m_ID;
    }
    void                        setID(int val) {
        m_ID = val;
    }

    std::vector<sf::RectangleShape>&    getRectangles() {
        return m_rectangles;
    }
    std::vector<sf::Text>&              getText() {
        return m_text;
    }
    std::vector<sf::Sprite>&            getBoxParts() {
        return m_BoxParts;
    }
    void                                setColor(sf::Color col) {
        for (auto& r : m_rectangles) r.setFillColor(col);
    }
    void clear() { m_text.clear(); }

protected:

private:
    int                         m_ID;
    std::string                 m_name;
    sf::Vector2f                m_pos;              //!< Member variable "pos"
    sf::Vector2f                m_size;             //!< Member variable "size"
    sf::Font*                   mp_FontSuperThin;   //!< Member variable "font_dialog"
    sf::Font*                   mp_FontMessy;       //!< Member variable "font_stats"
    sf::Font*                   mp_FontNice;        //!< Member variable "font_dialog"
    sf::Font*                   mp_FontRoman;       //!< Member variable "font_stats"
    sf::Font*                   mp_FontOutline;     //!< Member variable "font_dialog"
    sf::Font*                   mp_FontZombie;     //!< Member variable "font_dialog"
    sf::Font*                   mp_FontNinja;     //!< Member variable "font_dialog"
    sf::Font*                   mp_FontWonder;     //!< Member variable "font_dialog"
    sf::Texture*                mp_Texture;         //!< Member variable "texture"
    sf::Sound*                  mp_SoundConfirm;    //!< Confirming a selection
    sf::Sound*                  mp_SoundCancel;     //!< Canceling a menu
    sf::Sound*                  mp_SoundSelect;     //!< Moving around a menu

    std::vector<sf::RectangleShape> m_rectangles;   //!< Member variable "rectangles"
    std::vector<sf::Text>       m_text;             //!< Member variable "text"
    std::vector<sf::Sprite>     m_BoxParts;         //!< Member variable "sprites"

    int                         m_style = BOX_PLAIN;//!< Style of box: BOX_PLAIN
};

#endif // BOX_H
