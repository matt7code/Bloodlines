#include "states/combat_cutscene.h"
/*
# Reminder: Enemy is ID# 0-14
# Reminder: Party is ID# 15-21
# Examples:

START
END

PROP 22 LOAD 0
PROP 22 ADD_FRAMES 1 2 3 4 5 6 7 8 9
PROP 22 SET_STYLE OSCILLATE
PROP 22 SET_FRAME 0
PROP 22 SET_FRAMETIME 0.0333333
PROP 22 UNLOAD

SELECT ACTOR
SELECT TARGET
SELECT 16

(First SELECT the active prop.)
ANIMATE START
ANIMATE START ALL
ANIMATE STOP
ANIMATE STOP ALL
ANIMATE SET_INDEX 0
ANIMATE SET_NAME UpDown
ANIMATE SET_STYLE END
ANIMATE SET_STYLE LOOP
ANIMATE SET_STYLE OSCILLATE
ANIMATE SET_STYLE OSCILLATE FORWARD
ANIMATE SET_STYLE OSCILLATE BACK

SPRITE MOVE 7 -7
SPRITE PLACE 300 450
SPRITE SHOW
SPRITE HIDE
SPRITE SCALE x y
SPRITE RECOLOR r g b a
SPRITE ROTATE <relative angle>
SPRITE ROTATION <angle to set rotation to>
SPRITE RECTANGLE <left top width height>

WAIT 1.0f
WAIT ANIM

*/
Combat_Cutscene Combat_Cutscene::m_Combat_Cutscene;

void Combat_Cutscene::Init(Engine *eng) {
    InitializeValues(eng);
    LoadScript();
    LoadProps();
}

void Combat_Cutscene::Update(Engine* e) {
    m_TimeDelta       = e->getTimeRunning().getElapsedTime() - m_TimePrevious;
    m_TimePrevious    = e->getTimeRunning().getElapsedTime();
    m_TimeInPhase    += m_TimeDelta;

    for (auto& tempProp : m_Props) {
        Prop& prop = tempProp.second;
        if (prop.getAnimated()) {
            if(m_TimePrevious.asSeconds() - prop.getPrevAnimTime().asSeconds() > prop.getAnimFrameTime()) {
                UpdateAnimatedProp(prop);
            }
        }

        /// The Prop is currently moving onscreen, ie. Not the internal animation handled above.
        if (prop.getActive() && prop.getAnimationRunning()) {
            int index = CalculateAnimationIndex(prop);
            if (prop.getAnimationRunning()) {
//                if(prop.getAnimated()) {
//                    std::cout << "Animated ";
//                    for (auto& sprite : prop.getAnimSprites()) sprite.move((*m_pAnims)[prop.getAnimationName()][index]);
//                } else {
//                    std::cout << "Not Animated ";
//                    std::cout << "prop.getAnimationName(): " << prop.getAnimationName() << " Index: " << index << std::endl;
                if( m_pAnims->find(prop.getAnimationName()) == m_pAnims->end()) {
                    std::cerr << "Animation not found in Anim Array: " << prop.getAnimationName() << std::endl;
                } else {
                    if( (int)(*m_pAnims)[prop.getAnimationName()].size() < index ) {
                        std::cerr << "Frame index not found in animation " << prop.getAnimationName() << " (Frame " << index << " of " << (*m_pAnims)[prop.getAnimationName()].size() << ")" << std::endl;
                    } else {
                        prop.getSprite().move((*m_pAnims)[prop.getAnimationName()][index]);
                    }
                }
            }
        }
    }

    if(m_waiting && !m_waitingAnim && (m_TimePrevious.asSeconds() > m_TimeWait.asSeconds() + m_waitLength)) m_waiting = false;
    if(!m_waiting) ProcessLine();
}

