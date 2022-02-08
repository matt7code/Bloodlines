#include "states/dialog.h"

/*
# Commands:
# NPC <TAG>
# PUT <npc-dialog>
# SAY <pc-dialog>
# QUEST_SET <key-to-set> <value-to-set-it-to>
# QUEST_REMOVE <key-to-remove>
# COND TRUE
# COND FALSE
# COND HAS_GREATER_THAN_ITEM_AMOUNT <item-to-check> <amount-checked>
# COND HAS_LESS_THAN_ITEM_AMOUNT <item-to-check> <amount-checked>
# COND QUEST_HAS <key-to-retrieve> <value-to-compare-it-to>
# COND QUEST_LACKS <key-to-retrieve> <value-to-compare-it-to>
# COND HAS_GREATER_THAN_GOLD_AMOUNT <amount>
# COND HAS_LESS_THAN_GOLD_AMOUNT <amount>
# COND_END
# CHOICE START // Begin a Choice Block
# CHOICE <choice-text>
# CHOICE END // End a Choice Block
# !<bookmark-name>
# GOTO <bookmark>
# GOTO <line>
# GOTO START
# GOTO END
# RAND <potential-dialog>
# END
# PAUSE
# LIST_QUEST
# ITEM_GIVE <item-id> <amount>
# ITEM_TAKE <item-id> <amount>
# ITEM_TAKE_ALL <TRUE if euqipped included>
# GOLD_GIVE <amount>
# GOLD_TAKE <amount>
# ACTION SLEEP
*/

Dialog Dialog::m_Dialog;

void Dialog::Init(Engine* eng) {
    e = eng; /* Sorta new technique, duplicate the engine pointer locally. */
    //e->setPauseMode(PAUSE_MODE_DIALOG);

    m_Boxes.getBoxes().clear();
    m_Boxes.addBox("Convo", sf::Vector2f(BOX_CONVO_WIDTH, BOX_CONVO_HEIGHT), sf::Vector2f(BOX_CONVO_LEFT, BOX_CONVO_TOP), Box::BOX_CONVO);

    NpcPtr  = e->m_conversationTarget;
    HeroPtr = &e->getParty().getMember(0);

    if(e->m_Level.getFaceIndexByAvatar(HeroPtr->getAvatar()) == -1) {
        m_pcFace = &e->m_Level.getFaceByAvatar(0);
    } else {
        m_pcFace = &e->m_Level.getFaceByAvatar(HeroPtr->getAvatar());
    }
    if(e->m_Level.getFaceIndexByAvatar(NpcPtr->avatar) != -1) m_npcFace = &e->m_Level.getFaceByAvatar(NpcPtr->avatar);
    else m_npcFace = m_pcFace;

    setCurrentLine(0);
    m_mode      = DIALOG_MODE_READ;
    m_paused    = false;
    ProcessDialog();
}

