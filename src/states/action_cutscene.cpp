#include "states/action_cutscene.h"

/*
    Cutscene commands:

# SAFE TRUE|FALSE
# START
# END
# PAUSE <seconds> <STACK | "">
# FADE <fade length in seconds> <fade percentage start> <fade percentage finish> <Fade color R> <Fade color G> <Fade color B>
# FADE OFF
# PLAY_SOUND <STAIRS | SLEEP>
# STAIRS <UP|DOWN|(int)>
# WAIT

# MUSIC <START|STOP|PLAY (int=song)>
# ANIMATE_WALK <TRUE|FALSE>
# CAM_FOLLOW <TRUE|FALSE>
# CALL <script_name.scr>
# MOVE <NORTH|SOUTH|EAST|WEST> <float amount>
# HEAL <ALL>
# NOTICE <Message for player>

# QUEST_HAS <VALUE>
# QUEST_LACKS <VALUE>
# QUEST_SET <VALUE>
# QUEST_REMOVE <VALUE>

# TELEPORT <World X, World Y, World Z>
# RETURN
# SHOW_PARTY <TRUE/FALSE>
# IMAGE <name> <x_pos> <y_pos>
# IMAGE CLEAR
# SET_RETURN_STRING <message for resuming state>
*/

Action_Cutscene Action_Cutscene::m_Action_Cutscene;

void Action_Cutscene::Init(Engine* eng) {
    e = eng;
    m_scrollText.clear();
    m_scroll.setString("");
    m_script.clear();
    m_Sprites.clear();

    if (!Tool::LoadVecStr(e->m_Level.getName() + "/cutscenes/" + e->getCurrentScript() + ".scr", m_script)) {
        std::cerr << "Script Load Failure: " << e->m_Level.getName() + "/cutscenes/" + e->getCurrentScript() + ".scr" << std::endl;
        e->PopState();
    } else {
        m_Boxes.getBoxes().clear();
        m_paused            = false;
        m_waiting           = false;
        m_paused            = false;
        m_stack             = false;
        m_camFollow         = true;
        m_animateWalk       = true;
        m_showParty         = true;
        m_drawLevel         = true;
        m_scrolling         = false;
        m_scrollTextLoaded  = false;
        m_scrollSpeedMod    = 0.0f;

        e->m_RenderWindow.setView(e->m_ViewBase);

        m_prevPos = e->getParty().getPos();
        m_prevZ = e->m_Level.z_Level;
        m_startTime = e->getTimeRunning().getElapsedTime();
        m_scriptName = e->m_Level.getName() + "/cutscenes/" + e->getCurrentScript() + ".scr";
        m_cl = 0;
        m_full_screen_fade.setSize(sf::Vector2f(1920, 1080));
        m_moveQueue.clear();
    }
}

void Action_Cutscene::HandleEvents(Engine* e) {
    sf::Event event;
    while (e->m_RenderWindow.pollEvent(event)) {
        if (event.type == sf::Event::Closed) e->Quit();

        if (event.type == sf::Event::KeyPressed) {
            switch(event.key.code) {
            case sf::Keyboard::Return:
            case sf::Keyboard::Space:
                if(m_waiting) {
                    m_waiting = false;
                    if(m_scrolling) {
                        m_scroll.setPosition(0, -m_scroll.getLocalBounds().height);
                    }
                }
                break;
            case sf::Keyboard::Down:
                if(m_scrolling) {
                    m_scrollSpeedMod -= SCROLL_SPEED;
                }
                break;
            case sf::Keyboard::Up:
                if(m_scrolling) {
                    m_scrollSpeedMod += SCROLL_SPEED;
                }
                break;
            /* RESTRICTED */
            case sf::Keyboard::Escape:
                Tool::DumpVecStr(m_script);
                m_paused = false;
                e->PopState();
                break;
            default:
                break;
            }
        }
    }
}

void Action_Cutscene::Update(Engine* e) {
    float delta = e->getTimeRunning().getElapsedTime().asSeconds() - m_deltaTime.asSeconds();
    m_deltaTime = e->getTimeRunning().getElapsedTime();

    if(!m_waiting && !m_paused && m_moveQueue.empty()) ProcessLine();

    if(!m_moveQueue.empty()) doMove(delta);

    if(m_paused && (m_deltaTime.asSeconds() > m_pauseTime.asSeconds() + m_pauseLength)) doUnpause(delta);

    if(m_scrolling) doScrollText(delta);

    if(m_fading) doFade(delta);

    if(m_stack)  doStack(delta);

    if(!m_camFollow)
        e->m_Level.Update(e->getTimeRunning(), e->m_View.getCenter(), e->viewExtents);
    else {
        e->m_View.setCenter(e->getParty().getPos());
        e->m_Level.Update(e->getTimeRunning(), e->getParty().getPos(), e->viewExtents);
    }
}