void Combat_Cutscene::ProcessLine() {
    if(m_script.empty()) return;

    std::vector<std::string> line = Tool::Tokenize(m_script[m_cl]);
    std::string command = line[0];

    if (command == "START") {}

    else if (command == "END") e->PopState();

    else if (command == "PROP") {
        // Get the index
        std::stringstream ss1;
        unsigned index = 0;
        ss1 << line[1];
        ss1 >> index;

        if(line[2] == "LOAD") { /// Only load can create a new prop.
            //std::cout << "Loading new prop." << std::endl;
            std::stringstream ss;
            unsigned newSprite = 0;
            ss << line[3];
            ss >> newSprite;
            if (newSprite > m_pSprites->size() - 1 ) std::cerr << "Prop Load command attempted to load out of bounds frame." << std::endl;
            else {
                m_Props[index].setActive(true);
                m_Props[index].setSprite((*m_pSprites)[newSprite]);
                m_Props[index].getSprite().setPosition(960, 540);
                /// Set this freshly loaded prop to the active prop.
                m_pActiveProp = &m_Props[index];
                //std::cout << "m_PropMap size: " << m_PropMap.size() << std::endl;
            }
        } else {
            auto it = m_Props.find(index);
            if (it == m_Props.end()) std::cerr << "PROP targeted a non-existent index: " << index << std::endl;
            else {
                // Guaranteed Valid Index
                if(line[2] == "ADD_FRAMES") {
                    //std::cout << "Adding Frames" << std::endl;
                    /// Add original frame to animated frame stack
                    m_Props[index].getAnimSprites().push_back(m_Props[index].getSprite());
                    sf::Vector2f mainPos = m_Props[index].getSprite().getPosition();
                    m_Props[index].setAnimated(true);
                    for(unsigned i = 3; i != line.size(); ++i) {
                        std::stringstream ss;
                        unsigned newSprite = 0;
                        ss << line[i];
                        ss >> newSprite;
                        if (newSprite > m_pSprites->size() - 1 ) std::cerr << "Prop ADD_FRAMES command attempted to load out of bounds frame." << std::endl;
                        else {
                            m_Props[index].getAnimSprites().push_back((*m_pSprites)[newSprite]);
                            m_Props[index].getAnimSprites().back().setPosition(mainPos);
                            //std::cout << "m_PropMap index " << index << ": Adding sprite #" << newSprite << " to it." << std::endl;
                        }
                    }
                    //std::cout << "Prop frame stack size: " << m_PropMap[index].getAnimSprites().size() << std::endl;
                } else if(line[2] == "SET_FRAME") {
                    unsigned frame = 0;
                    if(line.size() < 4) {
                        std::cerr << "SET_FRAME command missing arguments." << std::endl;
                        frame = 0;
                    } else {
                        std::stringstream ss;
                        ss << line[3];
                        ss >> frame;
                    }
                    if (frame > m_Props[index].getAnimSprites().size() - 1) std::cerr << "Prop SET_FRAME targeted out of bounds frame: " << frame << std::endl;
                    else m_Props[index].setAnimCurrentFrame(frame);
                } else if(line[2] == "SET_FRAMETIME") {
                    std::stringstream ss;
                    float time = 0;
                    ss << line[3];
                    ss >> time;
                    m_Props[index].setAnimFrameTime(time);
                } else if(line[2] == "SET_STYLE") {
                    unsigned mode = 0;
                    if (line[3] == "END") mode = Prop::ANIM_END;
                    else if (line[3] == "LOOP") mode = Prop::ANIM_LOOP;
                    else if (line[3] == "OSCILLATE_BACKWARD") mode = Prop::ANIM_OSCILLATE_BACKWARD;
                    else if (line[3] == "OSCILLATE_FORWARD") mode = Prop::ANIM_OSCILLATE_FORWARD;
                    else if (line[3] == "OSCILLATE") mode = Prop::ANIM_OSCILLATE_FORWARD;
                    else (mode = 999);
                    if(mode < Prop::ANIM_MAX) m_Props[index].setAnimMode(mode);
                    else std::cerr << "PROP STYLE targeted a non-existent mode: " << index << std::endl;
                } else if(line[2] == "UNLOAD") {
                    auto it = m_Props.find(index);
                    if (it != m_Props.end()) m_Props.erase(it);
                    else std::cerr << "PROP UNLOAD targeted a non-existent index: " << index << std::endl;
                } else {
                    std::cerr << "Malformed PROP command: " << Tool::reconstituteString(line) << std::endl;
                }
            }
        }
    }

    else if (command == "SELECT") {
        if(line[1] == "ACTOR")     m_pActiveProp = &m_Props[m_pAction->actor];
        else if(line[1] == "TARGET")    m_pActiveProp = &m_Props[m_pAction->target];
//        else if(line[1] == "PARTY")     m_pActiveProp = &m_PropMap[m_pAction->actor]; /// Needs Work
//        else if(line[1] == "ENEMIES")   m_pActiveProp = &m_PropMap[m_pAction->actor]; /// Needs Work
        else {
            std::stringstream ss;
            ss.str(line[1]);
            int propNum;
            ss >> propNum;
            auto it = m_Props.find(propNum);
            if(it == m_Props.end()) {
                std::cerr << "SELECT ERROR Invalid Prop requested: " << Tool::reconstituteString(line) << std::endl;
            } else {
                m_pActiveProp = &m_Props[propNum];
            }
        }
        if(m_pActiveProp && m_pActiveProp->getActive() == false) {
            std::cerr << "SELECT ERROR Inactive Prop requested: " << Tool::reconstituteString(line) << std::endl;
            m_pActiveProp = nullptr;
        }
    }

    else if (command == "ANIMATE") {
        if (!m_pActiveProp) std::cerr << "No Active Prop set for ANIMATE." << std::endl;
        {
            if(line[1] == "SET_INDEX") {
                std::stringstream ss;
                int index;
                ss << line[2];
                ss >> index;
                m_pActiveProp->setAnimationIndex(index);
            } else if (line[1] == "SET_NAME") {
                m_pActiveProp->setAnimationName(line[2]);
            } else if (line[1] == "SET_STYLE") {
                if (line[2] == "LOOP") {
                    m_pActiveProp->setAnimationStyle(Prop::ANIM_LOOP);
                    //std::cout << "LOOP SET" << std::endl;
                } else if (line[2] == "OSCILLATE" || line[2] == "OSCILLATE_FORWARD") {
                    m_pActiveProp->setAnimationStyle(Prop::ANIM_OSCILLATE_FORWARD);
                } else if (line[2] == "OSCILLATE_BACK") {
                    m_pActiveProp->setAnimationStyle(Prop::ANIM_OSCILLATE_BACKWARD);
                    unsigned index;
                    index = (*m_pAnims)[m_pActiveProp->getAnimationName()].size();
                    m_pActiveProp->setAnimationIndex(index);
                } else {
                    m_pActiveProp->setAnimationStyle(Prop::ANIM_END);
                }
            } else if (line[1] == "START") {
                m_pActiveProp->setAnimationRunning(true);
                if(line.size() > 2 && line[2] == "ALL") {
                    //std::cout << "Animating All" << std::endl;
                    for (auto& tempProp : m_Props) {
                        Prop& prop = tempProp.second;
                        if(prop.getAnimationName() != "None") prop.setAnimationRunning(true);
                    }
                }
            } else if (line[1] == "STOP") {
                m_pActiveProp->setAnimationRunning(false);
                if(line.size() > 2 && line[2] == "ALL") {
                    //std::cout << "Animating All" << std::endl;
                    for (auto& tempProp : m_Props) {
                        Prop& prop = tempProp.second;
                        if(prop.getAnimationName() != "None") prop.setAnimationRunning(false);
                    }
                }
            }
        }
    }

    else if (command == "SPRITE") {
        if (line[1] == "MOVE") {
            if(m_pActiveProp) {
                float x, y;
                std::stringstream ss;
                ss << line[2] << " " << line[3];
                ss >> x >> y;
                if(m_pActiveProp->getAnimated()) {
                    for(auto& sprite : m_pActiveProp->getAnimSprites()) sprite.move(x, y);
                } else {
                    m_pActiveProp->getSprite().move(x, y);
                }
            } else {
                std::cerr << "SPRITE MOVE Error: No Active Prop" << std::endl;
            }



        } else if (line[1] == "SHOW") {
            m_pActiveProp->setActive(true);
        } else if (line[1] == "HIDE") {
            m_pActiveProp->setActive(false);
        } else if (line[1] == "RECOLOR") {
            if(line.size() == 6) {
                int r, g, b, a;
                std::stringstream ss;
                ss << line[2] << " " << line[3] << " " << line[4] << " " << line[5];
                ss >> r >> g >> b >> a;
                if(m_pActiveProp->getAnimated()) {
                    for(auto& sprite : m_pActiveProp->getAnimSprites()) sprite.setColor(sf::Color(r, g, b, a));
                } else {
                    m_pActiveProp->getSprite().setColor(sf::Color(r, g, b, a));
                }
            } else std::cerr << "Malformed SPRITE RECOLOR command: " << Tool::reconstituteString(line) << std::endl;
        } else if (line[1] == "SCALE") {
            if(line.size() == 4) {
                float x, y;
                std::stringstream ss;
                ss << line[2] << " " << line[3];
                ss >> x >> y;
                if(m_pActiveProp->getAnimated()) {
                    for(auto& sprite : m_pActiveProp->getAnimSprites()) sprite.setScale(sf::Vector2f(x, y));
                } else {
                    m_pActiveProp->getSprite().setScale(sf::Vector2f(x, y));
                }
            } else std::cerr << "Malformed SPRITE SCALE command: " << Tool::reconstituteString(line) << std::endl;
        } else if (line[1] == "ROTATE") {
            if(line.size() == 3) {
                float x;
                std::stringstream ss;
                ss << line[2];
                ss >> x;
                if(m_pActiveProp->getAnimated()) {
                    for(auto& sprite : m_pActiveProp->getAnimSprites()) sprite.rotate(x);
                } else {
                    m_pActiveProp->getSprite().rotate(x);
                }
            } else std::cerr << "Malformed SPRITE ROTATE command: " << Tool::reconstituteString(line) << std::endl;
        } else if (line[1] == "ROTATION") {
            if(line.size() == 3) {
                float x;
                std::stringstream ss;
                ss << line[2];
                ss >> x;
                if(m_pActiveProp->getAnimated()) {
                    for(auto& sprite : m_pActiveProp->getAnimSprites()) sprite.setRotation(x);
                } else {
                    m_pActiveProp->getSprite().setRotation(x);
                }
            } else std::cerr << "Malformed SPRITE ROTATION command: " << Tool::reconstituteString(line) << std::endl;
        } else if (line[1] == "RECTANGLE") {
            if(line.size() == 6) {
                unsigned left, top, width, height;
                std::stringstream ss;
                ss << line[2] << " " << line[3] << " " << line[4] << " " << line[5];
                ss >> left >> top >> width >> height;
                m_pActiveProp->getSprite().setTextureRect(sf::IntRect(left, top, width, height));
            } else std::cerr << "Malformed SPRITE RECTANGLE command: " << Tool::reconstituteString(line) << std::endl;
        } else if (line[1] == "PLACE") {
            if(m_pActiveProp) {
                float x, y;
                std::stringstream ss;
                ss << line[2] << " " << line[3];
                ss >> x >> y;
                if(m_pActiveProp->getAnimated()) {
                    for(auto& sprite : m_pActiveProp->getAnimSprites()) sprite.setPosition(x, y);
                } else {
                    m_pActiveProp->getSprite().setPosition(x, y);
                }

            } else {
                std::cerr << "SPRITE PLACE Error: No Active Prop" << std::endl;
            }
        } else std::cerr << "PROP SPRITE malformed command: " << Tool::reconstituteString(line) << std::endl;

    } else if (command == "WAIT") {
        m_waiting = true;
        if (line[1] == "ANIM") {
            if (m_pActiveProp && m_pActiveProp->getAnimationStyle() != Prop::ANIM_END) {
                m_pActiveProp->setAnimationRunning(true);
                std::cerr << "[Combat_Cutscene] ANIM_WAIT changed to 3 seconds, since current ANIM_STYLE has no ending." << std::endl;
                m_waitingAnim = false;
                m_waitLength = 3.0f;
                m_TimeWait = e->getTimeRunning().getElapsedTime();
            } else {
                m_waitingAnim = true;
            }
        } else {
            m_TimeWait = e->getTimeRunning().getElapsedTime();
            std::stringstream ss;
            if(line.size() == 1) m_waitLength = 3.0f;
            else {
                ss << line[1];
                ss >> m_waitLength;
            }
        }
    }

    else {
        std::cerr << "[Combat_Cutscene] Unknown Command: " << command << std::endl;
    }
    m_cl++;
    if (!m_waiting) ProcessLine();
}