void Dialog::ProcessLine() {
    std::string command = getCommand();
    vecStr line = getLine();

    if(e->getAnnoyed() >= ANNOY_THRESHHOLD && !SayAnnoy.empty()) { // the 16th time you chat
        m_currAnnoyLine = e->getAnnoyed() - ANNOY_THRESHHOLD;
        m_currAnnoyLine %= SayAnnoy.size();
        Tool::FormatLineIntoVector(SayAnnoy[m_currAnnoyLine], 55, displayBlock);
        DisplayCurrentBlock();
    } else if (command == "COND_END") {
    } else if (command == "STORE") {
        doStore(line);
    } else if (command == "ACTION") {
        doAction(line);
    } else if (command == "ITEM_TAKE_ALL") {
        if(line.size() > 1 && line[1] == "TRUE") HeroPtr->getEquipment() = {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0};
        HeroPtr->getInventory().clear();
    } else if (command == "COND") {
        doCondition(line);
    } else if (command == "QUEST_SET") {
        e->getParty().setSavedValue(line[0], line[1]);
    } else if (command == "CHOICE") {
        m_currFace = m_pcFace;
        doChoice(line);
    } else if (command == "GOTO") {
        doGoto(line[0]);
    } else if (command == "ITEM_GIVE") {
        std::stringstream ss;
        int item, amount;
        ss << line[0] << " " << line[1];
        ss >> item >> amount;
        e->getParty().getMember(0).invGive(item, amount);
    } else if (command == "ITEM_TAKE") {
        std::stringstream ss;
        int item, amount;
        ss << line[0] << " " << line[1];
        ss >> item >> amount;
        e->getParty().getMember(0).invTake(item, amount);
    } else if (command == "PAUSE") {
        m_paused = true;
    } else if (command == "LIST_QUEST") {
        e->getParty().debugSavedValues();
    } else if (command == "QUEST_REMOVE") {
        e->getParty().deleteQuestValue(line[0]);
    } else if (command == "PUT") {
        m_currFace = m_npcFace;
        m_cdl = 0;
        Tool::FormatLineIntoVector(Tool::reconstituteString(line), 55, displayBlock);
        DisplayCurrentBlock();
    } else if (command == "SAY") {
        m_currFace = m_pcFace;
        m_cdl = 0;
        Tool::FormatLineIntoVector(Tool::reconstituteString(line), 55, displayBlock);
        DisplayCurrentBlock();
    } else if (command == "RAND") {
        std::string output = Tool::reconstituteString(line);
        SayRandom.push_back(output);
        if(peekCommand(1) != "RAND") {
            m_currFace = m_npcFace;
            output = SayRandom.at(Tool::getRand(0, SayRandom.size() - 1));
            Tool::FormatLineIntoVector(output, 55, displayBlock);
            DisplayCurrentBlock();
            SayRandom.clear();
        }
    } else if (command == "GOLD_GIVE") {
        std::stringstream ss;
        ss << line[1];
        int amount;
        ss >> amount;
        e->getParty().modPartyGold(amount);
    } else if (command == "GOLD_TAKE") {
        std::stringstream ss;
        ss << line[0];
        int amount;
        ss >> amount;
        e->getParty().modPartyGold(-amount);
        if(e->getParty().getPartyGold() < 0) e->getParty().setPartyGold(0);
    } else if (command == "END") {
        e->PopState();
    } else if (command == "STATS") {
        PrintStats();
    } else {
        std::cerr << "CRITICAL ERROR: Bad Command in Dialog \"" << NpcPtr->tag << "\"." << std::endl;
        std::cerr << "NPC Name: " << NpcPtr->name << std::endl;
        std::cerr << " Command: " << command << std::endl;
        std::cerr << "      CL: " << getCurrentLine() << std::endl;
    }
    advanceCurrentLine();
}

void Dialog::ConfirmChoice() {
    m_Boxes.PlayConfirm();
    m_mode = DIALOG_MODE_READ;
    doGoto(choiceLineRefs[m_choice + m_cdl + 1] + 1);
    m_paused = false;
}

void Dialog::Cleanup(Engine* e) {
    m_currAnnoyLine = 0;
    m_cdl           = 0;
    m_paused        = true;

    choiceLineRefs.clear();
    displayBlock.clear();
    SayRandom.clear();
    SayAnnoy.clear();
    dialog.clear();

    //e->setPauseMode(PAUSE_MODE_NONE);
    //e->getParty().Unfreeze(e->getTimeRunning().getElapsedTime());
    e->m_conversationTarget->facing             = e->m_prevFacing;
    e->m_conversationTarget->moveMode           = e->m_prevMoveMode;
    e->m_conversationTarget->UpdateAppearance();
    e->m_conversationTarget                     = nullptr;
}

void Dialog::HandleEvents(Engine* e) {
    sf::Event event;
    while (e->m_RenderWindow.pollEvent(event)) {
        if (event.type == sf::Event::Closed) e->Quit();
        if (event.type == sf::Event::KeyPressed) {
            switch(event.key.code) {
            case sf::Keyboard::Escape:
                m_Boxes.PlayCancel();
                e->PopState();
                break;

            case sf::Keyboard::Return:
            case sf::Keyboard::Space:
                if(m_mode == DIALOG_MODE_READ) {
                    m_paused = false;
                } else if (m_mode == DIALOG_MODE_CHOICE) {
                    ConfirmChoice();
                }
                break;

            case sf::Keyboard::W:
            case sf::Keyboard::Up:
                if(m_mode == DIALOG_MODE_READ) RewindConvo();
                else if (m_mode == DIALOG_MODE_CHOICE) PrevChoice();
                DisplayCurrentBlock();
                break;

            case sf::Keyboard::S:
            case sf::Keyboard::Down:
                if(m_mode == DIALOG_MODE_READ) AdvanceConvo();
                else if (m_mode == DIALOG_MODE_CHOICE) NextChoice();
                DisplayCurrentBlock();
                break;

            default:
                break;
            }
        }
    }
}

void Dialog::Update(Engine* e) {
    e->m_View.setCenter(e->getParty().getPos());
    e->m_Level.Update(e->getTimeRunning(), e->getParty().getPos(), e->viewExtents);
    while(!m_paused) ProcessLine();
}

