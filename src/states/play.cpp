#include "states/play.h"

Play Play::m_PlayState;

void Play::Init(Engine* eng) {
    e = eng;
    LoadShader();
    e->m_View.zoom(0.5f);
    e->m_Level.Load("book1");
    e->stopMusic();
    e->playMusic(Engine::SONG_WORLD);
    e->StartInput();
}

void Play::Resume(Engine* e) {
    m_prevTime = e->getTimeRunning().getElapsedTime();
    //e->setCurrentScript("");
    //e->setCurrentScriptCombat("");
}

void Play::HandleEvents(Engine* e) {
    sf::Event event;
    while (e->m_RenderWindow.pollEvent(event)) {
        if (event.type == sf::Event::Closed) e->Quit();

        if (event.type == sf::Event::KeyPressed) {
            switch(event.key.code) {
            case sf::Keyboard::Return:
                DoTalkAndSearch(e);
                break;

            case sf::Keyboard::Escape:
                m_Boxes.PlayConfirm();
                if(m_Boxes.hasBox("Search")) {
                    m_Boxes.removeBox("Search");
                } else {
                    e->PushState( MiniMenu::Instance() );
                }
                break;

            case sf::Keyboard::F1:
                m_Boxes.PlayConfirm();
                //LOG("EditMenu");
                e->PushState( MiniMenu_Edit::Instance() );
                break;

            /// Restricted ///
            case sf::Keyboard::Space:
                std::cout << "Safe? " << (e->getParty().safe ? "True" : "False") << std::endl;
                break;

            case sf::Keyboard::H:
                for (auto& p : e->getParty().getMembers()) {
                    p.setCurHP(p.getMaxHP());
                    p.setCurMP(p.getMaxMP());
                }
                break;
            case sf::Keyboard::M:
                std::cout << e->getParty().getPartyGold() << " gold.";
                e->getParty().modPartyGold(-100);
                break;

            /// Restricted ///
            case sf::Keyboard::Q:
                e->Quit();
                break;

            /// Restricted ///
//            case sf::Keyboard::M:
//                for (const auto& pos : e->getParty().getMoveRecord())
//                {
//                    std::cout << "(mr) " << pos.x << "/" << pos.y << std::endl;
//                }
//                break;

            default:
                break;
            }
        }
    }
}

void Play::Update(Engine* e) {
    float delta;

    using kb = sf::Keyboard;
//    if (e->getPauseMode() == PAUSE_MODE_NONE)
//    {
    delta = e->getTimeRunning().getElapsedTime().asSeconds() - m_prevTime.asSeconds();
    m_prevTime = e->getTimeRunning().getElapsedTime();

    if(kb::isKeyPressed(kb::Up) || kb::isKeyPressed(kb::W))         Move(sf::Vector2f(0.0f, -delta * e->getParty().getCurrentPartySpeed()));
    else if(kb::isKeyPressed(kb::Down) || kb::isKeyPressed(kb::S))  Move(sf::Vector2f(0.0f, +delta * e->getParty().getCurrentPartySpeed()));
    else if(kb::isKeyPressed(kb::Left)  || kb::isKeyPressed(kb::A)) Move(sf::Vector2f(-delta * e->getParty().getCurrentPartySpeed(), 0.0f));
    else if(kb::isKeyPressed(kb::Right) || kb::isKeyPressed(kb::D)) Move(sf::Vector2f(+delta * e->getParty().getCurrentPartySpeed(), 0.0f));

    e->getParty().Update(delta);
    e->m_View.setCenter(e->getParty().getPos());

    /// Every STEP
    if (e->getParty().getPartySteps() != m_PartyCurrentSteps) {
        // Remove Search Dialog
        if(m_Boxes.hasBox("Search")) m_Boxes.removeBox("Search");

        // Check Triggers
        m_inTrigger = ProcessTriggers();

        // Check Combat
        if(!e->getParty().safe) {
            Beacon* bp = e->m_Level.getNearestCombatBeacon(sf::Vector3i(e->getParty().getPos().x, e->getParty().getPos().y, e->m_Level.z_Level));
            if (bp && (Tool::getRand(1, 1000) < bp->getValue2()) && (e->getTimeRunning().getElapsedTime().asSeconds() > 1.0f)) e->PushState( Combat::Instance() );
        }

        // Update Steps
        m_PartyCurrentSteps = e->getParty().getPartySteps();
    }

    e->m_Level.Update(e->getTimeRunning(), e->getParty().getPos(), e->viewExtents);
    /// Experimental
//    for(auto& npc : e->m_Level.Npcs) {
//        if (Tool::dist(npc.sprite.getPosition(), e->m_View.getCenter()) < CLIP_DISTANCE_NPCS) npc.Update(delta);
//    }
}