void Combat_Cutscene::Draw(Engine* eng) {
    e->m_RenderWindow.draw(*m_pBackground);

//    for(auto it = m_Props.begin(); it != m_Props.end(); ++it)
//        if(it->second.getActive()) eng->m_RenderWindow.draw(it->second.getSprite());

    for (unsigned i = 0; i < 15; ++i) {
        auto it = m_Props.find(m_foeOrder[i]);
        if (it != m_Props.end() && it->second.getActive())
            e->m_RenderWindow.draw(it->second.getSprite());
    }

    for (unsigned i = 15; i < PROP_LIMIT; ++i) {
        auto it = m_Props.find(i);
        if (it != m_Props.end() && it->second.getActive()) {
            e->m_RenderWindow.draw(it->second.getSprite());
            //std::cout << "P" << i << " " << std::endl;
        }
    }

    if (m_debug) DrawDebug();
    m_pBoxes->DrawBoxes(e->m_RenderWindow);
}

void Combat_Cutscene::HandleEvents(Engine * e) {
    sf::Event event;
    while (e->m_RenderWindow.pollEvent(event)) {
        if (event.type == sf::Event::Closed) e->Quit();

        if (event.type == sf::Event::KeyPressed) {
            switch(event.key.code) {
//            case sf::Keyboard::Return:
//            case sf::Keyboard::Space:
//                if(m_waiting) m_waiting = false;
//                break;
            /* RESTRICTED */
            case sf::Keyboard::Q:
                e->PopState();
                e->PopState();
                break;
            case sf::Keyboard::Escape:
                Tool::DumpVecStr(m_script);
                e->PopState();
                break;
            case sf::Keyboard::F1:
                m_debug =! m_debug;
                if (m_debug) {
                    if (!m_pBoxes->hasBox("Debug")) m_pBoxes->addBox("Debug", sf::Vector2f(0,0), sf::Vector2f(50,100),Box::BOX_NONE);
                } else {
                    if (m_pBoxes->hasBox("Debug")) m_pBoxes->removeBox("Debug");
                }
                break;
            default:
                break;
            }
        }
    }
}