void Dialog::Draw(Engine* e) {
    e->m_RenderWindow.setView(e->m_View);
    e->m_Level.Draw(e->m_RenderWindow);
    e->getParty().Draw(e->m_RenderWindow);
    e->m_RenderWindow.setView(e->m_ViewBase);
    m_Boxes.DrawBoxes(e->m_RenderWindow);

    if (m_currFace) e->m_RenderWindow.draw(*m_currFace);

    if(displayBlock.size() > 4) { /// We'll need a 'there's more text above/below' arrow.
        if (m_cdl > 0) m_Boxes.drawArrow(sf::Vector2f(1920 / 2, 800), 1, sf::Vector2f(1.0, 3.0), e->m_RenderWindow, true);
        if (m_cdl + 4 < (int)displayBlock.size()) m_Boxes.drawArrow(sf::Vector2f(1920 / 2, 1000), 3, sf::Vector2f(1.0, 3.0), e->m_RenderWindow, true);
    }

    if (m_mode == DIALOG_MODE_CHOICE) {
        m_Boxes.drawArrow(sf::Vector2f(500, 838 + (40 * m_choice)), 2, sf::Vector2f(1.0, 1.0), e->m_RenderWindow, true);
    }
}

void Dialog::doGoto(int dest) {
    setCurrentLine(dest);
}

void Dialog::doGoto(std::string dest) {
    if (dest == "START") {
        setCurrentLine(0);
    } else if (dest == "END") {
        setCurrentLine(dialog.size() - 1);
    } else if (dest == "CHOICESTART") {
        setCurrentLine(choiceLineRefs[0] - 1);
        choiceLineRefs.clear();
    } else {
        auto it = bookmarks.find(dest);
        if (it == bookmarks.end()) {
            std::cerr << "Error Invalid Bookmark: " << dest << std::endl;
        } else {
            setCurrentLine(it->second);
        }
    }
}


void Dialog::doStore(std::vector<std::string> line) {
    e->setCurrentScript(line[0]);
    //if(line[0]=="Sleep") m_Boxes.getBoxes().clear();
    e->PushState(Store::Instance());

    //e->ChangeState(Action_Cutscene::Instance());
}

void Dialog::doAction(std::vector<std::string> line) {
    e->setCurrentScript(line[0]);
    //if(line[0]=="Sleep") m_Boxes.getBoxes().clear();
    e->PushState(Action_Cutscene::Instance());
    //e->ChangeState(Action_Cutscene::Instance());
}

void Dialog::doCondition(std::vector<std::string> line) {
    std::string command = line[0];
    //std::cout << "Condition: " << Tool::reconstituteString(line) << std::endl;
    bool pass = true;

    if (command == "TRUE") pass = true;
    else if (command == "FALSE") pass = false;
    else if (command == "HAS_GREATER_THAN_GOLD_AMOUNT") {
        std::stringstream ss;
        ss << line[1];
        ss << line[1];
        int amount;
        ss >> amount;
        pass = (e->getParty().getPartyGold() > amount);
        std::cout << "DEBUG GOLD: Pass: " << pass << " amount: " << amount << " Gold: " << e->getParty().getPartyGold() << std::endl;
    } else if (command == "HAS_LESS_THAN_GOLD_AMOUNT") {
        std::stringstream ss;
        ss << line[1];
        int amount;
        ss >> amount;
        pass = (e->getParty().getPartyGold() < amount);
        std::cout << "DEBUG GOLD: Pass: " << pass << " amount: " << amount << " Gold: " << e->getParty().getPartyGold() << std::endl;
    } else if (command == "HAS_GREATER_THAN_ITEM_AMOUNT") {
        std::stringstream ss;
        int item, amount;
        ss << line[1] << " " << line[2];
        ss >> item >> amount;
        pass = (HeroPtr->getInventoryAmount(item) > amount);
        //std::cout << "Hero Has: " << HeroPtr->getInventoryAmount(item) << " \"" << e->Items[item].name << "\"" << std::endl;
    } else if (command == "HAS_LESS_THAN_ITEM_AMOUNT") {
        std::stringstream ss;
        int item, amount;
        ss << line[1] << " " << line[2];
        ss >> item >> amount;
        pass = HeroPtr->getInventoryAmount(item) < amount;
        //std::cout << "Hero Has: " << HeroPtr->getInventoryAmount(item) << " \"" << e->Items[item].name << "\"" << std::endl;
    } else if (command == "QUEST_HAS") {
        pass = (e->getParty().getSavedValue(line[1]) == line[2]);
    } else if (command == "QUEST_LACKS") {
        pass = (e->getParty().getSavedValue(line[1]) != line[2]);
    } else {
        std::cerr << "Error: Unrecognized Condition: " << Tool::reconstituteString(line) << std::endl;
    }

    /// Dealing with pass or failure.
    if(!pass) { // Skip current choice.
        advanceCurrentLine();
        while (peekCommand(0) != "COND_END") advanceCurrentLine();
        //advanceCurrentLine();
    }
}