void Action_Cutscene::ProcessLine() {
//    if(m_script[m_cl].empty()) {
//        LOG("!! Empty Script line");
//        e->PopState();
//        return;
//    }
    vecStr line = Tool::Tokenize(m_script[m_cl]);
    //std::cout << "ProcessLine check: m_script[m_cl] = \"" << m_script[m_cl] << "\" m_cl = " << m_cl << " Size: " << m_script.size() << " line.size: " << line.size() << std::endl;
    std::string command = line[0];

    //std::cout << "Line: " << line << std::endl;
//    std::cout << "m_cl: " << m_cl << " (" << Tool::reconstituteString(line) << ")" << std::endl;

    if (command == "START") {
    } else if (command == "COMBAT") {
        //std::cout << "Special Check 1: " << Tool::reconstituteString(line) << std::endl;
        line.erase(line.begin());
        //std::cout << "Special Check 2: " << Tool::reconstituteString(line) << std::endl;
        e->setCurrentScriptCombat(Tool::reconstituteString(line));
        e->PushState( Combat::Instance() );
    } else if (command == "DRAW_LEVEL") {
        if(line[1] == "TRUE") m_drawLevel = true;
        else if(line[1] == "FALSE") m_drawLevel = false;
        else m_drawLevel = !m_drawLevel;
    } else if (command == "SHOW_PARTY") {
        if(line[1] == "TRUE") m_showParty = true;
        else if(line[1] == "FALSE") m_showParty = false;
        else m_showParty = !m_showParty;
    } else if (command == "SET_RETURN_STRING") {
        e->setCurrentScript(line[1]);
    } else if (command == "TELEPORT") {
        float x, y;
        int z;
        std::stringstream ss;
        std::cout << "Teleport loading: " << line[1] << " " << line[2] << " " << line[3] << std::endl;
        ss << line[1] << " " << line[2] << " " << line[3];
        std::cout << "Telporting to: " << ss.str() << std::endl;
        ss >> x >> y >> z;
        e->m_Level.z_Level = z;
        e->getParty().MovePartyTo(sf::Vector2f(x, y));
    } else if (command == "SAFE") {
        if(line.size() == 2) {
            if (line[1] == "TRUE") e->getParty().safe = true;
            else if (line[1] == "FALSE") e->getParty().safe = false;
        } else {
            std::cerr << "Malformed SAFE command: " << Tool::reconstituteString(line) << std::endl;
        }
    } else if (command == "IMAGE") {
        if(line[1] == "CLEAR") {
            //m_tempTexture.
            m_Sprites.clear();
        } else {
            if(!m_tempTexture.loadFromFile(line[1])) std::cerr << "IMAGE Command failed. Error loading \"" << line[1] << "\"" << std::endl;
            else {
                sf::Sprite tempSprite;
                tempSprite.setTexture(m_tempTexture);
                if(line.size() == 4) {
                    std::cout << "Adjusting image placement." << std::endl;
                    float x, y;
                    std::stringstream ss;
                    ss << line[2] << " " << line[3];
                    ss >> x >> y;
                    tempSprite.setPosition(x, y);
                }
                m_Sprites.push_back(tempSprite);
            }
        }
    } else if (command == "RETURN") {
        std::cout << "Returning to previous location." << std::endl;
        e->getParty().MovePartyTo(m_prevPos);
        e->m_Level.z_Level = m_prevZ;
    } else if (command == "QUEST_HAS") {
        std::cout << "Checking Quest Value 1:" << line[1] << " 2:" << line[2] << std::endl;
        bool has_it = e->getParty().getSavedValue(line[1]) == line[2];
        if(has_it) {
            //std::cout << "Value found, continuing script." << std::endl;
        } else {
            //std::cout << "Value not found, exiting script." << std::endl;
            e->PopState();
        }
    } else if (command == "QUEST_LACKS") {
        std::cout << "Checking Quest Value 1:" << line[1] << " 2:" << line[2] << std::endl;
        bool has_it = e->getParty().getSavedValue(line[1]) == line[2];
        if(has_it) {
            //std::cout << "Value found, exiting script." << std::endl;
            e->PopState();
        } else {
            //std::cout << "Value not found, continuing script." << std::endl;
        }
    } else if (command == "QUEST_SET") {
        std::cout << "Setting Quest Value \"" << line[1] << "\" to \"" << line[2] << "\""<< std::endl;
        e->getParty().setSavedValue(line[1], line[2]);
    } else if (command == "QUEST_REMOVE") {
        std::cout << "Deleting Quest Value \"" << line[1] << "\""<< std::endl;
        e->getParty().deleteQuestValue(line[1]);
    } else if (command == "NOTICE") {
        m_Boxes.getBoxes().clear();
        m_Boxes.addBox("Notice", sf::Vector2f(NOTICE_WIDTH, NOTICE_HEIGHT), sf::Vector2f(NOTICE_LEFT, NOTICE_TOP), Box::BOX_CONVO);
        line.erase(line.begin());
        m_Boxes.getBox("Notice").addText(Tool::FormatLine(Tool::reconstituteString(line), 60), sf::Vector2f(20, 10), 45);
        m_waiting = true;
    } else if (command == "MUSIC") {
        if(line[1] == "PLAY") {
            std::stringstream ss;
            ss << line[2];
            int song;
            ss >> song;
            e->playMusic(song);
        } else if(line[1] == "STOP") {
            e->stopMusic();
        } else if(line[1] == "PAUSE") {
            e->pauseMusic();
        }
    } else if (command == "ANIMATE_WALK") {
        if(line.size() > 2) {
            std::cerr << "Malformed ANIMATE_WALK: " << Tool::reconstituteString(line) << std::endl;
            return;
        }
        if(line.size() == 1) m_animateWalk = !m_animateWalk;
        else if(line[1] == "TRUE") m_animateWalk = true;
        else if(line[1] == "FALSE") m_animateWalk = false;
    } else if (command == "CAM_FOLLOW") {
        if(line.size() > 2) {
            std::cerr << "Malformed CAM_FOLLOW: " << Tool::reconstituteString(line) << std::endl;
            return;
        }
        if(line.size() == 1) m_camFollow = !m_camFollow;
        else if(line[1] == "TRUE") m_camFollow = true;
        else if(line[1] == "FALSE") m_camFollow = false;
    } else if (command == "CALL") {
        std::ifstream in(e->m_Level.getName() + "/cutscenes/" + line[1] + ".scr");
        if (!in) std::cerr << "Error loading called script: " << Tool::reconstituteString(line) << std::endl;
        else {
            std::cout << "Call Command at m_cl #" << m_cl << std::endl;
            std::string input;
            vecStr sub_script;
            while (std::getline(in, input)) {
                if(input[0] == '#') continue;
                if(input.empty()) continue;
                Tool::Trim(input);
                std::string temp;
                std::stringstream ss;
                ss.str(input);
                ss >> temp;
                if(temp == "START" || temp == "END") continue;
                sub_script.push_back(input);
            }
            in.close();
            std::cout << "Inserted Called Script: " << sub_script.size() << " lines inserted." << std::endl;
//                m_script.insert(m_script.begin() + (m_cl + n - 1), input);
//                std::cout << "\tm_cl + n: " << (m_cl + n - 1) << std::endl;
            m_script.insert(m_script.begin() + m_cl + 1, sub_script.begin(), sub_script.end());
        }
    } else if (command == "MOVE") {
        //std::cout << Tool::reconstituteString(line) << std::endl;
        sf::Vector2f pos = e->getParty().getPos();
        if (line.size() != 3) line[1] = "MALFORMED";
        float distance;
        std::stringstream ss;
        ss << line[2];
        ss >> distance;
        //std::cout << line[1] << " " << line[2] << std::endl;

        if(line[1] == "NORTH")      m_moveQueue.push_front(sf::Vector2f(pos.x, pos.y - distance));
        else if(line[1] == "SOUTH") m_moveQueue.push_front(sf::Vector2f(pos.x, pos.y + distance));
        else if(line[1] == "EAST")  m_moveQueue.push_front(sf::Vector2f(pos.x + distance, pos.y));
        else if(line[1] == "WEST")  m_moveQueue.push_front(sf::Vector2f(pos.x - distance, pos.y));
        else std::cerr << "Malformed cutscene command in " << m_scriptName << ": " << Tool::reconstituteString(line) << std::endl;
        //m_paused = true;
    } else if (command == "PLAY_SOUND") {
        if(line[1] == "STAIRS") e->m_Level.PlayStairs();
        else if(line[1] == "SLEEP") e->m_Level.PlaySleep();
    } else if (command == "HEAL") {
        if(line.size() == 1 || line[1] == "ALL") {
            for (auto& p : e->getParty().getMembers()) {
                p.setCurHP(p.getMaxHP());
                p.setCurMP(p.getMaxMP());
            }
        } else {
            int member;
            std::stringstream ss;
            ss << line[1];
            ss >> member;
            e->getParty().getMember(member).setCurHP(e->getParty().getMember(member).getMaxHP());
        }
    } else if (command == "STAIRS") {
        int m_elevationChange = 0;
        if(line.size() == 2) {
            if(line[1] == "UP") {
                m_elevationChange = 1;
            } else if(line[1] == "DOWN") {
                m_elevationChange = -1;
            } else {
                std::stringstream ss;
                ss << line[1];
                ss >> m_elevationChange;
            }
            e->m_Level.z_Level += m_elevationChange;
        } else {
            std::cerr << "Malformed STAIRS command: " << Tool::reconstituteString(line) << std::endl;
        }


    } else if (command == "SCROLL_TEXT" || command == "ST") {
        //m_scrollStart = e->getTimeRunning().getElapsedTime();
        line.erase(line.begin());
        if(line.empty()) m_scrollText.push_back(" ");
        else m_scrollText.push_back(Tool::reconstituteString(line));
        //std::cout << "Check: " << Tool::Tokenize(m_script[m_cl + 1])[0] << std::endl;
        std::string next_command = Tool::Tokenize(m_script[m_cl + 1])[0];
        if(next_command != "SCROLL_TEXT" && next_command != "ST") {
            m_scrolling = true;
            std::cout << "m_scrolling activated" << std::endl;
        }
    } else if (command == "FADE") {
        if(line[1] == "OFF" || line[1] == "END") {
            m_fading = false;
        } else {
            m_fading = true;
            m_fadeStart = e->getTimeRunning().getElapsedTime();
            if(line.size() == 1) {
                m_fadeDuration = 5.0f;
                m_fadeDepthStart = 0.0f;
                m_fadeDepthFinish = 100.0f;
                m_fadeColor = sf::Color::Black;
            } else if(line.size() == 2) {
                std::stringstream ss;
                ss << line[1];
                ss >> m_fadeDuration;

                m_fadeDepthStart = 0.0f;
                m_fadeDepthFinish = 100.0f;
                m_fadeColor = sf::Color::Black;
            } else if(line.size() == 3) {
                std::stringstream ss;
                ss << line[1];
                ss >> m_fadeDuration;
                ss.clear();
                ss.str(std::string());
                ss << line[2];
                ss >> m_fadeDepthStart;
                m_fadeDepthFinish = 100.0f;
                m_fadeColor = sf::Color::Black;
            } else if(line.size() == 4) {
                std::stringstream ss;
                ss << line[1];
                ss >> m_fadeDuration;
                ss.clear();
                ss.str(std::string());
                ss << line[2];
                ss >> m_fadeDepthStart;
                ss.clear();
                ss.str(std::string());
                ss << line[3];
                ss >> m_fadeDepthFinish;
                m_fadeColor = sf::Color::Black;
            } else if(line.size() == 7) {
                std::stringstream ss;
                ss << line[1];
                ss >> m_fadeDuration;

                ss.clear();
                ss.str(std::string());
                ss << line[2];
                ss >> m_fadeDepthStart;

                ss.clear();
                ss.str(std::string());
                ss << line[3];
                ss >> m_fadeDepthFinish;

                int r, g, b;

                ss.clear();
                ss.str(std::string());
                ss << line[4];
                ss >> r;

                ss.clear();
                ss.str(std::string());
                ss << line[5];
                ss >> g;

                ss.clear();
                ss.str(std::string());
                ss << line[6];
                ss >> b;
                m_fadeColor = sf::Color(r, g, b);
            } else {
                std::cerr << "Script Error: Malformed FADE instruction on line #" << m_cl << " of " << m_scriptName << std::endl;
            }
            m_full_screen_fade.setFillColor(m_fadeColor);
        }
    } else if (command == "PAUSE") {
        m_paused = true;
        m_pauseTime = e->getTimeRunning().getElapsedTime();
        std::stringstream ss;
        if(line.size() == 1) m_pauseLength = 5.0f;
        else {
            ss << line[1];
            ss >> m_pauseLength;
        }
        if(line.size() > 2 && line[2] == "STACK") m_stack = true;
    } else if (command == "END") {
        //std::cout << "Scene Ended" << std::endl;
        //std::cout << "Ending Script" << std::endl;
        e->PopState();
        return;
    } else if (command == "WAIT") {
        m_waiting = true;
    } else {
        std::cerr << "CRITICAL ERROR: Bad Command in Cutscene \"" << e->getCurrentScript() << "\"." << std::endl;
        std::cerr << "  Command: " << command << std::endl;
        std::cerr << " CurrLine: " << m_cl << std::endl;
    }
    m_cl++;
}

