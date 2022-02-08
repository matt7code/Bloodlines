#include "box.h"

void Box::Draw(sf::RenderWindow& m_RenderWindow) {
    for (auto& r : m_rectangles) m_RenderWindow.draw(r);
    for (auto& b : m_BoxParts)   m_RenderWindow.draw(b);
    for (auto& t : m_text)       m_RenderWindow.draw(t);
}

/** \brief Add text to a Box
 *
 * \param std::string   text                "Blank Text"
 * \param sf::Vector2f  pos                 sf::Vector2f(10.0f, -5.0f)
 * \param int           charsize            24
 * \param int           font                Box::FONT_NORMAL
 * \param sf::Color     text_color          sf::Color::White
 * \param int           shadow_thickness    2
 * \param sf::Color     shadow_color        sf::Color::Black
 * \return  void
 *
 */
void Box::addText(std::string text, sf::Vector2f pos, int charsize, int font, sf::Color text_color, int shadow_thickness, sf::Color shadow_color) {
    sf::Text t;
    t.setString(text);
    t.setCharacterSize(charsize);
    switch (font) {
    case FONT_MESSY:
        t.setFont(*mp_FontMessy);
        break;
    case FONT_NORMAL:
        t.setFont(*mp_FontNice);
        break;
    case FONT_OUTLINE:
        t.setFont(*mp_FontOutline);
        break;
    case FONT_ROMAN:
        t.setFont(*mp_FontRoman);
        break;
    case FONT_SUPERTHIN:
        t.setFont(*mp_FontSuperThin);
        break;
    case FONT_ZOMBIE:
        t.setFont(*mp_FontZombie);
        break;
    case FONT_NINJA:
        t.setFont(*mp_FontNinja);
        break;
    case FONT_WONDER:
        t.setFont(*mp_FontWonder);
        break;
    default:
        break;
    }

    if(shadow_thickness != 0) {
        t.setPosition(sf::Vector2f(m_pos.x + pos.x + shadow_thickness, m_pos.y + pos.y + shadow_thickness));
        t.setColor(shadow_color);
        m_text.push_back(t);
    }

    t.setPosition(sf::Vector2f(m_pos.x + pos.x, m_pos.y + pos.y));
    t.setColor(text_color);
    m_text.push_back(t);
}

//void Box::addText(std::string text, sf::Vector2f pos, int charsize, int font, sf::Color color)
//{
//    sf::Text t;
//    t.setString(text);
//    t.setPosition(sf::Vector2f(m_pos.x + pos.x, m_pos.y + pos.y));
//    t.setCharacterSize(charsize);
//    t.setColor(color);
//    switch (font)
//    {
//    case FONT_MESSY:
//        t.setFont(*mp_FontMessy);
//        break;
//    case FONT_NORMAL:
//        t.setFont(*mp_FontNice);
//        break;
//    case FONT_OUTLINE:
//        t.setFont(*mp_FontOutline);
//        break;
//    case FONT_ROMAN:
//        t.setFont(*mp_FontRoman);
//        break;
//    case FONT_SUPERTHIN:
//        t.setFont(*mp_FontSuperThin);
//        break;
//    default:
//        break;
//    }
//    m_text.push_back(t);
//}

void Box::setPos(sf::Vector2f val) {
    m_pos = val;

    for (auto& r : m_rectangles) {
        sf::Vector2f localPos = r.getPosition();
        r.setPosition(val.x + localPos.x, val.y + localPos.y);
    }
    for (auto& b : m_BoxParts) {
        sf::Vector2f localPos = b.getPosition();
        b.setPosition(val.x + localPos.x, val.y + localPos.y);
    }
    for (auto& t : m_text) {
        sf::Vector2f localPos = t.getPosition();
        t.setPosition(val.x + localPos.x, val.y + localPos.y);
    }
}

void Box::setSize(sf::Vector2f val) {
    m_size = val;
    for (auto& r : m_rectangles) {
        r.setSize(m_size);
    }
}

void Box::setStyle(int style) {
    if (style == BOX_PLAIN) {
        m_BoxParts.clear();
        return;
    }
    if (style == BOX_NONE) {
        m_BoxParts.clear();
        m_rectangles.clear();
        return;
    }

    sf::Sprite sprite;
    sprite.setTexture(*mp_Texture);
    sprite.setOrigin(16, 16);

    int offset = 0;
    if (style == BOX_CONVO) offset = 96;
    if (style == BOX_INFO) offset = 192;

    sprite.setTextureRect(sf::IntRect(0, 0 + offset, 32, 32));
    sprite.setPosition(sf::Vector2f(m_pos.x, m_pos.y));
    m_BoxParts.push_back(sprite);

    sprite.setTextureRect(sf::IntRect(64, 0 + offset, 32, 32));
    sprite.setPosition(sf::Vector2f(m_pos.x + m_size.x, m_pos.y));
    m_BoxParts.push_back(sprite);

    sprite.setTextureRect(sf::IntRect(0, 64 + offset, 32, 32));
    sprite.setPosition(sf::Vector2f(m_pos.x, m_pos.y + m_size.y));
    m_BoxParts.push_back(sprite);

    sprite.setTextureRect(sf::IntRect(64, 64 + offset, 32, 32));
    sprite.setPosition(sf::Vector2f(m_pos.x + m_size.x, m_pos.y + m_size.y));
    m_BoxParts.push_back(sprite);

    // Fill in the top edge.
    sprite.setTextureRect(sf::IntRect(32, 0 + offset, 32, 32));
    for(int i = 1; i <= ((m_size.x / 32) - 1); i++) {
        sprite.setPosition(sf::Vector2f(m_pos.x + (i * 32), m_pos.y));
        m_BoxParts.push_back(sprite);
    }
    sprite.setPosition(sf::Vector2f(m_pos.x + m_size.x - 32, m_pos.y));
    m_BoxParts.push_back(sprite);

    // Fill in the bottom edge.
    sprite.setTextureRect(sf::IntRect(32, 64 + offset, 32, 32));
    for(int i = 1; i <= ((m_size.x / 32) - 1); i++) {
        sprite.setPosition(sf::Vector2f(m_pos.x + (i * 32), m_pos.y + m_size.y));
        m_BoxParts.push_back(sprite);
    }
    sprite.setPosition(sf::Vector2f(m_pos.x + m_size.x - 32, m_pos.y + m_size.y));
    m_BoxParts.push_back(sprite);

    // Fill in the left edge.
    sprite.setTextureRect(sf::IntRect(0, 32 + offset, 32, 32));
    for(int i = 1; i < m_size.y / 32 - 1; i++) {
        sprite.setPosition(sf::Vector2f(m_pos.x, m_pos.y + (i * 32)));
        m_BoxParts.push_back(sprite);
    }
    if(m_size.y > 32) {
        sprite.setPosition(sf::Vector2f(m_pos.x, m_pos.y + m_size.y - 32));
        m_BoxParts.push_back(sprite);
    }

    // Fill in the right edge.
    sprite.setTextureRect(sf::IntRect(64, 32 + offset, 32, 32));
    for(int i = 1; i < m_size.y / 32 - 1; i++) {
        sprite.setPosition(sf::Vector2f(m_pos.x + m_size.x, m_pos.y + (i * 32)));
        m_BoxParts.push_back(sprite);
    }
    if(m_size.y > 32) {
        sprite.setPosition(sf::Vector2f(m_pos.x + m_size.x, m_pos.y + m_size.y - 32));
        m_BoxParts.push_back(sprite);
    }
}
