#include <iostream>
#include <SFML/Graphics.hpp>
#include "gameengine.h"
#include "gamestate.h"

void Engine::Init(const char* title, int width, int height, int bpp, bool fullscreen) {
    // Create the main window
    std::cout << "Fullscreen resolution: " << sf::VideoMode::getDesktopMode().width << ", " << sf::VideoMode::getDesktopMode().height << std::endl;
    //if (fullscreen) m_RenderWindow.create(sf::VideoMode(width, height, bpp), title, sf::Style::Fullscreen);
    if (fullscreen) m_RenderWindow.create(sf::VideoMode(sf::VideoMode::getDesktopMode().width, sf::VideoMode::getDesktopMode().height, bpp), title, sf::Style::Fullscreen);
    //if (fullscreen) m_RenderWindow.create(sf::VideoMode(width, height, bpp), title);
    //else m_RenderWindow.create(sf::VideoMode(width, height, bpp), title, sf::Style::None);
    else m_RenderWindow.create(sf::VideoMode(width, height, bpp), title);

    /* Window Position on screen */
    //m_RenderWindow.setPosition(sf::Vector2i(0, 0));
#ifdef DEBUG
    m_RenderWindow.setPosition(sf::Vector2i(320, 0));
#endif

    m_RenderWindow.setMouseCursorVisible(false);
    m_RenderWindow.setVerticalSyncEnabled(true);

    m_ViewBase = m_RenderWindow.getView();
    m_ViewBase.reset(sf::FloatRect(0, 0, 1920, 1080));

    m_fullscreen = fullscreen;
    m_running = true;

    viewExtents.left    = VIEW_LEFT;
    viewExtents.width   = VIEW_RIGHT;
    viewExtents.top     = VIEW_TOP;
    viewExtents.height  = VIEW_BOTTOM;

    std::cout << "Loading: Music" << std::endl;
    m_Music_EpicAdventureTheme.openFromFile("music/es.ogg");
    m_Music_EpicAdventureTheme.setLoop(true);
    m_Music_FightForThisPlanet.openFromFile("music/FightForThisPlanet.ogg");
    m_Music_FightForThisPlanet.setLoop(true);
    m_Music_UltimateVictory.openFromFile("music/UltimateVictory.ogg");
    m_Music_UltimateVictory.setLoop(true);
    m_Music_Yesterday.openFromFile("music/Yesterday.ogg");
    m_Music_Yesterday.setLoop(true);
    m_Music_EpicBattleAndWarMusic.openFromFile("music/EpicBattleAndWarMusic.ogg");
    m_Music_EpicBattleAndWarMusic.setLoop(true);

    std::cout << "Loading: Items" << std::endl;
    LoadItems();
    std::cout << "Loading: Spells" << std::endl;
    LoadSpells();
    std::cout << "Loading: Engine Graphics" << std::endl;
    LoadGraphics(); // numbers, game-wide stuff.
    //DebugItems();
}

void Engine::LoadGraphics()
{
    if(!m_gameGraphics.loadFromFile("graphics/game.png")) std::cerr << "Failed to load graphics/game.png";
    for (int i = 0; i < 10; ++i) {
        m_numbers.push_back(sf::Sprite(m_gameGraphics, sf::IntRect(i*27, 0, 27, 35)));
        Tool::centerSprite(m_numbers.back());
    }
}

void Engine::Cleanup() {
    while (!states.empty()) {
        states.back()->Cleanup(this);
        states.pop_back();
    }
}

void Engine::ChangeState(GameState* state) {
    // Clean up the current state.
    if (!states.empty()) {
        states.back()->Cleanup(this);
        states.pop_back();
    }

    // Store and initiate the new state.
    states.push_back(state);
    states.back()->Init(this);
}

void Engine::PushState(GameState* state) {
    if (!states.empty()) {
        states.back()->Pause(this);
    }

    // Store and initiate the new state.
    states.push_back(state);
    states.back()->Init(this);
}

void Engine::PopState() {
    // cleanup the current state
    if (!states.empty()) {
        states.back()->Cleanup(this);
        states.pop_back();
    }

    // resume previous state
    if (!states.empty()) {
        states.back()->Resume(this);
    } else {
        Quit();
    }
}


void Engine::HandleEvents() {
    // let the state handle events
    states.back()->HandleEvents(this);
}

void Engine::Update() {
    // let the state update the game
    if(!states.empty()) states.back()->Update(this);
}

void Engine::Draw() {
    m_RenderWindow.clear();
    if(!states.empty()) states.back()->Draw(this);
    m_RenderWindow.display();
}

std::string Engine::getTimeRunningString() {
    std::stringstream ss;

    long t = (long)getTimeRunning().getElapsedTime().asSeconds();

    long s = t % 60;
    long m = (t / 60) % 60;
    long h = (t / 3600) % 24;
    long d = (t / 86400) % 7;
    long w = (t / 604800);

    if (w > 0) ss << w << (w < 2 ? " week, " : " weeks, ");
    if (d > 0) ss << d << (d < 2 ? " day, " : " days, ");
    if (h > 0) ss << h << (h < 2 ? " hour, " : " hours, ");
    if (m > 0) ss << m << (m < 2 ? " minute, " : " minutes, ");
    if (s > 0) ss << s << (s < 2 ? " second " : " seconds ");

    return ss.str();
}