void Action_Cutscene::Draw(Engine* e) {
    if(m_drawLevel) {
        //std::cout << " >> Draw: Cutscene << " << std::endl;
        e->m_RenderWindow.setView(e->m_View);
        e->m_Level.Draw(e->m_RenderWindow);
        if(m_showParty) e->getParty().Draw(e->m_RenderWindow);

        e->m_RenderWindow.setView(e->m_ViewBase);
        m_Boxes.DrawBoxes(e->m_RenderWindow);
    }

    if(!m_Sprites.empty()) e->m_RenderWindow.draw(m_Sprites.back());

    if(m_fading) e->m_RenderWindow.draw(m_full_screen_fade);

    if(m_scrolling) {
        e->m_RenderWindow.draw(m_scrollShadow);
        e->m_RenderWindow.draw(m_scroll);
    }

}

void Action_Cutscene::doMove(float delta) {
    //std::cout << m_moveQueue.size() << std::endl;
    sf::Vector2f heroPos = e->getParty().getPos();
    sf::Vector2f dest = m_moveQueue.back();

    float m_distance = Tool::dist(heroPos, dest);
    float partySpeed = e->getParty().getCurrentPartySpeed(); // 150

    if(m_distance > 1) {
        e->getParty().recordMove((dest - heroPos) / m_distance * partySpeed * delta);
        if(m_animateWalk) {
            for(auto& m : e->getParty().getMembers()) {
                m.AnimateWalk(delta);
            }
        }
        if(m_camFollow) e->m_View.setCenter(heroPos);
    } else {
        m_moveQueue.pop_back();
    }
}