void Combat_Cutscene::ProcessMessages() {
    const int text_size = 30;
    if(m_pBoxes->hasBox("Messages")) m_pBoxes->removeBox("Messages");
    if(m_pMessages->empty()) {
        m_pBoxes->addBox("Messages", sf::Vector2f(1020, 38 + (m_pMessages->size() * text_size)), sf::Vector2f(450, 16));
        int n = 0;
        for (auto it = m_pMessages->begin(); it != m_pMessages->end(); ) {
            it->duration -= m_TimeDelta;
            if(it->duration < sf::Time::Zero) {
                m_pMessages->erase(it);
            } else {
                m_pBoxes->getBox("Messages").addText(it->message, sf::Vector2f(20.0f, 10 + (n * text_size)), text_size);
                ++it;
                ++n;
            }
        }
    }
}

void Combat_Cutscene::DrawDebug() {
    if(!m_pBoxes->hasBox("Debug")) return;
    m_pBoxes->getBox("Debug").getText().clear();
    std::stringstream ss;
    ss << "     Time: " << m_TimeInPhase.asSeconds() << std::endl;
    ss << "    Actor: " << m_pAction->actor << std::endl;
    ss << "   Target: " << m_pAction->target << std::endl;
    ss << "   Action: " << m_pAction->action << std::endl;
    ss << "SubAction: " << m_pAction->sub_action << std::endl;
    ss << std::endl;
    ss << "m_pPropActive : ";
    if(m_pActiveProp) ss << &(*m_pActiveProp) << std::endl;
    else ss << "NONE" << std::endl;

    std::vector<sf::Sprite>& spellSprites = (*m_pSprites);
    for (unsigned i = 0; i < spellSprites.size(); ++i) {
        spellSprites[i].setScale(sf::Vector2f(2.0f, 2.0f));
        spellSprites[i].setPosition(100 + (i*74), 300);
        e->m_RenderWindow.draw(spellSprites[i]);
    }
    m_pBoxes->getBox("Debug").addText(ss.str());
}