void Engine::playMusic(int music) {
    // Can't remember where or how I set m_music... (Jan, 2021)
    //if (!m_music) return;
    switch(music) {
    case SONG_WORLD:
        m_Music_UltimateVictory.play();
        break;
    case SONG_THEME:
        m_Music_EpicAdventureTheme.play();
        break;
    case SONG_BATTLE:
        m_Music_EpicBattleAndWarMusic.play();
        break;
    case SONG_STATUS:
        m_Music_FightForThisPlanet.play();
        break;
    case SONG_TOWN:
        m_Music_Yesterday.play();
        break;
    }
}

void Engine::stopMusic() {
    //if (!m_music) return;
    m_Music_EpicBattleAndWarMusic.stop();
    m_Music_EpicAdventureTheme.stop();
    m_Music_Yesterday.stop();
    m_Music_UltimateVictory.stop();
    m_Music_FightForThisPlanet.stop();
}

void Engine::pauseMusic() {
    //if (!m_music) return;
    m_Music_EpicBattleAndWarMusic.pause();
    m_Music_EpicAdventureTheme.pause();
    m_Music_UltimateVictory.pause();
    m_Music_Yesterday.pause();
    m_Music_FightForThisPlanet.pause();
}

void Engine::ToggleMusic() {
    m_music = !m_music;
    if (!m_music) stopMusic();
}

void Engine::ToggleSound() {
    m_sound = !m_sound;
}

void Engine::DebugItems() {
    for(auto& i : Items) {
        std::cout << "Item ID     : " << i.itemID << std::endl;
        std::cout << "Stackable?  : " << i.stackable << std::endl;
        std::cout << "Quest Item? : " << i.quest << std::endl;
        std::cout << "Equip Slot  : " << i.equipSlot << std::endl;
        std::cout << "Type        : " << i.type << std::endl;
        std::cout << "SubType     : " << i.subtype << std::endl;
        std::cout << "Cost/Value  : " << i.cost << std::endl;
        std::cout << "Value 1     : " << i.value1 << std::endl;
        std::cout << "Value 2     : " << i.value2 << std::endl;
        std::cout << "Value 3     : " << i.value3 << std::endl;
        std::cout << "Name        : " << i.name << std::endl;
        std::cout << "Description : " << i.desc << std::endl;
        std::cout << std::endl;
    }
}

void Engine::LoadItems() {
    Items.clear();
    std::ifstream in("items.db");
    if (!in) return;

    std::string input;
    std::vector<std::string> record;
    while (std::getline(in, input)) {
        if(input[0] == '#') continue;
        if(input.empty()) continue;
        record.push_back(input);
    }
    in.close();

    // Process
    auto it = record.begin();

    std::stringstream ss;
    while(it != record.end()) {
        Item newItem(-1);
        ss.clear();
        ss.str(*it++);
        ss >> newItem.itemID >> newItem.stackable >> newItem.quest >> newItem.equipSlot >> newItem.type >> newItem.subtype >> newItem.cost >> newItem.value1 >> newItem.value2 >> newItem.value3 >> newItem.value4;

        newItem.name = *it++;
        newItem.desc = *it++;

        Items.push_back(newItem);
    }
}

