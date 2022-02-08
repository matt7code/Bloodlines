#include "states/save_load.h"

SaveLoad SaveLoad::m_SaveLoad;

void SaveLoad::Init(Engine* e) {
    number_of_saves = 0;
    m_GameLabels.clear();
    m_GameRecords.clear();
    m_loading = false;
    // Label the save slots properly into m_GameLabels
    for(int i = 0; i < 13; ++i) {
        std::string input;
        std::vector<std::string> record;
        std::stringstream ss;
        ss << i;
        std::string label;
        std::ifstream in("saves/savegame" + ss.str() + ".sav");
        if(!in) continue;
        number_of_saves++;
        while(std::getline(in, input)) {
            if(input[0] == '#') continue;
            record.push_back(input);
        }
        in.close();
        int seconds;
        ss.str("");
        ss.clear();
        ss << record[2];
        ss >> seconds;
        seconds /= 1000;
        m_GameLabels.push_back(record[1] + " " + Tool::pad(Tool::TimeToString(seconds), 10) + " " + Tool::Clip(record[4], 17));
        in.close();
        m_GameRecords.push_back(record);
    }

    m_SaveLoadMode = e->getSaveLoadMode();
    if(m_SaveLoadMode) m_takeScreenShot = true;

    m_Boxes.getBoxes().clear();

    m_Boxes.addBox("SaveBody", sf::Vector2f(1075, 675), sf::Vector2f(100, 200));

    std::stringstream ss;
    for(int i = 0; i < number_of_saves; ++i) {
        ss.str("");
        ss << i;
        m_Boxes.getBox("last").addText(m_GameLabels[i], sf::Vector2f(68, 7 + (i * 49)), 48);
    }

    m_Boxes.addBox("SaveLoad", sf::Vector2f(250, 64), sf::Vector2f(200, 130));
    std::string mode;
    m_SaveLoadMode ? mode = "Save" : mode = "Load";
    m_Boxes.getBox("last").addText(mode + " Game", sf::Vector2f(8, -4), 48);

    if(number_of_saves > 0) {
        m_Boxes.addBox("Screenie", sf::Vector2f(600, 350), sf::Vector2f(1220, 200));
        m_Boxes.addBox("ScreenieDesc", sf::Vector2f(600, 282), sf::Vector2f(1220, 595));
        LoadScreenie();
    } else {
        m_Boxes.getBox("SaveBody").addText("No Saved Games", sf::Vector2f(68, 7), 48);
    }
}

void SaveLoad::Cleanup(Engine* e) {
}

void SaveLoad::Pause(Engine* e) {
    m_View = e->m_RenderWindow.getView();
}

void SaveLoad::Resume(Engine* e) {
    e->m_RenderWindow.setView(m_View);
}

void SaveLoad::HandleEvents(Engine* e) {
    // Process events
    sf::Event event;
    while(e->m_RenderWindow.pollEvent(event)) {
        // Close window : exit
        if(event.type == sf::Event::Closed) {
            e->Quit();
        }
        if(event.type == sf::Event::KeyPressed) {
            switch(event.key.code) {
            case sf::Keyboard::Escape:
                m_Boxes.PlayCancel();
                e->PopState();
                break;


            case sf::Keyboard::Return:
                if(m_loading) {
                    return;
                } else {
                    m_loading = true;
                    m_Boxes.PlayConfirm();
                    SelectConfirm(m_currChoice, e);
                }
                break;

            case sf::Keyboard::Up:
            case sf::Keyboard::W:
                m_Boxes.PlaySelect();
                SelectUp();
                break;

            case sf::Keyboard::Down:
            case sf::Keyboard::S:
                m_Boxes.PlaySelect();
                SelectDown();
                break;

            default:
                break;
            }
        }
    }
}