void Combat_Cutscene::LoadScript() {
    std::cout << "Combat_Cutscene::LoadScript() Action: " << m_pAction->action << std::endl;
    switch(m_pAction->action) {
    case Action::COMMAND_ATTACK:
        if(m_pAction->actor > 14)   m_scriptName = "attack_melee_basic_player";
        else                        m_scriptName = "attack_melee_basic_monster";
        break;
    case Action::COMMAND_SPECIAL:
        m_scriptName = "high_chop";
        switch(m_pAction->sub_action) {
            case Special::SPELL_HEROISM:
                break;
            case Special::SPELL_BRACE:
                break;
            case Special::SPELL_FOCUS:
                break;
            case Special::SPELL_CHALLENGE:
                break;
            case Special::SPELL_BASH:
                break;

            // Rogue
            case Special::SPELL_EXAMINE:
                break;
            case Special::SPELL_EXPOSE_WEAKNESS:
                break;
            case Special::SPELL_PEEK:
                break;
            case Special::SPELL_SNARE:
                break;
            case Special::SPELL_SNEAK:
                break;
            case Special::SPELL_STEAL:
                break;

            // Wizard
            case Special::SPELL_MISSILE:
                break;
            case Special::SPELL_LIGHTNING_BOLT:
                m_scriptName = "spell_lightning_bolt";
                break;
            case Special::SPELL_BURN:
                m_scriptName = "spell_burn";
                break;
            case Special::SPELL_FIREBALL:
                break;
            case Special::SPELL_GUSH_WATER:
                break;
            case Special::SPELL_SLOW:
                break;
            case Special::SPELL_HASTE:
                break;
            case Special::SPELL_EARTHQUAKE:
                break;
            case Special::SPELL_FIRE_STORM:
                break;
            case Special::SPELL_LIGHTNING_STORM:
                break;
            case Special::SPELL_SNOW_STORM:
                break;
            case Special::SPELL_FLOOD:
                break;
            case Special::SPELL_PETRIFY:
                break;
            case Special::SPELL_VOLCANO:
                break;
            case Special::SPELL_ASTEROID:
                break;

            // Cleric
            case Special::SPELL_HEAL:
                break;
            case Special::SPELL_BLESS:
                break;
            case Special::SPELL_CURSE:
                break;
            case Special::SPELL_SICK:
                break;
            case Special::SPELL_BLIND:
                break;
            case Special::SPELL_CURE:
                break;
            case Special::SPELL_RESTORE:
                break;
            case Special::SPELL_REMOVECURSE:
                break;
            case Special::SPELL_REGEN:
                break;
            case Special::SPELL_POISON:
                break;
            case Special::SPELL_RESURRECT:
                break;
            case Special::SPELL_BLESS_ALL:
                break;
            case Special::SPELL_HEAL_ALL:
                break;
            case Special::SPELL_REGEN_ALL:
                break;
            case Special::SPELL_RES_ALL:
                break;
            case Special::SPELL_RESTORE_ALL:
                break;
            default:
                break;
        }
        std::cout << "Script Selected: " << m_scriptName << std::endl;
        break;
    case Action::COMMAND_USE:
        if(e->Items[m_pAction->sub_action].subtype == Item::ItemType_Potion)        m_scriptName = "UsePotion";
        else if(e->Items[m_pAction->sub_action].subtype == Item::ItemType_Scroll)   m_scriptName = "UseScroll";
        else if(e->Items[m_pAction->sub_action].subtype == Item::ItemType_Wand)     m_scriptName = "UseWand";
        else m_scriptName = "UseGenericItem";
        break;
    case Action::COMMAND_GUARD:
        m_scriptName = "Guard";
        break;
    case Action::COMMAND_FLEE:
        m_scriptName = "Flee";
        break;
    case Action::COMMAND_STANCE:
        m_scriptName = "Stance";
        break;
    default:
        m_scriptName = "None";
        break;
    }

    std::string originalName = m_scriptName;
    auto script = m_pSpells->find(m_scriptName);
    if (script == m_pSpells->end()) std::cerr << "[Combat_Cutscene] ERROR: Script not found: " << originalName << std::endl;
    else {
        m_script = script->second;
        // Debug:
        Tool::DumpVecStr(m_script);
    }
}