void Action_Cutscene::doStack(float delta) {
    e->getParty().recordMove(sf::Vector2f(0, 0));
    e->getParty().Update(delta);

    Hero& first = e->getParty().getMembers().front();
    Hero& last = e->getParty().getMembers().back();

    if(first.getSprite().getPosition().x == last.getSprite().getPosition().x && first.getSprite().getPosition().y == last.getSprite().getPosition().y) {
        m_stack = false;
    }
}

void Action_Cutscene::doFade(float delta) {
    float timeSpentFading   = e->getTimeRunning().getElapsedTime().asSeconds() - m_fadeStart.asSeconds();
    float fadeFraction      = timeSpentFading / m_fadeDuration;
    float fadeRange         = m_fadeDepthFinish * 255 - m_fadeDepthStart * 255;
    float alpha             = fadeFraction * fadeRange;

    if(alpha < 0) {
        alpha *= -1;
        alpha = 255 - alpha;
    }
    if(alpha > 255) alpha = 255;
    if(alpha < 0) alpha = 0;

    m_full_screen_fade.setFillColor(sf::Color(m_fadeColor.r, m_fadeColor.g, m_fadeColor.b, alpha));
}

void Action_Cutscene::doUnpause(float delta) {
    m_paused = false;
    m_stack = false;
}