void Dialog::doChoice(std::vector<std::string> line) {
    if (line[0] != "START") {
        doGoto(choiceLineRefs.back());
        return;
    }
    displayBlock.clear();
    choiceLineRefs.push_back(getCurrentLine());
    m_mode = DIALOG_MODE_CHOICE;
    //displayBlock.clear();
    m_cdl = m_choice = 0;
    bool gatheringChoiceData = true;

    while (gatheringChoiceData) {
        advanceCurrentLine();
        std::string command = getCommand();
        vecStr line = getLine();

        if (command == "CHOICE") {
            if (line[0] == "END") {
                gatheringChoiceData = false;
                choiceLineRefs.push_back(getCurrentLine());
                //PrintStats();
                DisplayCurrentBlock();
            } else {
                std::string output = Tool::reconstituteString(line);
                output.resize(55);
                displayBlock.push_back(output);
                choiceLineRefs.push_back(getCurrentLine());
            }
        }
    }
}

void Dialog::PrintStats() {
    std::clog << "-[ Dialog Stats: ]----------------------------------" << std::endl;
    std::clog << "Current Dialog Tag: " << NpcPtr->tag << std::endl;
    for (auto& x : dialog) std::clog << x.first << ": " << Tool::reconstituteString(x.second) << std::endl;
    std::clog << "Total lines: " << dialog.size() << std::endl;
    std::clog << "Choice Line Refs: ";
    for(const auto& n : choiceLineRefs) std::clog << n << " ";
    std::clog << std::endl << "-[ m_choice + m_cdl: " << (m_choice + m_cdl) << " ]------------------------------" << std::endl;
}

void Dialog::ProcessDialog() {
    std::string input;
    for (const auto& in : e->m_Level.getDialog(NpcPtr->tag)) {
        input = Tool::Trim(in);

        if (input.empty() || (input[0] == '#')) continue;

        if (input[0] == '!') {
            input.erase(input.begin());
            bookmarks[input] = dialog.size() - 1;
        } else {
            dialog[dialog.size()] = Tool::Tokenize(input);
        }
    }
    std::clog << "Dialog Lines Processed: " << dialog.size() << std::endl;
}

void Dialog::DebugLine(int cl) {
    auto it = dialog.begin();
    if (cl != -1) it = dialog.find(cl);
    else it = dialog.find(getCurrentLine());
    if (it == dialog.end()) std::cerr << "Debug Failure in Display Current Line." << std::endl;
    for (const auto& w : it->second) std::cout << w << " ";
    std::cout << std::endl;
}

std::string Dialog::peekCommand(int line_modifier) {
    auto it = dialog.find(getCurrentLine() + line_modifier);
    if (it == dialog.end()) return "NOT_FOUND";
    return it->second[0];
}


void Dialog::RewindConvo() {
    if (m_cdl > 0) {
        m_Boxes.PlaySelect();
        --m_cdl;
    }
}

void Dialog::AdvanceConvo() {
    if (m_cdl < (int)displayBlock.size() - 4) {
        m_Boxes.PlaySelect();
        ++m_cdl;
    }
}

void Dialog::DisplayCurrentBlock() {
    int spacer = 0;
    m_Boxes.getBox("Convo").getText().clear();

    for (int i = m_cdl; i < m_cdl + 4; ++i) {
        if(i < (int)displayBlock.size()) {
            m_Boxes.getBox("Convo").addText(displayBlock[i], sf::Vector2f(260, 10 + (40 * spacer++)), 40);
        }
    }
    m_paused = true;
}

void Dialog::PrevChoice() {
    if(m_choice > 0) {
        m_Boxes.PlaySelect();
        m_choice--;
    } else RewindConvo();
}

void Dialog::NextChoice() {
    if(m_choice < 3 && m_choice < (int)displayBlock.size() - 1) {
        m_Boxes.PlaySelect();
        m_choice++;
    } else if(m_choice == 3 && m_choice + m_cdl < (int)displayBlock.size()) AdvanceConvo();
}

std::string Dialog::getCommand() {
    auto it = dialog.find(getCurrentLine());
    if (it == dialog.end()) {
        std::cerr << "Current Line not found: " << getCurrentLine() << std::endl;
        return "NOT_FOUND";
    }
    return it->second[0];
}

Dialog::vecStr Dialog::getLine() {
    auto it = dialog.find(getCurrentLine());
    if (it == dialog.end()) {
        std::cerr << "Current Line not found: " << getCurrentLine() << std::endl;
        e->PopState();
    }
    std::vector<std::string> line = it->second;
    if(!line.empty()) line.erase(line.begin());
    return line;
}
