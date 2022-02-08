#ifndef GAMEENGINE_H
#define GAMEENGINE_H

#include <SFML/Graphics.hpp>
#include <vector>
#include <map>

#include "party.h"
#include "map_level.h"

#include "item.h"
#include "boxes.h"
#include "status_message.h"
#include "combat_action.h"
#include "special.h"

class GameState;

class Engine {
public:
    enum    { MUSIC_PLAY, MUSIC_PAUSE, MUSIC_STOP };
    enum    { SONG_WORLD, SONG_THEME, SONG_BATTLE, SONG_STATUS, SONG_TOWN };
    enum    { SOUND_WOOSH, SOUND_THUNDER, SOUND_HUM, SOUND_ZAP, SOUND_PEW,
              SOUND_KABOOM, SOUND_ANGEL_SING, SOUND_EVIL_LAUGH, SOUND_FEMALE_YELL,
              SOUND_FEMALE_GRUNT, SOUND_MALE_YELL, SOUND_MALE_GRUNT
            };

    Engine() {};

    void Init(const char* title, int width = 800, int height = 600, int bpp = 32, bool fullscreen = false);
    void Cleanup();

    void ChangeState(GameState* state);
    void PushState(GameState* state);
    void PopState();

    void HandleEvents();
    void Update();
    void Draw();

    bool Running() { return m_running; }
    void Quit() { m_running = false; }
//    void                            resetGameClock() { m_TimeRunning.restart(); m_TimePlayingMs = 0; }
    sf::Clock&                      getTimeRunning() { return m_TimeRunning; }
    long                            getTimePlaying() { return m_TimePlayingMs + m_TimeRunning.getElapsedTime().asMilliseconds(); }
    long                            getMillisecondsSinceLoad() { return m_TimeRunning.getElapsedTime().asMilliseconds() - m_LastLoadMs; }
    void                            setTimePlaying( long val ) { m_TimePlayingMs = val; }
    std::string                     getTimeRunningString();
    void                            setLastLoadMilliseconds(long val) { m_LastLoadMs = val; }
    std::string                     getInputString() { return m_inputString; }
    void                            setInputString(std::string val) { m_inputString = val; }

    Party&                          getParty() { return m_party; }
    Level                           m_Level;

    sf::RenderWindow                m_RenderWindow;
    sf::View                        m_View;
    sf::View                        m_ViewBase;
    sf::Clock                       ClockDelta;
    sf::IntRect                     viewExtents;

    void                            ToggleMusic();
    void                            ToggleSound();
    void                            playMusic(int music);
    void                            stopMusic();
    void                            pauseMusic();

    void                            setSaveLoadMode(bool val) { m_SaveLoadMode = val; };
    bool                            getSaveLoadMode() { return m_SaveLoadMode; };
    bool                            m_bCameFromMainMenu;
    sf::Vector3i                    BeaconPos;
    //sf::Vector3i                    ObjectPos;

    std::vector<Item>               Items;
    void LoadItems();
    void SaveItems();
    void DebugItems();

    // Conversation
    int m_prevFacing;
    int m_prevMoveMode;
    Npc* m_conversationTarget = nullptr;
    Npc* m_prevConversationTarget = nullptr;
    int annoyedNPC = 0;
    int getAnnoyed() { return annoyedNPC; }

    void setCurrentScript(std::string val) { m_currentScript = val; }
    std::string getCurrentScript() { return m_currentScript; }

    void setCurrentScriptCombat(std::string val) { m_scriptCombat = val; std::cout << "m_scriptCombat: " << m_scriptCombat << std::endl; }
    std::string getCurrentScriptCombat() { return m_scriptCombat; }

    /// Spells
    void LoadSpells();
    std::map<std::string, std::vector<std::string>> m_Spells;
    std::map<std::string, std::vector<sf::Vector2f>>m_SpellAnims;
    std::vector<sf::Sprite>                         m_SpellSprites;
    std::vector<sf::Sound>                          m_SpellSounds;
    sf::Texture                                     m_SpellTexture;

    /// Variables passing between Combat and Combat_Cutscene.
    std::vector<Hero>*              m_pCombat_Arena;        /**< Pointer to Combat States' Arena (a vector of Heroes) */
    sf::Sprite*                     m_pCombat_Background;   /**< Pointer to Combat States' Background */
    Boxes*                          m_pCombat_Boxes;        /**< Pointer to Combat States' Boxes */
    std::vector<StatusMessage>*     m_pCombat_Messages;     /**< Pointer to Combat States' Status messages */
    Action*                         m_pCombat_Action;       /**< Pointer to current combat action */

    /// Game stuff
    std::vector<sf::Sprite>         m_numbers;
    void                            StopInput() { m_acceptInput = false; }
    void                            StartInput() { m_acceptInput = true; }
    bool                            QueryInput() { return m_acceptInput; }
private:
    void                            LoadGraphics();
    Party                           m_party;

    sf::Music                       m_Music_EpicAdventureTheme;
    sf::Music                       m_Music_FightForThisPlanet;
    sf::Music                       m_Music_UltimateVictory;
    sf::Music                       m_Music_Yesterday;
    sf::Music                       m_Music_EpicBattleAndWarMusic;

    sf::SoundBuffer                 m_SpellSoundBuffer_Swoosh;
    sf::SoundBuffer                 m_SpellSoundBuffer_Thunder;
    sf::SoundBuffer                 m_SpellSoundBuffer_Hum;
    sf::SoundBuffer                 m_SpellSoundBuffer_Zap;
    sf::SoundBuffer                 m_SpellSoundBuffer_Pew;
    sf::SoundBuffer                 m_SpellSoundBuffer_Kaboom;
    sf::SoundBuffer                 m_SpellSoundBuffer_AngelSing;
    sf::SoundBuffer                 m_SpellSoundBuffer_EvilLaugh;
    sf::SoundBuffer                 m_SpellSoundBuffer_FemaleYell;
    sf::SoundBuffer                 m_SpellSoundBuffer_FemaleGrunt;
    sf::SoundBuffer                 m_SpellSoundBuffer_MaleYell;
    sf::SoundBuffer                 m_SpellSoundBuffer_MaleGrunt;

    std::vector<GameState*>        states; /**< A stack of game states. */

    sf::Clock                       m_TimeRunning;
    long                            m_TimePlayingMs;
    long                            m_LastLoadMs;

    bool                            m_running;
    bool                            m_fullscreen = false;
    bool                            m_sound = true;
    bool                            m_music = true;
    bool                            m_SaveLoadMode = false;
    bool                            m_acceptInput = true;
    std::string                     m_currentScript = "None";
    std::string                     m_scriptCombat = "";
    std::string                     m_inputString;

    sf::Texture                     m_gameGraphics;
};

#endif