void SaveLoad::Update(Engine* e) {
    if(!e->m_bCameFromMainMenu) {
        // Center the View
        e->m_View.setCenter(e->getParty().getPos());

        // Update the Level
        e->m_Level.Update(e->getTimeRunning(), e->getParty().getPos(), e->viewExtents);
    }
}

void SaveLoad::Draw(Engine* e) {
    if(!e->m_bCameFromMainMenu) {
        // Set the View
        e->m_RenderWindow.setView(e->m_View);

        // Draw the Level
        e->m_Level.Draw(e->m_RenderWindow);

        // Draw the Party
        e->getParty().Draw(e->m_RenderWindow);
    }

    if(m_takeScreenShot) {
        //sf::RenderTexture temp;
        //temp.create(1920/4, 1080/4);
        m_screenShotTex.create(e->m_RenderWindow.getSize().x, e->m_RenderWindow.getSize().y);
        m_screenShotTex.update(e->m_RenderWindow);
        m_takeScreenShot = false;
    }
    // Reset the View
    e->m_RenderWindow.setView(e->m_ViewBase);

    // Draw the HUD
    m_Boxes.DrawBoxes(e->m_RenderWindow);

    // Draw Arrow
    m_Boxes.drawArrow(sf::Vector2f(147.0f, 255.0f + (49 * m_currChoice)), 2, sf::Vector2f(1.0f, 1.0f), e->m_RenderWindow);

    // Draw Screenshot
    if(m_currChoice < number_of_saves) e->m_RenderWindow.draw(m_screenShotSprite);
}

void SaveLoad::SelectConfirm(int choice, Engine* e) {
    std::stringstream ss;
    ss << choice;
    if(m_SaveLoadMode) {
        SaveScreenShot(choice, e);
        Save(choice, e);
    } else {
        if(number_of_saves > 0) Load(choice, e);
    }
    if(!e->m_bCameFromMainMenu) e->PopState();
}

void SaveLoad::SelectUp() {
    m_currChoice--;
    if(m_currChoice < 0) m_currChoice = number_of_saves - 1;
    LoadScreenie();
}

void SaveLoad::SelectDown() {
    m_currChoice++;
    if(m_currChoice > 12) m_currChoice = 0;

    if(!m_SaveLoadMode) {
        if(m_currChoice > number_of_saves - 1) m_currChoice = 0;
    } else {
        if(m_currChoice > number_of_saves) m_currChoice = 0;
    }
    if(m_currChoice < number_of_saves) LoadScreenie();
    //else UnLoadScreenie();
}