void Combat_Cutscene::LoadProps() {
    // Reminder: Enemy is ID# 0-14
    // Reminder: Party is ID# 15-21
    std::vector<Hero>& ArenaRef = *m_pArena;

    for (int i = 0; i < 22; ++i) {
        if (ArenaRef[i].isActive()) {
            m_Props[i].setSprite(ArenaRef[i].getSprite());
            m_Props[i].setActive(true);
        }
    }
}

void Combat_Cutscene::Cleanup(Engine * eng) {
    InitializeValues(eng);
}

void Combat_Cutscene::InitializeValues(Engine * eng) {
    e               = eng;
    m_cl            = 0;
    m_pActiveProp   = nullptr;
    m_waitingAnim   = false;

    m_Props.clear();
    m_script.clear();

    m_waitLength    = 5.0f;
    m_waiting       = false;

    m_TimeDelta     = sf::Time::Zero;
    m_TimeInPhase   = sf::Time::Zero;
    m_TimePrevious  = e->getTimeRunning().getElapsedTime();

    m_pArena        = e->m_pCombat_Arena;
    m_pBoxes        = e->m_pCombat_Boxes;
    m_pBackground   = e->m_pCombat_Background;
    m_pMessages     = e->m_pCombat_Messages;
    m_pAction       = e->m_pCombat_Action;

    m_pSpells       = &e->m_Spells;
    m_pSprites      = &e->m_SpellSprites;
    m_pAnims        = &e->m_SpellAnims;
    m_pSounds       = &e->m_SpellSounds;

    m_pActor        = &(*m_pArena)[m_pAction->actor];

    //std::cout << "Target: " << m_pAction->target << std::endl;
    m_pTarget = &(*m_pArena)[m_pAction->target];
    m_originalTargetPos = m_pActor->getSprite().getPosition();
    //std::cout << "Cutscene Phase: " << m_pAction->actor << std::endl;

    // Big Problem
    m_originalActorPos  = m_pTarget->getSprite().getPosition();
    if (m_pBoxes->hasBox("Debug")) m_pBoxes->getBox("Debug").getText().clear();
    m_scriptName = "None";
}