void Engine::LoadSpells() {
    m_Spells.clear();

    /// Texture & Sprites
    if(!m_SpellTexture.loadFromFile("graphics/spells.png")) {
        std::cerr << "Error loading graphics/spells.png" << std::endl;
        return;
    }
    for(unsigned y = 0; y < (m_SpellTexture.getSize().y / 32); ++y) {
        for(unsigned x = 0; x < (m_SpellTexture.getSize().x / 32); ++x) {
            m_SpellSprites.push_back(sf::Sprite(m_SpellTexture, sf::IntRect(x * 32, y * 32, 32, 32)));
            Tool::centerSprite(m_SpellSprites.back());
        }
    }

    /// Scripts
    std::vector<std::string> spell_list;
    std::ifstream in("specials/specials.db");
    int counter = 0;
    if (!in) {
        std::cerr << "Error loading specials/specials.db" << std::endl;
        return;
    } else {
        std::string input;
        while (std::getline(in, input)) {
            if(input[0] == '#' || input.empty()) continue;
            //for (auto &c : input) c = tolower(c);
            //std::cout << "Spell Name Listed: " << input << std::endl;
            spell_list.push_back(input);
            counter++;
        }
        in.close();
        std::cout << counter << " special names listed." << std::endl;
    }

    for(const auto& spell_name : spell_list) {
        std::ifstream in("specials/" + spell_name);
        if (!in) {
            std::cerr << "Error loading specials/" << spell_name << std::endl;
        } else {
            std::cout << "Loading script: " << spell_name << std::endl;
            std::string tag, input;
            std::vector<std::string> script;

            while (std::getline(in, input)) {
                input = Tool::Trim(input);
                if(input.empty()) continue;
                if(input[0] == '#' || input.empty()) continue;
                else script.push_back(input);
            }
            tag = spell_name;
            m_Spells[tag] = script;
            //std::cout << "Loaded: " << tag << std::endl;
            in.close();
        }
    }
    std::cout << "Spell scripts loaded: " << m_Spells.size() << std::endl;

    /// Animations
    std::vector<std::string> AnimList;

    in.open("anims/anims.db");
    if (!in) {
        std::cerr << "File Error opening anims/anims.db for input." << std::endl;
        return;
    } else {
        std::string input;
        while(std::getline(in, input)) {
            AnimList.push_back(input);
        }
    }
    in.close();

    for (auto anim : AnimList) {
        std::ifstream in("anims/" + anim + ".ani");
        if (!in) {
            std::cerr << "File Error opening listed animation for input: " << anim << ".ani" << std::endl;
            return;
        } else {
            std::cout << "Animation loaded: " << anim << ".ani" << std::endl;
            float temp_x, temp_y;
            std::string input;
            std::stringstream ss;
            std::vector<sf::Vector2f> animVec;
            /// Eat the first line, which is the anchor position used in the editor, and not useful in game.
            std::getline(in, input);
            while(std::getline(in, input)) {
                ss.clear();
                ss.str(input);
                ss >> temp_x >> temp_y;
                animVec.push_back(sf::Vector2f(temp_x, temp_y));
            }
            m_SpellAnims[anim] = animVec;
        }
        in.close();
    }
    std::cout << "Spell animations loaded: " << m_SpellAnims.size() << std::endl;

    /// Sounds
    if (!m_SpellSoundBuffer_Swoosh.loadFromFile("sounds/swoosh.wav")) std::cerr << "Failed to open select-button." << std::endl;
    if (!m_SpellSoundBuffer_Thunder.loadFromFile("sounds/thunder.wav")) std::cerr << "Failed to open select-button." << std::endl;
    if (!m_SpellSoundBuffer_Hum.loadFromFile("sounds/hum.wav")) std::cerr << "Failed to open select-button." << std::endl;
    if (!m_SpellSoundBuffer_Zap.loadFromFile("sounds/zap.wav")) std::cerr << "Failed to open select-button." << std::endl;
    if (!m_SpellSoundBuffer_Pew.loadFromFile("sounds/pew.wav")) std::cerr << "Failed to open select-button." << std::endl;
    if (!m_SpellSoundBuffer_Kaboom.loadFromFile("sounds/kaboom.wav")) std::cerr << "Failed to open select-button." << std::endl;
    if (!m_SpellSoundBuffer_AngelSing.loadFromFile("sounds/chorus_sing.wav")) std::cerr << "Failed to open select-button." << std::endl;
    if (!m_SpellSoundBuffer_EvilLaugh.loadFromFile("sounds/evil_laugh.wav")) std::cerr << "Failed to open select-button." << std::endl;
    if (!m_SpellSoundBuffer_FemaleYell.loadFromFile("sounds/female_yell.wav")) std::cerr << "Failed to open select-button." << std::endl;
    if (!m_SpellSoundBuffer_FemaleGrunt.loadFromFile("sounds/female_grunt.wav")) std::cerr << "Failed to open select-button." << std::endl;
    if (!m_SpellSoundBuffer_MaleYell.loadFromFile("sounds/male_yell.wav")) std::cerr << "Failed to open select-button." << std::endl;
    if (!m_SpellSoundBuffer_MaleGrunt.loadFromFile("sounds/male_grunt.wav")) std::cerr << "Failed to open select-button." << std::endl;

    m_SpellSounds.push_back(sf::Sound(m_SpellSoundBuffer_Swoosh));
    m_SpellSounds.push_back(sf::Sound(m_SpellSoundBuffer_Thunder));
    m_SpellSounds.push_back(sf::Sound(m_SpellSoundBuffer_Hum));
    m_SpellSounds.push_back(sf::Sound(m_SpellSoundBuffer_Zap));
    m_SpellSounds.push_back(sf::Sound(m_SpellSoundBuffer_Pew));
    m_SpellSounds.push_back(sf::Sound(m_SpellSoundBuffer_Kaboom));
    m_SpellSounds.push_back(sf::Sound(m_SpellSoundBuffer_AngelSing));
    m_SpellSounds.push_back(sf::Sound(m_SpellSoundBuffer_EvilLaugh));
    m_SpellSounds.push_back(sf::Sound(m_SpellSoundBuffer_FemaleYell));
    m_SpellSounds.push_back(sf::Sound(m_SpellSoundBuffer_FemaleGrunt));
    m_SpellSounds.push_back(sf::Sound(m_SpellSoundBuffer_MaleYell));
    m_SpellSounds.push_back(sf::Sound(m_SpellSoundBuffer_MaleGrunt));
    std::cout << "Spell sounds loaded: " << m_SpellSounds.size() << std::endl;
}