void Action_Cutscene::doScrollText(float delta)
{
    if(!m_scrollTextLoaded) {
        m_scroll.setFont(m_Boxes.getFont(Box::FONT_WONDER));
        m_scroll.setCharacterSize(64);
        m_scroll.setPosition(sf::Vector2f(0, 1080));

        m_scrollShadow.setFont(m_Boxes.getFont(Box::FONT_WONDER));
        m_scrollShadow.setCharacterSize(64);
        m_scrollShadow.setPosition(sf::Vector2f(-4, 1080+4));
        m_scrollShadow.setColor(sf::Color::Black);

        std::stringstream ss;
        for(const auto& s : m_scrollText) {
            ss << std::string(5, ' ') << s << std::endl;
        }
        m_scroll.setString(ss.str());
        m_scrollShadow.setString(ss.str());
        m_scrollTextLoaded = true;
        //m_waiting = true;
    } else if (m_scroll.getPosition().y > -m_scroll.getLocalBounds().height) {

        float amount = (-SCROLL_SPEED + m_scrollSpeedMod) * delta;
        if(m_scroll.getPosition().y > 1080) {
            amount = -SCROLL_SPEED;
            m_scrollSpeedMod = 0;
        }
        m_scrollShadow.move(0, amount);
        m_scroll.move(0, amount);

    } else {
        m_scrollSpeedMod = 0;
        m_scrollText.clear();
        m_scrollText.clear();
        m_scrollTextLoaded = false;
        m_scrolling = false;
        //m_waiting = false;
        std::cout << "m_scrolling deactivated" << std::endl;
    }
}