void SaveLoad::Load(int slot, Engine* e) { /**< Load a savegame from disk. */
    // Announce
    std::stringstream ss;
    ss << slot;

    // File
    std::ifstream in("saves/savegame" + ss.str() + ".sav");
    if(!in) {
        std::cerr << "File Error opening saves/savegame" + ss.str() + ".sav for input." << std::endl;
        return;
    }

    e->getParty().getMembers().clear();

    // Read
    std::string input;
    std::vector<std::string> record;
    while(std::getline(in, input)) {
        if(input[0] == '#' || input.empty()) continue;
        record.push_back(input);
    }
    in.close();

    // Process
    for(auto it = record.begin(); it < record.end(); ++it) {
        if(*it == "[HEADER]") {
            ++it; // Skip 'Time & Date' label

            ss.str("");
            ss.clear();
            long time_played;
            int steps, gold;
            ss.str(*(++it));
            ss >> time_played >> steps >> gold;
            e->getTimeRunning().restart();
            e->setTimePlaying(time_played);
            e->getParty().setPartySteps(steps);
            e->getParty().setPartyGold(gold);
        } else if(*it == "[PARTY]") {
            ++it; // Skip 'Location Name' label

            ss.str("");
            ss.clear();
            float x, y;
            int z;
            ss.str(*(++it));
            ss >> x >> y >> z;
            sf::Vector2f PartyPosition = sf::Vector2f(x, y);
            e->m_Level.z_Level = z;

            ss.str("");
            ss.clear();
            int partySize;
            ss.str(*(++it));
            ss >> partySize;

            for(int i = 0; i < partySize; ++i) {
                // Skip party position number
                ++it;

                std::string name;
                name = *(++it);

                ss.str("");
                ss.clear();
                float exp;
                ss.str(*(++it));
                ss >> exp;

                ss.str("");
                ss.clear();
                int level, clas, facing, avatar;
                ss.str(*(++it));
                ss >> level >> clas >> facing >> avatar;

                e->getParty().AddMember(avatar, level, clas, name);

                ss.str("");
                ss.clear();
                float chp, cmp;
                ss.str(*(++it));
                ss >> chp >> cmp;

                e->getParty().getMembers().back().setName(name);
                e->getParty().getMembers().back().setExp(exp);
                e->getParty().getMembers().back().setLevel(level);
                e->getParty().getMembers().back().setFacing(facing);
                e->getParty().getMembers().back().setAvatar(avatar);
                e->getParty().getMembers().back().setClass(clas);
                e->getParty().getMembers().back().setCurHP(chp);
                e->getParty().getMembers().back().setCurMP(cmp);

                // Process Spells
                ss.str("");
                ss.clear();
                ss.str(*(++it));
                int spell_number;
                e->getParty().getMembers().back().getSpells().clear();
                while(ss >> spell_number) {
                    e->getParty().getMembers().back().getSpells().push_back(spell_number);
                }

                // Process Inventory
                ss.clear();
                ss.str(*(++it));
                int item_number, item_qua;
                e->getParty().getMembers().back().getInventory().clear();
                while(ss >> item_number >> item_qua) {
                    if(item_qua == 0) continue;
                    CarriedItem newItem;
                    newItem.type = item_number;
                    newItem.amount = item_qua;
                    e->getParty().getMembers().back().getInventory().push_back(newItem);
                }
                // Process Equipped
                ss.clear();
                ss.str(*(++it));
                //std::cout << "Loaded Equipped: " << ss.str() << std::endl;
                int item_type;
                e->getParty().getMembers().back().getEquipment()[0] = 0;
                for(int i = 1; i < 11; ++i) {
                    ss >> item_type;
                    if(item_type == 0) continue;
                    e->getParty().getMembers().back().getEquipment()[i] = item_type;
                }
            }
            e->getParty().MovePartyTo(PartyPosition);

        } else if(*it == "[KILLS]") {
//            e->getParty().getKills().clear();
//            while(*(++it) != "[END]") {
//            }
        } else if(*it == "[QUESTS]") {

            e->getParty().getQuests().clear();
            while(*(++it) != "[END]") {
                std::size_t pos;
                pos = it->find("==");
                if(pos == std::string::npos) {
                    std::cerr << "Malformed Quest Value in Save File: " << *it << std::endl;
                } else {
                    // Process quest line
                    std::string saved_value = *it;
                    Quest temp;
                    temp.m_tag          = saved_value.substr(0, pos);
                    saved_value.erase(0, pos + 2);
                    std::stringstream ss;
                    ss << saved_value[0] << " " << saved_value[1] << " " << saved_value[2];
                    ss >> temp.m_part >> temp.m_display >> temp.m_completed;
                    saved_value.erase(0, 3);
                    size_t colon = saved_value.find(':');
                    temp.m_name  = saved_value.substr(0, colon);
                    saved_value.erase(0, colon + 1);
                    temp.m_desc  = saved_value;
                    e->getParty().getQuests().push_back(temp);
//
//                    std::cout << "QuestLoaded: " << std::endl;
//                    std::cout << "\tTag      : " << temp.m_tag << std::endl;
//                    std::cout << "\tName     : " << temp.m_name << std::endl;
//                    std::cout << "\tPart     : " << temp.m_part << std::endl;
//                    std::cout << "\tDisplayed: " << temp.m_display << std::endl;
//                    std::cout << "\tCompleted: " << temp.m_completed << std::endl;
//                    std::cout << "\tDesc     : " << temp.m_desc << std::endl;
                }
            }
        } else if(*it == "[SAVED_VALUES]") {
            e->getParty().getSavedValues().clear();
            ss.clear();
            ss.str(std::string());
            while(*(++it) != "[END]") {
                //std::cout << "[Quests] " << *it << std::endl;
                std::string temp = *it;
                std::size_t pos;
                pos = temp.find("==");
                if(pos == std::string::npos) {
                    std::cerr << "Malformed Saved Value in Save File: " << *it << std::endl;
                } else {
                    e->getParty().setSavedValue(temp.substr(0, pos), temp.substr(pos + 2));
                }
            }
        } else if(*it == "[CHESTS]") {
            e->getParty().getChests().clear();
            ss.clear();
            ss.str(std::string());
            while(*(++it) != "[END]") {
                std::cout << "[Chests] " << *it << std::endl;
                std::stringstream chests_opened;
                chests_opened.str(*it);
                int chest;
                while(chests_opened >> chest) {  // Record opened chests
                    std::cout << "Chest #" << chest << " empty." << std::endl;
                    e->getParty().getChests().push_back(chest);
                }
            }
        }
    }

    e->setLastLoadMilliseconds(e->getTimeRunning().getElapsedTime().asMilliseconds());
    e->m_Level.resetTimerNPC(e->getTimeRunning());

    if(e->m_bCameFromMainMenu) {
        e->ChangeState(Play::Instance());
    } else {
        e->m_Level.LoadNpcs(e->m_Level.getName()); /// Wipes out previous NPC vector...
        e->PopState();
        e->PopState();
    }
}