unsigned Combat_Cutscene::CalculateAnimationIndex(Prop& prop) {
    int index = prop.getAnimationIndex();
    int animLimit = (*m_pAnims)[prop.getAnimationName()].size();

    /// Handle oscillation extents.
    if(index == 0 && prop.getAnimationStyle() == Prop::ANIM_OSCILLATE_BACKWARD) prop.setAnimationStyle(Prop::ANIM_OSCILLATE_FORWARD);
    int style = prop.getAnimationStyle();

    /// Advance sequence.
    if(style == Prop::ANIM_OSCILLATE_BACKWARD) --index;
    else ++index;

    if (index == animLimit) {
        if (style == Prop::ANIM_END) {
            prop.setAnimationRunning(false);
            if(m_waitingAnim) {
                m_waitingAnim = false;
                m_waiting = false;
            }
            index = 0;
        } else if (style == Prop::ANIM_LOOP) {
            index = 0;
        } else if (style == Prop::ANIM_OSCILLATE_FORWARD) {
            prop.setAnimationStyle(Prop::ANIM_OSCILLATE_BACKWARD);
        }
    }
    prop.setAnimationIndex(index);
    return index;
}

void Combat_Cutscene::UpdateAnimatedProp(Prop& prop) {
    prop.setPrevAnimTime(m_TimePrevious);
    unsigned index      = prop.getAnimCurrentFrame();
    unsigned animLimit  = prop.getAnimTotalFrames() - 1;
    unsigned style      = prop.getAnimMode();

    /// Handle Oscillation Extents
    if(index == 0 && style == Prop::ANIM_OSCILLATE_BACKWARD) prop.setAnimMode(Prop::ANIM_OSCILLATE_FORWARD);
    else if(index == animLimit && prop.getAnimMode() == Prop::ANIM_OSCILLATE_FORWARD) prop.setAnimMode(Prop::ANIM_OSCILLATE_BACKWARD);
    style = prop.getAnimMode();

    /// Advance sequence.
    if(style == Prop::ANIM_OSCILLATE_BACKWARD) --index;
    else ++index;

    if (index == animLimit && style == Prop::ANIM_END) {
        prop.setAnimated(false);
        index = 0;
    } else if (index == animLimit && style == Prop::ANIM_LOOP) {
        index = 0;
    }
    if (index < prop.getAnimTotalFrames()) {
        prop.setSprite(prop.getAnimSprites()[index]);
        prop.setAnimCurrentFrame(index);
    } else std::cerr << "Animated prop frame was out of bounds: " << index << std::endl;
}