void Play::Move(sf::Vector2f mv) {
    if (e->getMillisecondsSinceLoad() < 200) return;
    sf::Vector2f target = e->getParty().getPos(0) + mv;
    e->getParty().getMember(0).SetFacingByVector(mv);

    // Check Collision
    if (!e->m_Level.getZone(target).isPassable(target)) {
        mv.x = mv.y = 0;
        switch (e->m_Level.getZone(target).getTileTypeAtPos(target)) {
        case 220: // Closed Dark Wooden Double Door, Open 98
            DoClosedDoor(target, 98);
            break;
        case 222: // Closed Metal Door, Open: 101
            DoClosedDoor(target, 101);
            break;
        case 221: // Closed Wood Door, Open: 423
            DoClosedDoor(target, 423);
            break;
        case 248: // Closed Wood Door, Open: 423
            DoClosedDoor(target, 423);
            break;
        case 223: // Closed Stone Door, Open: 101
            DoClosedDoor(target, 101);
            break;
        case 219: // Closed Stone Door, Open: 97
            DoClosedDoor(target, 97);
            break;

        case 102: // Stone Stairs Up
        case 103:
        case 104:
        case 105:
        case 106:
        case 107:
        case 247: // Wood Stairs Up
        case 271:
            e->setCurrentScript("StairsUp");
            e->PushState(Action_Cutscene::Instance());
            break;
        case 108: // Stone Stairs Down
        case 109:
        case 110:
        case 246: // Wood Stairs Down
        case 270:
            e->setCurrentScript("StairsDown");
            e->PushState(Action_Cutscene::Instance());
            break;
        default:
            break;
        }
    }
    e->getParty().recordMove(mv);
}

void Play::Draw(Engine* e) {
    //if(!m_inTrigger) {
    //std::cout << "Draw: Play" << std::endl;
    e->m_RenderWindow.setView(e->m_View);
    //e->m_Level.Draw(e->m_RenderWindow);
    //e->m_RenderWindow.draw(m_shader);
//    if(m_shader.isAvailable())
//        m_shader.setParameter("texture", sf::Shader::CurrentTexture);
//        e->m_Level.Draw(e->m_RenderWindow, m_shader);
//    } else {
//        e->m_Level.Draw(e->m_RenderWindow);
//    }

    e->m_Level.Draw(e->m_RenderWindow);
    e->getParty().Draw(e->m_RenderWindow);
    e->m_RenderWindow.setView(e->m_ViewBase);
    m_Boxes.DrawBoxes(e->m_RenderWindow);
//    } else {
//        m_inTrigger = false;
//    }
}

void Play::DoTalkAndSearch(Engine* e) {
    sf::Vector2f partyPos = sf::Vector2f(e->getParty().getPos().x, e->getParty().getPos().y - 14);

    for (auto& npc : e->m_Level.Npcs) {

        if(npc.m_zLevel == e->m_Level.z_Level && Tool::dist(npc.sprite.getPosition(), partyPos) < NPC_TALK_DISTANCE && !e->m_conversationTarget) {
            e->m_conversationTarget = &npc;
            if (e->m_prevConversationTarget == e->m_conversationTarget) {
                e->annoyedNPC++;
            } else {
                e->annoyedNPC = 0;
                e->m_prevConversationTarget = e->m_conversationTarget;
            }
        } else {
            DoSearch(e);
        }
    }
    if(e->m_conversationTarget) {
        //e->getParty().Freeze(false);
        e->m_prevFacing = e->m_conversationTarget->facing;
        e->m_prevMoveMode = e->m_conversationTarget->moveMode;
        // Face each other
        e->m_conversationTarget->moveMode = Npc::MOVE_STATIONARY;
        e->m_conversationTarget->facing = Tool::face(e->m_conversationTarget->sprite.getPosition(), partyPos);
        e->getParty().getMember(0).setFacing( Tool::face(partyPos, e->m_conversationTarget->sprite.getPosition()) );
        e->getParty().getMember(0).UpdateAppearance();
        e->m_conversationTarget->UpdateAppearance();
        // Push Conversation State
        e->PushState( Dialog::Instance() );
    } else {
        std::cout << "Who are you talking too?" << std::endl;
    }
//    }
}

void Play::DoSearch(Engine* e) {
    if(m_Boxes.hasBox("Search")) {
        m_Boxes.removeBox("Search");
        return;
    }
    Beacon* beacon = e->m_Level.getNearestSearchBeacon( sf::Vector3i(e->getParty().getPos().x, e->getParty().getPos().y, e->m_Level.z_Level) );
    if(!beacon) {
        m_Boxes.addBox("Search", sf::Vector2f(500, 64), sf::Vector2f(960 - 250, 600));
        m_Boxes.getBox("Search").addText("Your search reveals nothing.", sf::Vector2f(5, 5), 36);
        return;
    } else { /// Found Beacon
        if (beacon->getValue2() == -1 && std::find(e->getParty().getChests().begin(), e->getParty().getChests().end(), beacon->getValue3()) == e->getParty().getChests().end()) { /// It's a chest.
            /// Award Items
            std::stringstream ss;
            std::stringstream awards;
            ss.str(beacon->getText2());
            int chest;
            int n = 0;
            while (ss >> chest) {
                awards << std::endl << "\tReceived: " << Tool::Clip(e->Items[chest].name, 45);
                e->getParty().getMember(0).invGive(chest, 1);
                ++n;
            }
            // Notify of items found
            int len = 60 * 18;
            m_Boxes.addBox("Search", sf::Vector2f(len, n * 38 + 64), sf::Vector2f(960 - (len / 2), 600));
            m_Boxes.getBox("Search").addText(beacon->getText1() + awards.str(), sf::Vector2f(3, 5), 36);
            e->getParty().getChests().push_back(beacon->getValue3()); /// Record it as looted.
        } else { /// Not a chest.
            int len = beacon->getText1().size() * 18;
            m_Boxes.addBox("Search", sf::Vector2f(len, 64), sf::Vector2f(960 - (len / 2), 600));
            m_Boxes.getBox("Search").addText(beacon->getText1(), sf::Vector2f(3, 5), 36);
        }
    }
}