void SaveLoad::Save(int slot, Engine* e) { /**< Write the savegame to disk. */
    //std::cout << "Saving: " << slot << std::endl;
    std::stringstream ss;
    ss << slot;
    std::ofstream of("saves/savegame" + ss.str() + ".sav");
    if(!of) {
        std::cerr << "File Error opening saves/savegame" + ss.str() + ".sav for output." << std::endl;
        return;
    }
    of << "[HEADER]" << std::endl;
    of << "# Date and Time of Save" << std::endl;
    of << Tool::GetCurrentDateAndTime() << std::endl;
    of << "# Time Running (ms) - Total Steps - Gold" << std::endl;
    of << e->getTimePlaying() << " " << e->getParty().getPartySteps() << " " << e->getParty().getPartyGold() << std::endl;
    of << "[PARTY]" << std::endl;
    of << "# Current Location Name:" << std::endl;
    of << e->m_Level.getPlace(e->getParty().getPos()) << std::endl;
    of << "# Location:" << std::endl;
    of << e->getParty().getPos().x << " " << e->getParty().getPos().y << " " << e->m_Level.z_Level << std::endl;
    of << "# Party Size:" << std::endl;
    of << e->getParty().getMembers().size() << std::endl;
    of << "# [END]" << std::endl;

    int n = 0;
    for(auto& m : e->getParty().getMembers()) {
        of << "# [Party Member]" << std::endl << n++ << std::endl;
        of << "# Name" << std::endl << m.getName() << std::endl;
        of << "# Experience" << std::endl << m.getExp() << std::endl;
        of << "# Level Class Facing Avatar" << std::endl << m.getLevel() << " " << m.getClass() << " " << m.getFacing() << " " << m.getAvatar() << std::endl;
        of << "# Current Health and Magic" << std::endl << m.getCurHP() << " " << m.getCurMP() << std::endl;
        of << "# Spells" << std::endl;
        for(auto n : m.getSpells()) of << n << " ";
        of << std::endl;
        of << "# Inventory" << std::endl;
        for(auto n : m.getInventory()) of << n.type << " " << n.amount << " ";
        if(m.getInventory().empty()) of << "0 0";
        of << std::endl << "# Equipped" << std::endl;
        for(int i = 1; i < 11; ++i) of << m.getItemInSlot(i) << " ";
        of << std::endl;
        of << "# [END]" << std::endl;
    }
    if(!e->getParty().getChests().empty()) {
        of << "[CHESTS]" << std::endl;
        for(const auto& chest : e->getParty().getChests()) of << chest << " ";
        of << std::endl << "[END]" << std::endl;
    }
    if(!e->getParty().getSavedValues().empty()) {
        of << "[SAVED_VALUES]" << std::endl;
        for(const auto& it : e->getParty().getSavedValues()) {
            of << it.first << "==" << it.second << std::endl;
        }
        of << "[END]" << std::endl;
    }
/// ///////////////////////////////////////////////////////////////////
    if(!e->getParty().getQuests().empty()) {
        //std::map<std::string, std::string> questLines;
        //APPLE1==111Apple Man:Find an apple.
        //APPLE2==201Apple Man:Eat an apple.
        //APPLE3==301Apple Man:You found and ate an apple.
        //CRYPT4==111Trouble in the Crypt:There's something odd going on in the Woodhome town Crypt.
        //ZOMBIE_KILLER5==111Zombie Killer:Kill 10 zombies in the barrack's basement and report to Sheriff Roberts.
        //CRYPT6==101Trouble in the Crypt:There's something odd going on in the Woodhome town Crypt.
        //ZOMBIE_KILLER7==601Zombie Killer:You killed 10 zombies in the barrack's basement and Sheriff Roberts was very grateful.

        of << "[QUESTS]" << std::endl;
        of << "# Quest Format: Tag==Part/Display/Completed/Name:Description" << std::endl;
        for(auto q : e->getParty().getQuests()) {
            of << q.m_tag << "==" << q.m_part << q.m_display << q.m_completed << q.m_name << ":" << q.m_desc << std::endl;
        }
        of << "[END]" << std::endl;
    }
    of.close();
    if(!e->m_bCameFromMainMenu) {
        e->PopState();
        e->PopState();
    }
}

