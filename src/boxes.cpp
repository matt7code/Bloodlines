#include "boxes.h"

Boxes::Boxes() {
    m_FontSuperThin.loadFromFile("font/DK Hey Comrade.otf");
    m_FontMessy.loadFromFile    ("font/Write2.ttf");
    m_FontNice.loadFromFile     ("font/Inconsolata.otf");
    m_FontRoman.loadFromFile    ("font/EhmckeFederfraktur.ttf");
    m_FontOutline.loadFromFile  ("font/swissbo.ttf");
    m_FontZombie.loadFromFile   ("font/Zombie_Holocaust.ttf");
    m_FontNinja.loadFromFile    ("font/njnaruto.ttf");
    m_FontWonderland.loadFromFile("font/Beyond Wonderland.ttf");

    m_BoxTexture.loadFromFile("graphics/menu_textures.png");

    m_BufferSoundConfirm.loadFromFile   ("sounds/confirm-button.wav");
    m_BufferSoundSelect.loadFromFile    ("sounds/select-button.wav");
    m_BufferSoundCancel.loadFromFile    ("sounds/cancel-button.wav");

    m_BoxSoundConfirm.setBuffer (m_BufferSoundConfirm);
    m_BoxSoundCancel.setBuffer  (m_BufferSoundCancel);
    m_BoxSoundSelect.setBuffer  (m_BufferSoundSelect);

    m_BoxSoundConfirm.setVolume(5);
    m_BoxSoundCancel.setVolume(5);
    m_BoxSoundSelect.setVolume(5);
}

Box& Boxes::addBox(std::string _name, sf::Vector2f _size, sf::Vector2f _pos, int style, sf::Color _color) {
    Box newBox;

    newBox.setID(m_Boxes.size());
    newBox.setName(_name);
    newBox.setPos(_pos);
    newBox.setSize(_size);

    newBox.setTexture       (&m_BoxTexture);

    newBox.setSoundConfirm  (&m_BoxSoundConfirm);
    newBox.setSoundCancel   (&m_BoxSoundCancel);
    newBox.setSoundSelect   (&m_BoxSoundSelect);

    newBox.setFontDialog    (&m_FontRoman);
    newBox.setFontMessy     (&m_FontMessy);
    newBox.setFontSuperThin (&m_FontSuperThin);
    newBox.setFontNice      (&m_FontNice);
    newBox.setFontOutline   (&m_FontOutline);

    sf::RectangleShape TempRect;
    TempRect.setFillColor(_color);
    TempRect.setSize(newBox.getSize());
    TempRect.setPosition(newBox.getPos());

    newBox.getRectangles().push_back(TempRect);
    newBox.setStyle(style);

    m_Boxes.push_back(newBox);

    return m_Boxes.back();
}

Box& Boxes::getBox(std::string _name) {
    if(_name == "first") return *m_Boxes.begin();
    if(_name == "last") return *(m_Boxes.end() - 1);

    for (auto& b : m_Boxes)
        if (b.getName() == _name) return b;
    std::cerr << "Invalid Box Name" << std::endl;
    return *m_Boxes.begin();
}

/// @brief Removes the last added box.
/// @param None
void Boxes::removeBox() {
    if (!m_Boxes.empty()) m_Boxes.pop_back();
}

/// @brief Removes a box by ID.
/// @param int A Box ID.
void Boxes::removeBox(int id) {
    //std::cout << "Size: " << m_Boxes.size() << std::endl;
    for (auto it = m_Boxes.begin(); it != m_Boxes.end();) {
        //std::cout << " Box Name: " << it->getName() << std::endl;
        if (it->getID() == id) {
            m_Boxes.erase(it);
        } else {
            ++it;
        }
    }
}

/// @brief Removes a box by name.
/// @param _name The name of the box
void Boxes::removeBox(std::string _name) {
    for (auto it = m_Boxes.begin(); it != m_Boxes.end();) {
        if (it->getName() == _name) m_Boxes.erase(it);
        else ++it;
    }
}


std::vector<Box>& Boxes::getBoxes() {
    return m_Boxes;
}

void Boxes::DrawBoxes(sf::RenderWindow& m_RenderWindow) {
    for (auto& box : m_Boxes) {
        box.Draw(m_RenderWindow);
    }
}

void Boxes::UpdateBoxes(sf::Clock& clock) {
}

void Boxes::drawArrow(sf::Vector2f pos, int facing, sf::Vector2f scale, sf::RenderWindow& window, bool center, sf::Color color) {
    sf::Sprite arrow;
    arrow.setPosition(pos);
    arrow.setTexture(m_BoxTexture);
    arrow.setTextureRect(sf::IntRect(96, 0, 32, 32));
    arrow.setRotation(facing * 90);
    arrow.setColor(color);
    arrow.setScale(scale);
    if(center) arrow.setOrigin(Tool::centerSprite(arrow));
    window.draw(arrow);
}

void Boxes::PlaySelect(float pitch) {
    m_BoxSoundConfirm.setPitch(pitch);
    m_BoxSoundSelect.play();
}

void Boxes::PlayCancel(float pitch) {
    m_BoxSoundConfirm.setPitch(pitch);
    m_BoxSoundCancel.play();
}

void Boxes::PlayConfirm(float pitch) {
    m_BoxSoundConfirm.setPitch(pitch);
    m_BoxSoundConfirm.play();
}

sf::Font& Boxes::getFont(int font) {
    switch (font) {
    case Box::FONT_SUPERTHIN:
        return m_FontSuperThin;
        break;
    case Box::FONT_MESSY:
        return m_FontMessy;
        break;
    case Box::FONT_NORMAL:
        return m_FontNice;
        break;
    case Box::FONT_ROMAN:
        return m_FontRoman;
        break;
    case Box::FONT_OUTLINE:
        return m_FontOutline;
        break;
    case Box::FONT_ZOMBIE:
        return m_FontZombie;
        break;
    case Box::FONT_NINJA:
        return m_FontNinja;
        break;
    case Box::FONT_WONDER:
        return m_FontWonderland;
        break;
    }
    return m_FontNice;
}

int Boxes::Menu(std::initializer_list<std::string> choices, sf::Vector2f pos) {
    unsigned int length = 0;
    for (auto& elem : choices) if (elem.size() > length) length = elem.size();

    sf::Vector2f menuSize((length * 18.0f) + 100.0f, (choices.size() * 41.0f) + 100.0f);
    addBox("Menu", menuSize, pos);

    m_MenuChoices = choices.size();

    for (int i = 0; i < m_MenuChoices; ++i) {
        getBox("Menu").addText(*(choices.begin() + i), sf::Vector2f(48, 16 + (i * 50)), 36);
    }
    return choices.size();
}

bool Boxes::hasBox(const std::string& boxname) {
    for(auto& box : m_Boxes) {
        if(box.getName() == boxname) return true;
    }
    return false;
}

int Boxes::getNumberOfMenuChoices() {
    return m_MenuChoices;
}