/*! \fn bool CheckTriggers();
 *  \brief Checks to see if the party has entered or left a trigger area, and if so - activates an associated script.
 */
bool Play::ProcessTriggers() {
    bool ret = false;
    //std::cout << "Processing Triggers" << std::endl;
    std::vector<Trigger> NewTriggerList;
    std::vector<Trigger>& OldTriggerList = e->m_Level.CurrentTriggerList;
    sf::Vector2f partyPos = e->getParty().getPos();
    // Determine what triggers we are in, if none, exit
    for (auto& t : e->m_Level.getAllTriggers()) {
        if(t.ZLevel != e->m_Level.z_Level) continue;
        if(t.Area.contains(partyPos)) {
            // Collect all triggers we're in
            NewTriggerList.push_back(t);
        }
    }
    // Compare it to an old list
    ///e->m_Level.CurrentTriggerList

    for (auto& t : NewTriggerList) {
        // If a new tag is found in the old list, fire it's Step_Script
        bool found = false;
        for(auto &s : OldTriggerList) {
            if (t.Tag == s.Tag) found = true;
        }
        if (found) {
            //std::cout << "Play::ProcessTriggers() - Step Trigger" << std::endl;
            RunTriggerScript(t, STEP);
        } else {
            //std::cout << "Play::ProcessTriggers() - Entry Trigger" << std::endl;
            RunTriggerScript(t, ENTRY);
        }
    }

    for (auto& t : OldTriggerList) {
        // If an old tag is not found in the new list, fire it's Exit_Script
        bool found = false;
        for(auto &s : NewTriggerList) {
            if (t.Tag == s.Tag) found = true;
        }
        if (!found) {
            //std::cout << "Play::ProcessTriggers() - Exit Trigger" << std::endl;
            RunTriggerScript(t, EXIT);
        }
    }
    // Set OldList to NewList
    OldTriggerList = NewTriggerList;
    //std::cout << "Old Trigger List Size: " << OldTriggerList.size() << std::endl;
    return ret;
}

void Play::DoClosedDoor(sf::Vector2f& targetLocation, int openDoorTile)
{
    if (partyCanPassDoor(targetLocation)) {
        e->m_Level.PlayDoor(1.0 + (-0.5 + (0.1 * Tool::getRand(5, 10))));
        e->m_Level.getZone(targetLocation).setTileTypeAtPos(targetLocation, openDoorTile);
    } else {
        e->m_Level.PlayJiggle(1.0 + (-0.5 + (0.1 * Tool::getRand(5, 10))));
    }
}

bool Play::partyCanPassDoor(sf::Vector2f target)
{
    bool passable = true;
    //Is it locked?
    if(e->m_Level.getZone(target).getTile(target).getIsLocked()) {
        if(e->getParty().getMember(0).getInventoryAmount(e->m_Level.getZone(target).getTile(target).getKey()) == 0) passable = false;
    }
    return passable;
}

void Play::RunTriggerScript(Trigger& trigger, int part)
{
    //std::cout << "Play::RunTriggerScript" << std::endl;
    std::string fn = e->m_Level.getName() + "/cutscenes/" + trigger.Tag;
    std::string tag = trigger.Tag;
    if(part == ENTRY) {
        fn += "_Entry.scr";
        tag += "_Entry";
    }
    else if(part == STEP) {
        fn += "_Step.scr";
        tag += "_Step";
    }
    else if(part == EXIT) {
        fn += "_Exit.scr";
        tag += "_Exit";
    }
    else return;


    std::ifstream in(fn);
    if (!in) {
        std::cout << "Missing Script: " << fn << std::endl;
    } else {
        in.close();
        std::cout << "Found Script: " << fn << std::endl;
        e->setCurrentScript(tag);
        e->PushState(Action_Cutscene::Instance());
    }
}

// load both shaders
void Play::LoadShader() {
    if (!m_shader.loadFromFile("shaders/vertex_shader.vert", "shaders/fragment_shader.frag"))
    {
        std::cout << "Failed to load Shader." << std::endl;
    }
}