void SaveLoad::SaveScreenShot(int slot, Engine* e) {
    m_screenShotDisk = m_screenShotTex.copyToImage();
    // Write it to a .img file in the saves/ dir
    std::stringstream ss;
    ss << slot;
    m_screenShotDisk.saveToFile("saves/savegame" + ss.str() + ".png");
}

void SaveLoad::LoadScreenie() {
    std::stringstream ss;
    ss << m_currChoice;
//    std::cout << "saves/savegame" + ss.str() + ".png" << std::endl;
    if(!m_currGameTex.loadFromFile("saves/savegame" + ss.str() + ".png")) return;
    m_screenShotSprite.setTexture(m_currGameTex);
    m_screenShotSprite.setScale(sf::Vector2f(0.33f, 0.33f));
    m_screenShotSprite.setPosition(1251, 228);

    m_Boxes.getBox("ScreenieDesc").getText().clear();

    std::vector<std::string>& member = m_GameRecords[m_currChoice];

    ss.clear();
    ss.str("");
    ss << member[6];
    int partySize = 0;
    ss >> partySize;
    for(int i = 0; i < partySize; ++i) {
        // "NameNameNameNa Level 01 Wizard\n"
        const int LINES_PER_CHARACTER_RECORD = 8;
        std::string lable;
        lable = Tool::pad(member[(i * LINES_PER_CHARACTER_RECORD) + 8], 14);
        lable += " Level ";
        ss.clear();
        ss.str("");
        ss << member[(i * LINES_PER_CHARACTER_RECORD)+10];
        int level, clas;
        ss >> level >> clas;
        if(level < 10) lable += '0';
        ss.clear();
        ss.str("");
        ss << level;
        lable += ss.str();
        lable += ' ';
        lable += Hero::getClassName(clas);

        m_Boxes.getBox("ScreenieDesc").addText(lable, sf::Vector2f(18, -4 + (i * 37)), 36);
    }
}
