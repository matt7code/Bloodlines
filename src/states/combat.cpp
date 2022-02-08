#include "states/combat.h"

Combat Combat::m_Combat;

void Combat::Init(Engine *eng)
{
    e = eng;
    if(AllPartyMembersAreDead()) e->PopState();
    ResetValues();

    // Generate Arena
    Hero blank;
    blank.setActive(false);
    // Populate the arena with blank combatants
    for (int i = 0; i < 22; ++i) m_arena.push_back(blank);
    PopulatePositionVector();
    // Initialize Combat Parameters
    Beacon* BeaconPtr = e->m_Level.getNearestCombatBeacon(sf::Vector3i(e->getParty().getPos().x, e->getParty().getPos().y, e->m_Level.z_Level));

    if(e->getCurrentScriptCombat().empty()) {
        if(!BeaconPtr) {
            std::cerr << "Critical Error: No combat beacon found and getCurrentScriptCombat is empty." << std::endl;
            return;
        } else {
            m_difficulty = BeaconPtr->getValue3();
            m_goldMult   = BeaconPtr->getValue4();
            m_expMult    = BeaconPtr->getValue5();
            PopulateEnemyParty(BeaconPtr->getText1());
        }
    } else {
        //std::cout << "Check: " << e->getCurrentScriptCombat() << std::endl;
        PopulateEnemyParty(e->getCurrentScriptCombat(), true);
        e->setCurrentScriptCombat("");
    }
    // Generate Background
    int background = 0;
    Beacon* location = e->m_Level.getNearestLocationBeacon(sf::Vector3i(e->getParty().getPos().x, e->getParty().getPos().y, e->m_Level.z_Level));
    if (location) background = location->getValue2();
    else background = Tool::getRand(1, 8);

    switch(background) {
    case BACKGROUND_NONE:
        if(!m_BackgroundTex.loadFromFile("graphics/battle_scene_plain.png")) std::cerr << "File Load error.";
        break;
    case BACKGROUND_DUNGEON:
        if(!m_BackgroundTex.loadFromFile("graphics/battle_scene_dungeon.png")) std::cerr << "File Load error.";
        break;
    case BACKGROUND_FOREST:
        if(!m_BackgroundTex.loadFromFile("graphics/battle_scene_forest.png")) std::cerr << "File Load error.";
        break;
    case BACKGROUND_PLAIN:
        if(!m_BackgroundTex.loadFromFile("graphics/battle_scene_plain.png")) std::cerr << "File Load error.";
        break;
    case BACKGROUND_TOWN:
        if(!m_BackgroundTex.loadFromFile("graphics/battle_scene_town.png")) std::cerr << "File Load error.";
        break;
    case BACKGROUND_UNDERWATER:
        if(!m_BackgroundTex.loadFromFile("graphics/battle_scene_underwater.png")) std::cerr << "File Load error.";
        break;
    case BACKGROUND_OCEAN:
        if(!m_BackgroundTex.loadFromFile("graphics/battle_scene_water.png")) std::cerr << "File Load error.";
        break;
    case BACKGROUND_CASTLE:
        if(!m_BackgroundTex.loadFromFile("graphics/battle_scene_castle_interior.png")) std::cerr << "File Load error.";
        break;
    case BACKGROUND_CABIN:
        if(!m_BackgroundTex.loadFromFile("graphics/battle_scene_town_interior.png")) std::cerr << "File Load error.";
        break;
    default:
        break;
    }
    m_Background.setTexture(m_BackgroundTex);

    // Generate Party
    for(int i = 0; i < e->getParty().getSize(); ++i) {
        Hero hero = e->getParty().getMember(i);
        hero.getSprite().setScale(sf::Vector2f(4.0f, 4.0f));
        hero.getSprite().setTextureRect(sf::IntRect(hero.getTextureSheetBase().x + 32, hero.getTextureSheetBase().y + 32, 32, 32));
        if(hero.isDead()) hero.getSprite().setTextureRect(sf::IntRect(32, 1280, 32, 32));

        switch(i) { // Party Size Specific placement
        case 0:
            hero.getSprite().setPosition(sf::Vector2f(1110, 655));
            break;
        case 1:
            hero.getSprite().setPosition(sf::Vector2f(1145, 705));
            break;
        case 2:
            hero.getSprite().setPosition(sf::Vector2f(1180, 755));
            break;
        case 3:
            hero.getSprite().setPosition(sf::Vector2f(1320, 680));
            break;
        case 4:
            hero.getSprite().setPosition(sf::Vector2f(1355, 730));
            break;
        case 5:
            hero.getSprite().setPosition(sf::Vector2f(1520, 680));
            break;
        case 6:
            hero.getSprite().setPosition(sf::Vector2f(1555, 730));
            break;
        default:
            std::cerr << "Error: Oversize Party, problem placing them in combat." << std::endl;
            break;
        }
        hero.setActive(true);

        if     (hero.getClass() == CLASS_WARRIOR)   hero.setSpecial("Shouts    ");
        else if(hero.getClass() == CLASS_ROGUE)     hero.setSpecial("Tricks    ");
        else if(hero.getClass() == CLASS_CLERIC)    hero.setSpecial("Prayers   ");
        else if(hero.getClass() == CLASS_WIZARD)    hero.setSpecial("Spells    ");
        m_arena[i + 15] = hero;
    }

    // Generate UI
    m_Boxes.addBox("Commands", sf::Vector2f(601, 268), sf::Vector2f(18, 796));
    m_Boxes.addBox("Party", sf::Vector2f(1250, 268), sf::Vector2f(651, 796));

    // Restart Combat Music
    e->stopMusic();
    e->playMusic(Engine::SONG_BATTLE);

    // Set the combat up
    m_lastTime = e->getTimeRunning().getElapsedTime();
    m_action.actor = GetFirstLiveHero();

    if (PartyAllDead()) {
        std::cerr << "The Party is dead." << std::endl;
        return;
    }

    //m_action.action = m_arena[m_action.actor].lastCommand;
    //m_action.sub_action = m_arena[m_action.actor].lastSubCommand;
    DrawCommandText();

    m_action.actor = GetFirstLiveHero();
    m_action.target = GetFirstLiveMonster();
    RefreshInfoBox(e);
}

void Combat::Update(Engine* e)
{
    m_delta     = e->getTimeRunning().getElapsedTime() - m_lastTime;
    m_lastTime  = e->getTimeRunning().getElapsedTime();

    ScaleArrow(e);
    ProcessMessages();

    if(ReportMouse) {
        e->m_RenderWindow.setMouseCursorVisible(true);
        std::cout << (1920.0f / e->m_RenderWindow.getSize().x) * sf::Mouse::getPosition(e->m_RenderWindow).x << "x " << (1080.0f / e->m_RenderWindow.getSize().y) * sf::Mouse::getPosition(e->m_RenderWindow).y << "y" << std::endl;
    }

    float resultsTimeLimit = 1.0f;
    float AnimationRatio;
    //Effect damage;
    //std::stringstream ss;
    sf::Color tempColor;

    switch(m_phase) {
//    case PHASE_COMMAND:
//        if(m_Fleeing) { EndCombat(e); }
    case PHASE_ATTACK:
        std::cout << "\nATTACK" << std::endl;
        /// ======================================================
        /// ATTACK PHASE: Automated Phase where the party attacks.
        /// ======================================================
        if(m_actionQueue.empty()) {
            m_action.actor = GetFirstLiveMonster();
            m_phase = PHASE_DEFEND;
            m_acceptingInput = false;
        } else {
            m_action = m_actionQueue.front();
            m_actionQueue.pop_front();

            /// Stance Change?
            if(m_action.action == Action::COMMAND_STANCE) {
                // Can't stance change with an enemy
                if(!isTargetInParty(m_arena[m_action.target])) m_action.target = m_action.actor;
            } else {
                if(m_arena[m_action.target].isDead()) m_action.target = GetFirstLiveMonster();
            }
            TakeTurn();
//            std::cout << "M.Action Actor: " << m_action.actor << std::endl;
//            std::cout << "Break! Phase attack" << std::endl;
//            std::cout << "m_actionQueue: " << m_actionQueue.size() << std::endl;
        }
        break;

    case PHASE_RESULTS:
        //std::cout << "R" << std::endl;
        /// ===================================================================================
        /// RESULTS PHASE: Automated Phase where damage and healing are dealt after every turn.
        /// ===================================================================================
        /// Results phase start.
        if(m_resultsJustBegan == true) {
            std::cout << "\nRESULTS" << std::endl;
            m_resultsJustBegan = false;
            // This section gets run once, and can do things like
            // figure out the initial damage for a spell or attack,
            // or healing, or group damage from an AoE attack,
            // or group heal.
//            LOGaction();

            switch (m_action.action) {
            case Action::COMMAND_ATTACK:
                DoAttack();
                break;
            case Action::COMMAND_FLEE:
                break;
            case Action::COMMAND_GUARD:
                EffectsPlace(m_action.actor, "Guard", sf::Color::White);
                break;
            case Action::COMMAND_SPECIAL:
                DoSpecial();
                break;
            case Action::COMMAND_STANCE:
                EffectsPlace(m_action.actor, "Switch", sf::Color::Yellow);
                EffectsPlace(m_action.target, "Switch", sf::Color::Yellow);
                break;
            case Action::COMMAND_USE:
                EffectsPlace(m_action.actor, "Use", sf::Color::White);
                break;
            default:
                std::cerr << "PHASE_RESULTS has an invalid m_action.action: " << m_action.action << std::endl;
                break;
            }
        }
        AnimationRatio = (e->getTimeRunning().getElapsedTime().asSeconds() - m_resultsTimeStart.asSeconds()) / resultsTimeLimit;

        /// Results phase loop (AnimationRatio 0.0f -> 1.0f)
        EffectsAnimate(AnimationRatio);
        if(m_action.action == Action::COMMAND_STANCE) DoStanceChange(AnimationRatio);

        // Check for Death

        for(unsigned i = 0; i < m_arena.size(); ++i) {
            Hero& combatant = m_arena[i];
            if (combatant.isDead() && !combatant.m_DeathProcessed) {
                sf::Sprite& sprite = combatant.getSprite();
                sf::Color temp = sprite.getColor();
                //std::cout << "TempColorGet: " << (int)temp.r << " " << (int)temp.g << " " << (int)temp.b << " " << (int)temp.a << std::endl;
                if(i > 14) {
                    if (AnimationRatio < 0.5f) {
                        sprite.setColor(sf::Color(temp.r, temp.g, temp.b, 256 - (AnimationRatio * 512)));
                    } else {
                        sprite.setTextureRect(sf::IntRect(32, 1280, 32, 32));
                        sprite.setColor(sf::Color(temp.r, temp.g, temp.b, 0 + ((AnimationRatio - 0.5) * 512)));
                    }
                } else {
                    //std::cout << "TempColorUse: " << (int)temp.r << " " << (int)temp.g << " " << (int)temp.b << " " << (int)temp.a << std::endl;
                    //sprite.setColor(sf::Color(temp.r, temp.g, temp.b, 255 - (AnimationRatio * 255)));
                    sprite.setColor(sf::Color(255, 255, 255, 255 - (AnimationRatio * 255)));
                }
            }
        }

        /// Results phase done.
        if (e->getTimeRunning().getElapsedTime().asSeconds() - m_resultsTimeStart.asSeconds() > resultsTimeLimit) {
            for(auto& combatant : m_arena) {
                    if(combatant.isDead() && !combatant.m_DeathProcessed) {
                        combatant.m_DeathProcessed = true;
                        DoDeath(combatant);
                    }
                }
            if(m_action.action == Action::COMMAND_STANCE) {
                tempColor = m_arena[m_action.actor].getSprite().getColor();
                m_arena[m_action.actor].getSprite().setColor(sf::Color(tempColor.r, tempColor.g, tempColor.b, 255));
                tempColor = m_arena[m_action.target].getSprite().getColor();
                m_arena[m_action.target].getSprite().setColor(sf::Color(tempColor.r, tempColor.g, tempColor.b, 255));
            }
            m_stanceSwitchPhaseOne = true;
            m_resultsJustBegan = true;
            m_phase = m_prevPhase;
            m_texts.clear();

            /// Handling Deaths (Errors) essentially cosmetic
            //if(m_arena[m_action.target].isDead()) DoDeath(m_arena[m_action.target]);


            if (PartyAllDead())         DoLose(e);
            else if(MonstersAllDead())  DoWin(e);
            else {
                if(m_phase == PHASE_ATTACK) {
                    if (m_actionQueue.empty()) {
                        m_phase             = PHASE_DEFEND;
                        m_action.actor      = GetFirstLiveMonster();
                        m_acceptingInput    = false;
                    }
                } else if(m_phase == PHASE_DEFEND) {
                    m_action.actor = GetNextLiveMonster();
                    if(m_action.actor == ERROR) {
                        m_action.actor      = GetFirstLiveHero();
                        m_phase             = PHASE_COMMAND;
                        m_acceptingInput    = true;
                        if(m_Fleeing) EndCombat(e);
                    }
                } else if(m_phase == PHASE_WIN) {
                    std::cout << "Party victorius." << std::endl;
                } else if(m_phase == PHASE_LOSE) {
                    std::cout << "Party defeated." << std::endl;
                } else {
                    std::cerr << "Unrecognized prevPhase in Results: " << m_phase << std::endl;
                }
            }
        }
        break;

    case PHASE_DEFEND:
        std::cout << "\nDEFEND" << std::endl;
        /// ========================================================
        /// DEFEND PHASE: Automated Phase where the monsters attack.
        /// ========================================================
        if(m_action.actor == ERROR) {
            m_phase             = PHASE_COMMAND;
            m_action.actor      = GetFirstLiveHero();
            m_action.target     = GetFirstLiveMonster();
            m_action.action     = m_arena[m_action.actor].lastCommand;
            m_action.sub_action = m_arena[m_action.actor].lastSubCommand;
        } else {
            m_action.target     = GetPartyTarget();
            m_action.action     = 0;
            m_action.sub_action = 0;
            TakeTurn();
        }
        break;
    default:
        break;
    }
    RefreshInfoBox(e);
    return;
}

void Combat::EffectsPlace(int subject, std::string effect, sf::Color col)
{
    //std::cout << "Effect: " << effect << std::endl;
    sf::Text damageText;

    // Content:
    damageText.setFont(m_Boxes.getFont(Box::FONT_NINJA));
    damageText.setString(effect);
    damageText.setCharacterSize(60);
    damageText.setOrigin(sf::Vector2f(damageText.getLocalBounds().width / 2, damageText.getLocalBounds().height / 2));

    // Position:
    sf::Vector2f pos = m_arena[subject].getSprite().getPosition();
    pos = sf::Vector2f(pos.x, pos.y - 25);
    damageText.setPosition(pos);


    damageText.setColor(sf::Color::Black);
    m_texts.push_back(std::pair<sf::Text, sf::Vector2f>(damageText, damageText.getPosition()) );
    damageText.move(3, 3);
    damageText.setColor(col);
    m_texts.push_back(std::pair<sf::Text, sf::Vector2f>(damageText, damageText.getPosition()) );
}

void Combat::EffectsAnimate(float ratioCompletion)
{
    //std::cout << "EffectsAnimate: " << ratioCompletion << " / " << m_texts.size() << std::endl;
    float bounce = std::sin(ratioCompletion * 180 * RADIANS) * 100;

    for(auto& t : m_texts) {
        t.first.setPosition(sf::Vector2f(t.second.x, t.second.y - bounce));
        //std::cout << "X, Y: " << t.first.getPosition().x << "/" << t.first.getPosition().y << " Bounce: " << bounce << std::endl;
    }
}

void Combat::EffectsDraw()
{
    for(auto& t : m_texts) e->m_RenderWindow.draw(t.first);
}

void Combat::ReportDamage(float damage, int target, std::string desc)
{
    std::stringstream ss;
    ss << m_arena[m_action.actor].getName() << desc << m_arena[target].getName() << " for " << (int)damage << " health.";
    m_messages.push_back(StatusMessage(ss.str()));
}

void Combat::Resume(Engine* eng)
{
    //std::cout << "Resume: (10 = RESULTS) Phase " << m_phase << std::endl;
    m_resultsTimeStart = e->getTimeRunning().getElapsedTime();
}

void Combat::TakeTurn()
{
    e->m_pCombat_Arena      = &m_arena;
    e->m_pCombat_Boxes      = &m_Boxes;
    e->m_pCombat_Background = &m_Background;
    e->m_pCombat_Messages   = &m_messages;
    e->m_pCombat_Action     = &m_action;

    m_prevPhase = m_phase;
    m_phase = PHASE_RESULTS;
    e->PushState( Combat_Cutscene::Instance() );
}

void Combat::DoDeath(Hero& combatant)
{
    if(isTargetInParty(combatant)) {
        combatant.getSprite().setTextureRect(sf::IntRect(32, 1280, 32, 32));
        combatant.getSprite().setColor(sf::Color::White);
    } else {
        IncrementKillRecord(combatant.getTag());
        combatant.getSprite().setColor(sf::Color::Transparent);
    }
    m_messages.push_back(StatusMessage(combatant.getName() + " dies.", sf::seconds(2.0f)));
}

void Combat::IncrementKillRecord(std::string tag)
{
    std::string result;
    result = e->getParty().getSavedValue(tag);

    std::stringstream ss;
    int kills = 0;
    if(result != "NOT_FOUND") {
        ss.str(result);
        ss >> kills;
    }
    ss.clear();
    ss.str();
    ss << ++kills;
    e->getParty().setSavedValue(tag, ss.str());
}

void Combat::DoLose(Engine * e)
{
    m_acceptingInput = true;
    m_messages.push_back(StatusMessage("The party is wiped out, all hope is lost.", sf::seconds(60.0f)));
    m_phase = PHASE_LOSE;
}

void Combat::SelectConfirm(Engine * e)
{
    if(m_phase != PHASE_WIN && m_phase != PHASE_LOSE) m_Boxes.PlayConfirm();

    switch (m_phase) {
    case PHASE_COMMAND:
        m_arena[m_action.actor].lastCommand = m_action.action;
        switch (m_action.action) {
        case Action::COMMAND_ATTACK:
            m_action.target = GetFirstLiveMonster();
            m_phase = PHASE_TARGET;
            break;

        case Action::COMMAND_SPECIAL:
            m_action.sub_action = m_arena[m_action.actor].lastSubCommand;
            m_phase = PHASE_SPELL;
            m_Boxes.addBox("Spells", sf::Vector2f(1200, 220), sf::Vector2f(340, 820));
            m_Boxes.addBox("SpellsTitle", sf::Vector2f(175, 32), sf::Vector2f(400, 780));
            m_Boxes.getBox("SpellsTitle").addText(m_arena[m_action.actor].getSpecial(), sf::Vector2f(20, -10), 36);

            for(unsigned i = 0; i < m_arena[m_action.actor].getSpells().size(); ++i) {
                Special tempSpell(m_arena[m_action.actor].getSpells()[i]);
                m_Boxes.getBox("Spells").addText(tempSpell.m_name, sf::Vector2f(((i % 4) * 300) + 50, ((i / 4) * 48) + 10), 36);
            }
            break;

        case Action::COMMAND_USE:
            m_action.sub_action = m_arena[m_action.actor].lastSubCommand;
            m_phase = PHASE_ITEM;
            m_Boxes.addBox("Items", sf::Vector2f(1200, 220), sf::Vector2f(340, 820));
            m_Boxes.addBox("ItemsTitle", sf::Vector2f(200, 32), sf::Vector2f(400, 780));
            m_Boxes.getBox("ItemsTitle").addText("Inventory", sf::Vector2f(20, -10), 36);

            for(unsigned i = 0; i < m_arena[m_action.actor].getInventory().size(); i += 3 ) {
                m_Boxes.getBox("Items").addText("Unknown Item", sf::Vector2f(((i % 4) * 300) + 50, ((i / 4) * 48) + 10), 36);
            }
            break;
        case Action::COMMAND_GUARD:
            m_arena[m_action.actor].TakeStatus(Status(Status::STATUS_GUARD)); // The character immidiately gains 50% damage reduction

            m_actionQueue.push_back(m_action);
            m_action.actor = GetNextLiveHero();
            if(m_action.actor == ERROR) {
                AdvanceToActionPhase(e);
            } else {
                RefreshInfoBox(e);
                DrawCommandText();
                m_phase = PHASE_COMMAND;
            }

            break;
        case Action::COMMAND_FLEE:
            if(m_canFlee) {
                for (int i = 15; i < 22; ++i) {
                    if(m_arena[i].isAlive() && m_arena[i].isActive()) {
                        sf::IntRect cp = m_arena[i].getSprite().getTextureRect();
                        cp.top += 32;
                        m_arena[i].getSprite().setTextureRect(cp);
                        m_arena[i].TakeStatus(Status(Status::STATUS_FLEE));
                    }
                    m_Fleeing = true;
                    m_acceptingInput    = false;
                    m_action.actor      = GetFirstLiveMonster();
                    m_phase             = PHASE_DEFEND;
                    RefreshInfoBox(e);
                }
            } else {
                m_messages.push_back(StatusMessage("You cannot flee from this battle!"));
            }
            break;
        case Action::COMMAND_STANCE:
            // Immediate allow targeting another member and set up a switch
            // Both characters lose their turn switching
            m_action.target = GetFirstLiveHero();
            m_phase = PHASE_TARGET;
            break;
        }
        break;
    case PHASE_ITEM:
        m_arena[m_action.actor].lastSubCommand = m_action.sub_action;
        m_Boxes.removeBox("Items");
        m_Boxes.removeBox("ItemsTitle");
        m_action.target = GetFirstLiveHero();
        m_action.sub_action = m_arena[m_action.actor].getInventory()[m_action.sub_action].type;
        m_phase = PHASE_TARGET;
        break;
    case PHASE_SPELL:
        m_arena[m_action.actor].lastSubCommand = m_action.sub_action;
        m_Boxes.removeBox("Spells");
        m_Boxes.removeBox("SpellsTitle");
        if(m_action.sub_action == Special::SPELL_RESURRECT) m_action.target = GetFirstDeadHero();
        else if(m_arena[m_action.actor].getClass() == CLASS_CLERIC) m_action.target = GetFirstLiveHero();
        else m_action.target = GetFirstLiveMonster();
        m_action.sub_action = m_arena[m_action.actor].getSpells()[m_action.sub_action];
        m_phase = PHASE_TARGET;
        break;

    case PHASE_TARGET:
        if(m_action.action == Action::COMMAND_STANCE && !isTargetInParty(m_arena[m_action.actor])) {
            m_Boxes.PlayCancel();
        } else {
            m_actionQueue.push_back(m_action);
            m_action.actor = GetNextLiveHero();
            if(m_action.actor == ERROR) AdvanceToActionPhase(e);
            else {
                RefreshInfoBox(e);
                m_action.action = m_arena[m_action.actor].lastCommand;
                DrawCommandText();
                m_phase = PHASE_COMMAND;
            }
        }
        break;

    default:
        break;
    }
}

void Combat::AdvanceToActionPhase(Engine* e)
{
    for(auto it = m_arena.begin(); it != m_arena.end(); ++it) {
        if(it->isActive() && it->isAlive()) it->UpdateStatus();
    }
    m_acceptingInput    = false;
    m_phase             = PHASE_ATTACK;
    m_action.actor      = m_actionQueue.front().actor;

    RefreshInfoBox(e);
}

int Combat::GetFirstLiveHero()
{
    if (PartyAllDead()) return ERROR;
    int n = 15;
    while(m_arena[n].isDead()) ++n;
    return n;
}

int Combat::GetFirstDeadHero()
{
    if (PartyAllDead()) return ERROR;
    int n = 15;
    while(m_arena[n].isActive() && m_arena[n].isAlive()) ++n;
    if (n > 21) return GetFirstLiveHero();
    return n;
}

int Combat::GetFirstLiveMonster()
{
    if(MonstersAllDead()) return ERROR;
    int n = 0;
    while(m_arena[n].isDead() || m_arena[n].isInactive()) ++n;
    return n;
}

int Combat::GetLastLiveHero()
{
    if (PartyAllDead()) return ERROR;
    int n = 21;
    while(!m_arena[n].isActive() || m_arena[n].isDead()) --n;
    return n;
}

int Combat::GetLastLiveMonster()
{
    if(MonstersAllDead()) return ERROR;
    int n = 14;
    while(!m_arena[n].isActive() || m_arena[n].isDead()) --n;
    return n;
}


bool Combat::PartyAllDead()
{
    bool ret = true;
    for(unsigned i = 15; i < 22; i++) {
        if (m_arena[i].isActive() && m_arena[i].isAlive()) ret = false;
    }
    return ret;
}

bool Combat::MonstersAllDead()
{
    bool ret = true;
    for(unsigned i = 0; i < 15; i++) {
        if (m_arena[i].isActive() && m_arena[i].isAlive()) ret = false;
    }
    return ret;
}

int Combat::GetNextLiveMonster()
{
    for (unsigned i = m_action.actor + 1; i < 15; ++i) if(m_arena[i].isActive() && m_arena[i].isAlive()) return i;
    return ERROR;
}

int Combat::GetNextLiveHero()
{
    for (unsigned i = m_action.actor + 1; i < 22; ++i) {
        if(m_arena[i].isActive() && m_arena[i].isAlive()) return i;
    }
    return ERROR;
}

void Combat::EndCombat(Engine * e)
{
    m_acceptingInput = true;
    for (unsigned i = 15; i < 22; ++i) {
        if(m_arena[i].isActive()) {
            e->getParty().getMember(i - 15).setCurHP(m_arena[i].getCurHP());
            e->getParty().getMember(i - 15).setCurMP(m_arena[i].getCurMP());
            e->getParty().getMember(i - 15).setLevel(m_arena[i].getLevel());
            e->getParty().getMember(i - 15).setExp(m_arena[i].getExp());
        }
    }
    e->getParty().MovePartyTo(e->getParty().getPos());
    e->PopState();
}

void Combat::SelectCancel(Engine * e)
{
    m_Boxes.PlayCancel();

    switch (m_phase) {
    case PHASE_NONE:
        break;
    case PHASE_COMMAND:
        e->PopState();
        break;

    case PHASE_DEFEND:
        e->PopState();
        break;
    case PHASE_ATTACK:
        e->PopState();
        break;

    case PHASE_TARGET:
        m_phase = PHASE_COMMAND;
        break;
    case PHASE_SPELL:
        m_Boxes.removeBox("Spells");
        m_Boxes.removeBox("SpellsTitle");
        m_phase = PHASE_COMMAND;
        break;
    case PHASE_ITEM:
        m_Boxes.removeBox("Items");
        m_Boxes.removeBox("ItemsTitle");
        m_phase = PHASE_COMMAND;
        break;
    default:
        break;
    }
}

void Combat::DrawParty(Engine * e)
{
    int n = 0;
    for (int i = 15; i < 22; ++i)
        if(m_arena[i].isActive()) n++;

    switch(n) {
    case 1:
        e->m_RenderWindow.draw(m_arena[15].getSprite());
        break;
    case 2:
        e->m_RenderWindow.draw(m_arena[15].getSprite());
        e->m_RenderWindow.draw(m_arena[16].getSprite());
        break;
    case 3:
        e->m_RenderWindow.draw(m_arena[15].getSprite());
        e->m_RenderWindow.draw(m_arena[16].getSprite());
        e->m_RenderWindow.draw(m_arena[17].getSprite());
        break;
    case 4:
        e->m_RenderWindow.draw(m_arena[15].getSprite());
        e->m_RenderWindow.draw(m_arena[16].getSprite());
        e->m_RenderWindow.draw(m_arena[18].getSprite());
        e->m_RenderWindow.draw(m_arena[17].getSprite());
        break;
    case 5:
        e->m_RenderWindow.draw(m_arena[15].getSprite());
        e->m_RenderWindow.draw(m_arena[18].getSprite());
        e->m_RenderWindow.draw(m_arena[16].getSprite());
        e->m_RenderWindow.draw(m_arena[19].getSprite());
        e->m_RenderWindow.draw(m_arena[17].getSprite());
        break;
    case 6:
        e->m_RenderWindow.draw(m_arena[15].getSprite());
        e->m_RenderWindow.draw(m_arena[18].getSprite());
        e->m_RenderWindow.draw(m_arena[16].getSprite());
        e->m_RenderWindow.draw(m_arena[19].getSprite());
        e->m_RenderWindow.draw(m_arena[17].getSprite());
        e->m_RenderWindow.draw(m_arena[20].getSprite());
        break;
    case 7:
    default:
        e->m_RenderWindow.draw(m_arena[15].getSprite());
        e->m_RenderWindow.draw(m_arena[18].getSprite());
        e->m_RenderWindow.draw(m_arena[16].getSprite());
        e->m_RenderWindow.draw(m_arena[19].getSprite());
        e->m_RenderWindow.draw(m_arena[20].getSprite());
        e->m_RenderWindow.draw(m_arena[17].getSprite());
        e->m_RenderWindow.draw(m_arena[21].getSprite());
        break;
    }
}

void Combat::DrawMonsters(Engine * e)
{
    // Draw 3 rows of enemies from back to front.
    for (int i = 2; i < 15; i += 3) if (m_arena[i].isActive()) e->m_RenderWindow.draw(m_arena[i].getSprite());
    for (int i = 0; i < 15; i += 3) if (m_arena[i].isActive()) e->m_RenderWindow.draw(m_arena[i].getSprite());
    for (int i = 1; i < 15; i += 3) if (m_arena[i].isActive()) e->m_RenderWindow.draw(m_arena[i].getSprite());
}

void Combat::PlaceActorArrow(Engine * e)
{
    sf::Vector2f pos = sf::Vector2f(m_arena[m_action.actor].getSprite().getPosition().x, m_arena[m_action.actor].getSprite().getPosition().y - ROW_HEIGHT - 100);
    if(m_action.actor == m_action.target) pos.y -= 25;
    m_Boxes.drawArrow(pos, Box::ARROW_DOWN, sf::Vector2f(m_arrowScale.x, m_arrowScale.y * 2.0f), e->m_RenderWindow, true, sf::Color::Green);
}

void Combat::PlaceTargetArrow(Engine * e)
{
    sf::Color arrowColor;
    sf::Vector2f pos;

    if (isTargetInParty(m_arena[m_action.target])) arrowColor = sf::Color::Yellow;
    else arrowColor = sf::Color::Red;
//    arrowColor = sf::Color::Red;
    pos = sf::Vector2f(m_arena[m_action.target].getSprite().getPosition().x, m_arena[m_action.target].getSprite().getPosition().y - ROW_HEIGHT - 100);
    m_Boxes.drawArrow(pos, Box::ARROW_DOWN, sf::Vector2f(m_arrowScale.x, m_arrowScale.y * 2.0f), e->m_RenderWindow, true, arrowColor);

    sf::Text targetLabel;
    std::string targetName = m_arena[m_action.target].getName();
    targetLabel.setString(targetName);
    targetLabel.setFont(*m_Boxes.getBox("Commands").getFontNice());
    targetLabel.setCharacterSize(36);
    targetLabel.setOrigin(sf::Vector2f(((float)targetName.size() / 2.0f) * 18, 18));
    targetLabel.setPosition(sf::Vector2f(pos.x + 2, pos.y - 73));
    targetLabel.setColor(sf::Color::Black);
    e->m_RenderWindow.draw(targetLabel);
    targetLabel.setPosition(sf::Vector2f(pos.x, pos.y - 75));
    targetLabel.setColor(sf::Color::White);
    e->m_RenderWindow.draw(targetLabel);
}

void Combat::RefreshInfoBox(Engine * e)
{
    std::stringstream ss;
    Box& InfoBox = m_Boxes.getBox("Party");
    InfoBox.getText().clear();

    for (unsigned i = 15; i < 22; ++i) {
        if(m_arena[i].isActive()) {
            std::string statusName = m_arena[i].getName();
            std::string statusClass = Tool::pad(m_arena[i].getClassName(m_arena[i].getClass()), 7);
            ss.str("L");
            ss << m_arena[i].getLevel();
            std::string statusLevel = ss.str();
            ss.str("H: ");
            ss << m_arena[i].getCurHP() << "/" << m_arena[i].getMaxHP();
            std::string statusHealth = ss.str();
            ss.str("M: ");
            ss << m_arena[i].getCurMP() << "/" << m_arena[i].getMaxMP();
            std::string statusMagic = ss.str();

            int n = (i - 15) * 36;

            if(m_action.actor == i && m_phase != PHASE_DEFEND && m_phase != PHASE_WIN && m_phase != PHASE_LOSE) {
                InfoBox.addText(statusName,     sf::Vector2f(100, n), 38, Box::FONT_NORMAL, sf::Color::Green);
                InfoBox.addText(statusClass,    sf::Vector2f(400, n), 38, Box::FONT_NORMAL, sf::Color::Green);
                InfoBox.addText(statusLevel,    sf::Vector2f(600, n), 38, Box::FONT_NORMAL, sf::Color::Green);
                InfoBox.addText(statusHealth,   sf::Vector2f(700, n), 38, Box::FONT_NORMAL, sf::Color::Green);
                InfoBox.addText(statusMagic,    sf::Vector2f(950, n), 38, Box::FONT_NORMAL, sf::Color::Green);
            } else {
                InfoBox.addText(statusName,     sf::Vector2f(100, n), 38);
                InfoBox.addText(statusClass,    sf::Vector2f(400, n), 38);
                InfoBox.addText(statusLevel,    sf::Vector2f(600, n), 38);
                InfoBox.addText(statusHealth,   sf::Vector2f(700, n), 38);
                InfoBox.addText(statusMagic,    sf::Vector2f(950, n), 38);
            }

            if ((unsigned)m_action.target == i && m_phase == PHASE_TARGET && isTargetInParty(m_arena[m_action.actor])) {
                InfoBox.addText(statusName,     sf::Vector2f(100, n), 38, Box::FONT_NORMAL, sf::Color::Yellow);
                InfoBox.addText(statusClass,    sf::Vector2f(400, n), 38, Box::FONT_NORMAL, sf::Color::Yellow);
                InfoBox.addText(statusLevel,    sf::Vector2f(600, n), 38, Box::FONT_NORMAL, sf::Color::Yellow);
                InfoBox.addText(statusHealth,   sf::Vector2f(700, n), 38, Box::FONT_NORMAL, sf::Color::Yellow);
                InfoBox.addText(statusMagic,    sf::Vector2f(950, n), 38, Box::FONT_NORMAL, sf::Color::Yellow);
            }
        }
    }
}

void Combat::ScaleArrow(Engine * e)
{
    int arrow_timer = e->getTimeRunning().getElapsedTime().asMilliseconds() % 2000;
    if (arrow_timer > 1000) {
        arrow_timer -= 1000;
        int down_timer = 1000;
        down_timer -= arrow_timer;
        arrow_timer = down_timer;
    }
    float t = 0.001f * arrow_timer / 2;
    m_arrowScale = sf::Vector2f(t + 1.0f, t + 1.0f);
}

void Combat::PlaceCommandArrow(Engine * e)
{
    m_arrowDir = Box::ARROW_RIGHT;
    m_arrowPos = sf::Vector2f(56.0f + ((m_action.action / 3) * 320.0f), 865.0f + ((m_action.action % 3) * 70));
    m_Boxes.drawArrow(m_arrowPos, m_arrowDir, m_arrowScale, e->m_RenderWindow, true);
}

void Combat::Draw(Engine * e)
{
    e->m_RenderWindow.draw(m_Background);
    DrawParty(e);
    DrawMonsters(e);
    //sf::Sprite temp = m_arena[1].getSprite();
    //temp.setPosition(sf::Vector2f(200, 200));
    //e->m_RenderWindow.draw(temp);
    m_Boxes.DrawBoxes(e->m_RenderWindow);
    EffectsDraw();
//    for (const auto& s : m_effectNumbers) e->m_RenderWindow.draw(s);
//    for (const auto& e : m_texts) e->m_RenderWindow.draw(s);

    if(m_phase == PHASE_COMMAND) {
        PlaceCommandArrow(e);
        PlaceActorArrow(e);
    } else if(m_phase == PHASE_TARGET) {
        PlaceActorArrow(e);
        PlaceTargetArrow(e);
    } else if(m_phase == PHASE_SPELL || m_phase == PHASE_ITEM) {
        PlaceSubArrow(e);
    }
}

void Combat::PlaceSubArrow(Engine * e)
{
    m_Boxes.drawArrow(sf::Vector2f((m_action.sub_action % 4) * 300 + 375, (m_action.sub_action / 4) * 48 + 855), Box::ARROW_RIGHT, sf::Vector2f(m_arrowScale.x / 2.0f, m_arrowScale.y / 2.0f), e->m_RenderWindow, true);
}

void Combat::DoWin(Engine * e)
{
    m_acceptingInput = true;
    int gold = 10 * m_difficulty;
    float xp = m_difficulty * 8.0f;

    e->getParty().setPartyGold(e->getParty().getPartyGold() + gold);
    int liveMembers = 0;

    for(unsigned i = 0; i < m_arena.size(); ++i) {
        if(m_arena[i].isActive() && m_arena[i].isAlive()) liveMembers++;
    }

    float share = xp / (float)liveMembers;
    m_messages.push_back(StatusMessage("The party is victorious!", sf::seconds(60.0f)));
    std::stringstream ss;
    ss << "The party earns " << xp << " experience";
    if(gold > 0) ss << " and " << gold << "gp";
    else ss << ".";
    m_messages.push_back(StatusMessage(ss.str(), sf::seconds(60.0f)));

    for(unsigned i = 15; i < 22; ++i) {
        if(m_arena[i].isActive() && m_arena[i].isAlive()) {
            m_arena[i].setExp(m_arena[i].getExp() + share);
            ss.clear();
            ss.str(std::string());
            ss << m_arena[i].getName() << " gains " << (int)share << "xp.";
            m_messages.push_back(StatusMessage(ss.str()));
            if (m_arena[i].UpdateLevel()) {
                ss.clear();
                ss.str(std::string());
                ss << "Congratulations! " << m_arena[i].getName() << " is now level " << m_arena[i].getLevel() << ".";
                m_messages.push_back(StatusMessage(ss.str()));
            }
        }
    }
    e->stopMusic();
    e->playMusic(Engine::SONG_STATUS);
    m_phase = PHASE_WIN;
}

void Combat::ProcessMessages()
{
    const int text_size = 30;
    if(m_Boxes.hasBox("Messages")) m_Boxes.removeBox("Messages");
    if(!m_messages.empty()) {
        m_Boxes.addBox("Messages", sf::Vector2f(1020, 38 + (m_messages.size() * text_size)), sf::Vector2f(450, 16));
        int n = 0;
        for (auto it = m_messages.begin(); it != m_messages.end(); ) {
            it->duration -= m_delta;
            if(it->duration < sf::Time::Zero) m_messages.erase(it);
            else {
                m_Boxes.getBox("Messages").addText(it->message, sf::Vector2f(20.0f, 10 + (n * text_size)), text_size);
                ++it;
                ++n;
            }
        }
    }
}

void Combat::SelectUp()
{
    if(m_phase == PHASE_COMMAND) {
        m_Boxes.PlaySelect();
        m_action.action--;
        m_action.action %= 6;
    } else if(m_phase == PHASE_TARGET) {
        m_Boxes.PlaySelect();
        RotateUp();
        while(!m_arena[m_action.target].isActive() || (m_arena[m_action.target].isDead() && m_action.target < 15)) RotateUp();
    } else if(m_phase == PHASE_SPELL) {
        if (m_arena[m_action.actor].getSpells().empty()) return;
        m_Boxes.PlaySelect();
        m_action.sub_action -= 4;
        std::cout << "HERE: " << m_action.sub_action << std::endl;
        if (m_action.sub_action > 1000) m_action.sub_action = m_arena[m_action.actor].getSpells().size() - 1;
    } else if(m_phase == PHASE_ITEM) {
        if (m_arena[m_action.actor].getInventory().empty()) return;
        m_Boxes.PlaySelect();
        m_action.sub_action -= 4;
        std::cout << "HERE: " << m_action.sub_action << std::endl;
        if (m_action.sub_action > 1000) m_action.sub_action = m_arena[m_action.actor].getSpells().size() - 1;
    }
}

void Combat::SelectDown()
{
    if(m_phase == PHASE_COMMAND) {
        m_Boxes.PlaySelect();
        m_action.action ++;
        m_action.action %= 6;
    } else if(m_phase == PHASE_TARGET) {
        m_Boxes.PlaySelect();
        RotateDown();
        while(!m_arena[m_action.target].isActive() || (m_arena[m_action.target].isDead() && m_action.target < 15)) RotateDown();
    } else if(m_phase == PHASE_SPELL) {
        if (m_arena[m_action.actor].getSpells().empty()) return;
        m_Boxes.PlaySelect();
        m_action.sub_action += 4;
        if (m_action.sub_action >= m_arena[m_action.actor].getSpells().size()) m_action.sub_action = 0;
    } else if(m_phase == PHASE_ITEM) {
        if (m_arena[m_action.actor].getInventory().empty()) return;
        m_Boxes.PlaySelect();
        m_action.sub_action += 4;
        if (m_action.sub_action >= m_arena[m_action.actor].getInventory().size()) m_action.sub_action = 0;
    }
}

void Combat::SelectLeft()
{
    if(m_phase == PHASE_COMMAND) {
        m_Boxes.PlaySelect();
        if     (m_action.action == 0) m_action.action = 3;
        else if(m_action.action == 1) m_action.action = 4;
        else if(m_action.action == 2) m_action.action = 5;
        else {
            m_action.action -= 3;
            m_action.action %= 6;
        }
    } else if(m_phase == PHASE_TARGET) {
        m_Boxes.PlaySelect();
        RotateLeft();
        while(!m_arena[m_action.target].isActive() || (m_arena[m_action.target].isDead() && m_action.target < 15)) RotateLeft();
    } else if(m_phase == PHASE_SPELL) {
        if (m_arena[m_action.actor].getSpells().empty()) return;
        m_Boxes.PlaySelect();
        m_action.sub_action --;
        if (m_action.sub_action > 1000) m_action.sub_action = m_arena[m_action.actor].getSpells().size() - 1;
    } else if(m_phase == PHASE_ITEM) {
        if (m_arena[m_action.actor].getInventory().empty()) return;
        m_Boxes.PlaySelect();
        m_action.sub_action --;
        if (m_action.sub_action > 1000) m_action.sub_action = m_arena[m_action.actor].getSpells().size() - 1;
    }
}

void Combat::SelectRight()
{
    if(m_phase == PHASE_COMMAND) {
        m_Boxes.PlaySelect();
        m_action.action += 3;
        m_action.action %= 6;
    } else if(m_phase == PHASE_TARGET) {
        m_Boxes.PlaySelect();
        RotateRight();
        while(!m_arena[m_action.target].isActive() || (m_arena[m_action.target].isDead() && m_action.target < 15)) RotateRight();
    } else if(m_phase == PHASE_SPELL) {
        if (m_arena[m_action.actor].getSpells().empty()) return;
        m_Boxes.PlaySelect();
        m_action.sub_action++;
        if (m_action.sub_action >= m_arena[m_action.actor].getSpells().size()) m_action.sub_action = 0;
    } else if(m_phase == PHASE_ITEM) {
        if (m_arena[m_action.actor].getInventory().empty()) return;
        m_Boxes.PlaySelect();
        m_action.sub_action++;
        if (m_action.sub_action >= m_arena[m_action.actor].getInventory().size()) m_action.sub_action = 0;
    }
}

void Combat::HandleEvents(Engine * e)
{
    /// We exhaust the events if the module is not accepting input.
    if(!m_acceptingInput) {
        sf::Event event;
        while (e->m_RenderWindow.pollEvent(event)) {
            // There's a special exception to display the debugging info, or escape.
            if (event.type == sf::Event::KeyPressed && event.key.code == sf::Keyboard::Escape) {
                SelectCancel(e);
            } else if (event.type == sf::Event::KeyPressed && event.key.code == sf::Keyboard::Q) {
                e->PopState();
                //e->PopState();
            } else if (event.type == sf::Event::KeyPressed && event.key.code == sf::Keyboard::F1) {
                m_drawDebug = !m_drawDebug;
            }
        } // Exhaust events.
        return;
    }

    // Process events
    sf::Event event;
    while (e->m_RenderWindow.pollEvent(event)) {
        if (event.type == sf::Event::Closed) e->Quit();
        if (event.type == sf::Event::KeyPressed) {
            if(m_phase == PHASE_LOSE || m_phase == PHASE_WIN) EndCombat(e);

            switch(event.key.code) {
            case sf::Keyboard::F1:
                m_drawDebug = !m_drawDebug;
                break;

            case sf::Keyboard::Escape:
                SelectCancel(e);
                break;

            case sf::Keyboard::Up:
            case sf::Keyboard::W:
                SelectUp();
                break;

            case sf::Keyboard::Down:
            case sf::Keyboard::S:
                SelectDown();
                break;

            case sf::Keyboard::Right:
            case sf::Keyboard::D:
                SelectRight();
                break;

            case sf::Keyboard::Left:
            case sf::Keyboard::A:
                SelectLeft();
                break;

            case sf::Keyboard::Return:
                SelectConfirm(e);
                break;

            case sf::Keyboard::M:
                ReportMouse = !ReportMouse;
                e->m_RenderWindow.setMouseCursorVisible(ReportMouse);
                break;

            default:
                break;
            }
            if(m_phase == PHASE_TARGET) {
                RefreshInfoBox(e);
            }
        }
    }
}

void Combat::LOGaction()
{
    std::cout << "Actor  : " << m_action.actor << std::endl;
    std::cout << "Target : " << m_action.target << std::endl;
    std::cout << "Act/Sub: " << m_action.action << "/" << m_action.sub_action << std::endl;
    std::cout << "Name   : " << m_action.getName() << std::endl;
    std::cout << std::endl;
}

bool Combat::isTargetInParty(Hero& combatant)
{
    return !combatant.isMonster;
}

void Combat::RotateUp()
{
    switch(m_action.target) {
    case 0:
        m_action.target = 2;
        break;
    case 1:
        m_action.target = 0;
        break;
    case 2:
        m_action.target = 4;
        break;
    case 3:
        m_action.target = 5;
        break;
    case 4:
        m_action.target = 3;
        break;
    case 5:
        m_action.target = 7;
        break;
    case 6:
        m_action.target = 8;
        break;
    case 7:
        m_action.target = 6;
        break;
    case 8:
        m_action.target = 10;
        break;
    case 9:
        m_action.target = 11;
        break;
    case 10:
        m_action.target = 9;
        break;
    case 11:
        m_action.target = 13;
        break;
    case 12:
        m_action.target = 14;
        break;
    case 13:
        m_action.target = 12;
        break;
    case 14:
        m_action.target = 21;
        break;
    case 15:
        m_action.target = 1;
        break;
    case 16:
        m_action.target = 15;
        break;
    case 17:
        m_action.target = 16;
        break;
    case 18:
        m_action.target = 17;
        break;
    case 19:
        m_action.target = 18;
        break;
    case 20:
        m_action.target = 19;
        break;
    case 21:
        m_action.target = 20;
        break;
    }
}

void Combat::RotateDown()
{
    switch(m_action.target) {
    case 0:
        m_action.target = 1;
        break;
    case 1:
        m_action.target = 15;
        break;
    case 2:
        m_action.target = 0;
        break;
    case 3:
        m_action.target = 4;
        break;
    case 4:
        m_action.target = 2;
        break;
    case 5:
        m_action.target = 3;
        break;
    case 6:
        m_action.target = 7;
        break;
    case 7:
        m_action.target = 5;
        break;
    case 8:
        m_action.target = 6;
        break;
    case 9:
        m_action.target = 10;
        break;
    case 10:
        m_action.target = 8;
        break;
    case 11:
        m_action.target = 9;
        break;
    case 12:
        m_action.target = 13;
        break;
    case 13:
        m_action.target = 11;
        break;
    case 14:
        m_action.target = 12;
        break;
    case 15:
        m_action.target = 16;
        break;
    case 16:
        m_action.target = 17;
        break;
    case 17:
        m_action.target = 18;
        break;
    case 18:
        m_action.target = 19;
        break;
    case 19:
        m_action.target = 20;
        break;
    case 20:
        m_action.target = 21;
        break;
    case 21:
        m_action.target = 14;
        break;
    }
}

void Combat::RotateLeft()
{
    switch(m_action.target) {
    case 0:
        m_action.target = 3;
        break;
    case 1:
        m_action.target = 4;
        break;
    case 2:
        m_action.target = 5;
        break;
    case 3:
        m_action.target = 6;
        break;
    case 4:
        m_action.target = 7;
        break;
    case 5:
        m_action.target = 8;
        break;
    case 6:
        m_action.target = 9;
        break;
    case 7:
        m_action.target = 10;
        break;
    case 8:
        m_action.target = 11;
        break;
    case 9:
        m_action.target = 12;
        break;
    case 10:
        m_action.target = 13;
        break;
    case 11:
        m_action.target = 14;
        break;
    case 12:
        m_action.target = 21;
        break;
    case 13:
        m_action.target = 15;
        break;
    case 14:
        m_action.target = 20;
        break;
    case 15:
        m_action.target = 2;
        break;
    case 16:
        m_action.target = 0;
        break;
    case 17:
        m_action.target = 1;
        break;
    case 18:
        m_action.target = 16;
        break;
    case 19:
        m_action.target = 17;
        break;
    case 20:
        m_action.target = 18;
        break;
    case 21:
        m_action.target = 19;
        break;
    }
}

void Combat::RotateRight()
{
    switch(m_action.target) {
    case 0:
        m_action.target = 16;
        break;
    case 1:
        m_action.target = 17;
        break;
    case 2:
        m_action.target = 15;
        break;
    case 3:
        m_action.target = 0;
        break;
    case 4:
        m_action.target = 1;
        break;
    case 5:
        m_action.target = 2;
        break;
    case 6:
        m_action.target = 3;
        break;
    case 7:
        m_action.target = 4;
        break;
    case 8:
        m_action.target = 5;
        break;
    case 9:
        m_action.target = 6;
        break;
    case 10:
        m_action.target = 7;
        break;
    case 11:
        m_action.target = 8;
        break;
    case 12:
        m_action.target = 9;
        break;
    case 13:
        m_action.target = 10;
        break;
    case 14:
        m_action.target = 11;
        break;
    case 15:
        m_action.target = 13;
        break;
    case 16:
        m_action.target = 18;
        break;
    case 17:
        m_action.target = 19;
        break;
    case 18:
        m_action.target = 20;
        break;
    case 19:
        m_action.target = 21;
        break;
    case 20:
        m_action.target = 14;
        break;
    case 21:
        m_action.target = 12;
        break;
    }
}

bool Combat::row1empty()
{
    return ((m_arena[15].isInactive() || m_arena[15].isDead()) && (m_arena[16].isInactive() || m_arena[16].isDead()) && (m_arena[17].isInactive() || m_arena[17].isDead()));
}

bool Combat::row2empty()
{
    return ((m_arena[18].isInactive() || m_arena[18].isDead()) && (m_arena[19].isInactive() || m_arena[19].isDead()));
}

bool Combat::row3empty()
{
    return ((m_arena[20].isInactive() || m_arena[20].isDead()) && (m_arena[21].isInactive() || m_arena[21].isDead()));
}

bool Combat::targetable(int i)
{
    return (m_arena[i].isActive() && m_arena[i].isAlive());
}

int Combat::GetPartyTarget()
{
    int target = ERROR;
    int pickRow = Tool::getRand(0, 99);
    int pickSlot = Tool::getRand(0, 99);

    //std::cout << "Row: " << pickRow << " Slot: " << pickSlot << std::endl;

    if(!row1empty() && !row2empty() && !row3empty()) {
        if(pickRow < 10) { // Row 3
            // Pick from Row 3
            if(!targetable(21)) target = 20;
            else if(!targetable(20)) target = 21;
            else if(targetable(20) && targetable(21)) {
                if(pickSlot < 50) target = 20;
                else target = 21;
            }
        } else if (pickRow < 40) { // Row 2
            if(!targetable(18)) target = 19;
            else if(!targetable(19)) target = 18;
            else if(targetable(18) && targetable(19)) {
                if(pickSlot < 50) target = 18;
                else target = 19;
            }
        } else { // Row 1
            // Pick from Row 1
            if(targetable(15) && targetable(16) && targetable(17)) {
                if(pickSlot < 33) target = 15;
                else if(pickSlot < 66) target = 16;
                else target = 17;
            } else if(!targetable(15) && targetable(16) && targetable(17)) {
                if(pickSlot < 50) target = 16;
                else target = 17;
            } else if(targetable(15) && !targetable(16) && targetable(17)) {
                if(pickSlot < 50) target = 15;
                else target = 17;
            } else if(targetable(15) && targetable(16) && !targetable(17)) {
                if(pickSlot < 50) target = 15;
                else target = 16;
            } else if(!targetable(15) && !targetable(16) && targetable(17)) target = 17;
            else if(targetable(15) && !targetable(16) && !targetable(17)) target = 15;
            else target = 16;
        }
    }
    // Row 1 and 2 empty
    else if (row1empty() && row2empty()) {
        // Pick from Row 3
        if(!targetable(21)) target = 20;
        else if(!targetable(20)) target = 21;
        else if(targetable(20) && targetable(21)) {
            if(pickSlot < 50) target = 20;
            else target = 21;
        }
    }
    // Row 2 and 3 empty
    else if (row2empty() && row3empty()) {
        // Pick from Row 1
        if(targetable(15) && targetable(16) && targetable(17)) {
            if(pickSlot < 33) target = 15;
            else if(pickSlot < 66) target = 16;
            else target = 17;
        } else if(!targetable(15) && targetable(16) && targetable(17)) {
            if(pickSlot < 50) target = 16;
            else target = 17;
        } else if(targetable(15) && !targetable(16) && targetable(17)) {
            if(pickSlot < 50) target = 15;
            else target = 17;
        } else if(targetable(15) && targetable(16) && !targetable(17)) {
            if(pickSlot < 50) target = 15;
            else target = 16;
        } else if(!targetable(15) && !targetable(16) && targetable(17)) target = 17;
        else if(targetable(15) && !targetable(16) && !targetable(17)) target = 15;
        else target = 16;
    }
    // Row 1 and 3 empty
    else if (row1empty() && row3empty()) {
        if(!targetable(18)) target = 19;
        else if(!targetable(19)) target = 18;
        else if(targetable(18) && targetable(19)) {
            if(pickSlot < 50) target = 18;
            else target = 19;
        }
    } else if (!row1empty() && !row2empty() && row3empty()) {
        if(pickRow < 35) { // Row 2
            if(!targetable(18)) target = 19;
            else if(!targetable(19)) target = 18;
            else if(targetable(18) && targetable(19)) {
                if(pickSlot < 50) target = 18;
                else target = 19;
            }
        } else { // Row 1
            // Pick from Row 1
            if(targetable(15) && targetable(16) && targetable(17)) {
                if(pickSlot < 33) target = 15;
                else if(pickSlot < 66) target = 16;
                else target = 17;
            } else if(!targetable(15) && targetable(16) && targetable(17)) {
                if(pickSlot < 50) target = 16;
                else target = 17;
            } else if(targetable(15) && !targetable(16) && targetable(17)) {
                if(pickSlot < 50) target = 15;
                else target = 17;
            } else if(targetable(15) && targetable(16) && !targetable(17)) {
                if(pickSlot < 50) target = 15;
                else target = 16;
            } else if(!targetable(15) && !targetable(16) && targetable(17)) target = 17;
            else if(targetable(15) && !targetable(16) && !targetable(17)) target = 15;
            else target = 16;
        }
    } else if (!row1empty() && row2empty() && !row3empty()) {
        if(pickRow < 35) { // Row 3
            if(!targetable(21)) target = 20;
            else if(!targetable(20)) target = 21;
            else if(targetable(20) && targetable(21)) {
                if(pickSlot < 50) target = 20;
                else target = 21;
            }
        } else { // Row 1
            // Pick from Row 1
            if(targetable(15) && targetable(16) && targetable(17)) {
                if(pickSlot < 33) target = 15;
                else if(pickSlot < 66) target = 16;
                else target = 17;
            } else if(!targetable(15) && targetable(16) && targetable(17)) {
                if(pickSlot < 50) target = 16;
                else target = 17;
            } else if(targetable(15) && !targetable(16) && targetable(17)) {
                if(pickSlot < 50) target = 15;
                else target = 17;
            } else if(targetable(15) && targetable(16) && !targetable(17)) {
                if(pickSlot < 50) target = 15;
                else target = 16;
            } else if(!targetable(15) && !targetable(16) && targetable(17)) target = 17;
            else if(targetable(15) && !targetable(16) && !targetable(17)) target = 15;
            else target = 16;
        }
    } else if (row1empty() && !row2empty() && !row3empty()) {
        if(pickRow < 35) { // Row 3
            // Pick from Row 3
            if(!targetable(21)) target = 20;
            else if(!targetable(20)) target = 21;
            else if(targetable(20) && targetable(21)) {
                if(pickSlot < 50) target = 20;
                else target = 21;
            }
        } else { // Row 2
            if(!targetable(18)) target = 19;
            else if(!targetable(19)) target = 18;
            else if(targetable(18) && targetable(19)) {
                if(pickSlot < 50) target = 18;
                else target = 19;
            }
        }
    }
    return target;
}

void Combat::DrawCommandText()
{
    m_Boxes.getBox("Commands").getText().clear();
    m_Boxes.getBox("Commands").addText("Attack    Guard\n" + m_arena[m_action.actor].getSpecial() + "Order\nUse       Flee", sf::Vector2f(65, 20), 64);
}

/** \brief Populate the arena with appropriate enemies
 *
 * \param   std::string potentialEnemies        A string containing enemy indexes (ints) of enemies faced in the combat
 * \param   bool script = false                 Is this a scripted encounter (true) or random (false)?
 * \return  void
 *
 */
void Combat::PopulateEnemyParty(std::string potentialEnemies, bool script)
{
    // Populate Potential Enemy Vector with Monster IDs.
    std::stringstream ss(potentialEnemies);
    std::vector<int> enemyIDs;
    int temp;

    while(ss >> temp) enemyIDs.push_back(temp);

    //std::cout << "Difficulty: " << m_difficulty << "\t" << "PossibleIDs: ";
    //for(auto i : enemyIDs) std::cout << i << ", ";
    //std::cout << std::endl;

    if(enemyIDs.empty()) {
        std::cerr << "PopulateEnemyParty recieved a blank string." << std::endl;
        std::cerr << "  Combat Beacon May not have been found." << std::endl;
        std::cerr << "  Combat Beacon (Text Field 1) might be empty." << std::endl;
        std::cerr << "  Combat Beacon lists no possible enemys for the encounter." << std::endl;
        std::cerr << "  If scripted, e->getCurrentScriptCombat() may list no possible enemys for the encounter." << std::endl;
        e->PopState();
        return;
    }

    if(script) {
        for (unsigned int slot = 0; slot < enemyIDs.size(); slot++) LoadEnemy(slot, enemyIDs[slot]);
    } else {
        int random_index = 0;
        if(m_difficulty > 15) m_difficulty = 15;
        for(int n = 0; n < (int)m_difficulty; ++n) {
            random_index = Tool::getRand(0, enemyIDs.size() - 1);
            LoadEnemy(m_arrangements[(int)m_difficulty-1][n], enemyIDs[random_index]);
        }
    }
}

void Combat::LoadEnemy(int slot, int id)
{
    Enemy& enemy = e->m_Level.getEnemyRoster()[id];
    Hero temp(enemy.m_level, CLASS_WARRIOR);

    //std::cout << "Loading " << enemy.m_name << " (" << id << ") into slot " << slot << "." << std::endl;

    /// Header
    temp.setActive(true);
    temp.isMonster = true;
    temp.setName(enemy.m_name);
    temp.setTag(enemy.m_tag);
    /// Stats
    temp.setm_bonusAPhys(enemy.m_bonusAPhys);
    temp.setm_bonusAMagi(enemy.m_bonusAMagi);
    temp.setm_bonusAHeat(enemy.m_bonusAHeat);
    temp.setm_bonusACold(enemy.m_bonusACold);
    temp.setm_bonusAAcid(enemy.m_bonusAAcid);
    temp.setm_bonusAElec(enemy.m_bonusAElec);

    temp.setm_bonusDPhys(enemy.m_bonusDPhys);
    temp.setm_bonusDMagi(enemy.m_bonusDMagi);
    temp.setm_bonusDHeat(enemy.m_bonusDHeat);
    temp.setm_bonusDCold(enemy.m_bonusDCold);
    temp.setm_bonusDAcid(enemy.m_bonusDAcid);
    temp.setm_bonusDElec(enemy.m_bonusDElec);

    temp.setMaxHP((enemy.m_level * 8) + (enemy.m_level * 8 * enemy.m_bonusHp));

    /// Appearance
    temp.getSprite() = e->m_Level.getEnemyAvatars()[enemy.m_avatar];
    temp.getSprite().setScale(enemy.m_scale);
    temp.getSprite().setColor(enemy.m_color);
    temp.getSprite().setOrigin(temp.getSprite().getLocalBounds().width / 2, temp.getSprite().getLocalBounds().height);

    if(slot % NUM_ROWS == 2) temp.getSprite().setPosition(ROW_OFFSET + ENEMY_X - (ROW_WIDTH * (slot / NUM_ROWS)), ENEMY_Y - ROW_HEIGHT);
    if(slot % NUM_ROWS == 0) temp.getSprite().setPosition(ENEMY_X - (ROW_WIDTH * (slot / NUM_ROWS)), ENEMY_Y);
    if(slot % NUM_ROWS == 1) temp.getSprite().setPosition(ENEMY_X - (ROW_WIDTH * (slot / NUM_ROWS)) - ROW_OFFSET, ENEMY_Y + ROW_HEIGHT);

    //std::cout << "Placing " << id << " @ " << ENEMY_X - (ROW_WIDTH * (slot / NUM_ROWS)) << ", " << ENEMY_Y << std::endl;

    for(auto i : enemy.m_abilitiesOffensive) temp.getSpells().push_back(i);
    for(auto i : enemy.m_abilitiesDefensive) temp.getSpells().push_back(i);
    m_arena[slot] = temp;
}

void Combat::ResetValues()
{
    m_Boxes.getBoxes().clear();
    m_arena.clear();
    m_messages.clear();
    m_actionQueue.clear();
    m_arrowPos          = sf::Vector2f(56.0f, 865.0f);
    m_arrowScale        = sf::Vector2f(1.5, 1.5f);
    m_arrowDir          = Box::ARROW_RIGHT;
    m_action.action     = Action::COMMAND_ATTACK;
    m_phase             = PHASE_COMMAND;
    m_acceptingInput    = true;
    m_action            = Action();
    m_Fleeing           = false;
    m_canFlee           = true;
    m_round             = 1;
    m_arrangements.clear();
    m_resultsTimeStart  = sf::Time::Zero;
    m_resultsJustBegan  = true;
}

void Combat::PopulatePositionVector()
{
    m_arrangements.push_back(std::vector<int> {  6 });
    m_arrangements.push_back(std::vector<int> {  3,  9 });
    m_arrangements.push_back(std::vector<int> {  0,  6, 12 });
    m_arrangements.push_back(std::vector<int> {  4,  5, 10, 11 });
    m_arrangements.push_back(std::vector<int> {  0,  7,  8, 13, 14 });
    m_arrangements.push_back(std::vector<int> {  1,  2,  7,  8, 13, 14 });
    m_arrangements.push_back(std::vector<int> {  0,  4,  5,  6, 10, 11, 12 });
    m_arrangements.push_back(std::vector<int> {  1,  2,  4,  5,  7,  8, 10, 11 });
    m_arrangements.push_back(std::vector<int> {  0,  4,  5,  7,  8, 10, 11, 13, 14 });
    m_arrangements.push_back(std::vector<int> {  1,  2,  4,  5,  7,  8, 10, 11, 13, 14 });
    m_arrangements.push_back(std::vector<int> {  1,  2,  4,  5,  6,  7,  8, 10, 11, 13, 14 });
    m_arrangements.push_back(std::vector<int> {  1,  2,  3,  4,  5,  7,  8,  9, 10, 11, 13, 14 });
    m_arrangements.push_back(std::vector<int> {  0,  1,  2,  3,  4,  5,  7,  8,  9, 10, 11, 13, 14 });
    m_arrangements.push_back(std::vector<int> {  0,  1,  2,  3,  4,  5,  6,  7,  8,  9, 10, 11, 13, 14 });
    m_arrangements.push_back(std::vector<int> {  0,  1,  2,  3,  4,  5,  6,  7,  8,  9, 10, 11, 12, 13, 14 });
}

void Combat::DoStanceChange(float ratioCompletion)
{
    sf::Sprite& spriteActor  =  m_arena[m_action.actor].getSprite();
    sf::Sprite& spriteTarget = m_arena[m_action.target].getSprite();
    sf::Color colorActor  =  spriteActor.getColor();
    sf::Color colorTarget = spriteTarget.getColor();
    // m_stanceSwitchPhaseOne
    if (ratioCompletion < 0.50f && m_stanceSwitchPhaseOne) {
        spriteActor.setColor(sf::Color(colorActor.r, colorActor.g, colorActor.b, 256 - (ratioCompletion * 512)));
        spriteTarget.setColor(sf::Color(colorTarget.r, colorTarget.g, colorTarget.b, 256 - (ratioCompletion * 512)));
    } else if (ratioCompletion > 0.50f && m_stanceSwitchPhaseOne) {

        m_stanceSwitchPhaseOne = false;
        // Modify the swap target's Action.actor to stay with him in his new position.
        std::cout << "Swap information: " << m_actionQueue.size() << std::endl;
        //m_arena[m_action.target] m_actionQueue.size() << std::endl;
        int n = 0;
        for(auto& a : m_actionQueue) {
            n++;
            if(a.actor == m_action.target) {
                a.actor = m_action.actor;
                std::cout << "Switch Target found and altered: Queue #" << n << " changed from " << m_action.target << " to " << m_action.actor << std::endl;
            }
        }
        // Swap them
        std::swap(m_arena[m_action.actor], m_arena[m_action.target]);
        // Also swap the actual party, not just the combat copy
        std::vector<Hero>& party = e->getParty().getMembers();
        std::swap(party[m_action.actor - 15], party[m_action.target - 15]);

        sf::Vector2f tempPos = m_arena[m_action.actor].getSprite().getPosition();
        m_arena[m_action.actor].getSprite().setPosition(m_arena[m_action.target].getSprite().getPosition());
        m_arena[m_action.target].getSprite().setPosition(tempPos);
    }

    if (ratioCompletion > 0.50f && !m_stanceSwitchPhaseOne) {
        spriteActor.setColor(sf::Color(colorActor.r, colorActor.g, colorActor.b, 0 + ( (ratioCompletion - 0.5) * 512)));
        spriteTarget.setColor(sf::Color(colorTarget.r, colorTarget.g, colorTarget.b, 0 + ( (ratioCompletion - 0.5) * 512)));
    }
}

bool Combat::AllPartyMembersAreDead()
{
    bool LivePartyMember = false;
    for(auto p : e->getParty().getMembers()) if (p.isAlive()) LivePartyMember = true;
    if (!LivePartyMember) {
        std::clog << "Combat Skipped: All Party Members Dead." << std::endl;
        return true;
    }
    return false;
}

void Combat::DoSpecial()
{
    Effect damage;

    Hero& actor = m_arena[m_action.actor];
    //Hero& target = m_arena[m_action.target];
    /// All Spells, Tricks, Prayers and Shouts are specials.
    std::cout << "Executed Special: m_action.sub_action: " << m_action.sub_action << std::endl;

    //EffectsPlace(m_action.target, "Special!", sf::Color::Red);

    /// 1.) Compose Effect
    /// 2.) Submit to target
    /// 3.) Report that damage to the message box
    /// 4.) Place the value into the m_Text queue

    switch(m_action.sub_action) {
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
        if(actor.getCurMP() < MP_COST::SPELL_MISSILE) m_messages.push_back(StatusMessage(actor.getName() + " doesn't have enough mana."));
        // else if (target.isDead()) m_messages.push_back(StatusMessage(actor.getName() + "'s missile is too late, as " + target.getName() + " is already dead."));
        else {
            actor.setCurMP(actor.getCurMP() - MP_COST::SPELL_MISSILE);
            damage.magic    = 1.0f;
            SpellDamage(damage, sf::Color::Cyan, "'s missiles zaps ");
        }
        break;
    case Special::SPELL_LIGHTNING_BOLT:
        if(actor.getCurMP() < MP_COST::SPELL_LIGHTNING_BOLT) m_messages.push_back(StatusMessage(actor.getName() + " doesn't have enough mana."));
        else {
            actor.setCurMP(actor.getCurMP() - MP_COST::SPELL_LIGHTNING_BOLT);
            damage.magic    = 0.5f;
            damage.electric = 0.5f;
            SpellDamage(damage, sf::Color::Yellow, "'s lightning rips into ");
        }
        break;
    case Special::SPELL_BURN:
        if(actor.getCurMP() < MP_COST::SPELL_BURN) m_messages.push_back(StatusMessage(actor.getName() + " doesn't have enough mana."));
        else {
            actor.setCurMP(actor.getCurMP() - MP_COST::SPELL_BURN);
            damage.magic    = 0.5f;
            damage.heat     = 0.5f;
            SpellDamage(damage, sf::Color::Red, " burns ");
        }
        break;
    case Special::SPELL_FIREBALL:
        if(actor.getCurMP() < MP_COST::SPELL_FIREBALL) m_messages.push_back(StatusMessage(actor.getName() + " doesn't have enough mana."));
        else {
            actor.setCurMP(actor.getCurMP() - MP_COST::SPELL_FIREBALL);
            damage.magic    = 0.5f;
            damage.heat     = 0.5f;
            SpellDamage(damage, sf::Color::Red, " explodes around ");
        }
        break;
    case Special::SPELL_GUSH_WATER:
        if(actor.getCurMP() < MP_COST::SPELL_GUSH_WATER) m_messages.push_back(StatusMessage(actor.getName() + " doesn't have enough mana."));
        else {
            actor.setCurMP(actor.getCurMP() - MP_COST::SPELL_GUSH_WATER);
            damage.magic    = 0.5f;
            damage.cold     = 0.5f;
            SpellDamage(damage, sf::Color::Blue, " gush of water engulfs ");
        }
        break;
    case Special::SPELL_EARTHQUAKE:
        if(actor.getCurMP() < MP_COST::SPELL_EARTHQUAKE) m_messages.push_back(StatusMessage(actor.getName() + " doesn't have enough mana."));
        else {
            actor.setCurMP(actor.getCurMP() - MP_COST::SPELL_EARTHQUAKE);
            damage.magic    = 0.5f;
            damage.physical = 0.5f;
            SpellDamageAoE(damage, sf::Color(222,184,135), " opens the Earth to swallow ");
        }
        break;
    case Special::SPELL_FIRE_STORM:
        if(actor.getCurMP() < MP_COST::SPELL_EARTHQUAKE) m_messages.push_back(StatusMessage(actor.getName() + " doesn't have enough mana."));
        else {
            actor.setCurMP(actor.getCurMP() - MP_COST::SPELL_EARTHQUAKE);
            damage.magic    = 0.5f;
            damage.heat     = 0.5f;
            SpellDamageAoE(damage, sf::Color(255,69,0), " calls down a storm of fire on "); // orangered
        }
        break;
    case Special::SPELL_LIGHTNING_STORM:
        if(actor.getCurMP() < MP_COST::SPELL_LIGHTNING_STORM) m_messages.push_back(StatusMessage(actor.getName() + " doesn't have enough mana."));
        else {
            actor.setCurMP(actor.getCurMP() - MP_COST::SPELL_LIGHTNING_STORM);
            damage.magic    = 0.5f;
            damage.electric = 0.5f;
            SpellDamageAoE(damage, sf::Color::Yellow, " draws a lightning storm down upon ");
        }
        break;
    case Special::SPELL_SNOW_STORM:
        if(actor.getCurMP() < MP_COST::SPELL_SNOW_STORM) m_messages.push_back(StatusMessage(actor.getName() + " doesn't have enough mana."));
        else {
            actor.setCurMP(actor.getCurMP() - MP_COST::SPELL_SNOW_STORM);
            damage.magic    = 0.5f;
            damage.cold     = 0.5f;
            SpellDamageAoE(damage, sf::Color(173,216,230), " calls upon wind and ice, freezing "); // lightblue
        }
        break;
    case Special::SPELL_FLOOD:
        if(actor.getCurMP() < MP_COST::SPELL_FLOOD) m_messages.push_back(StatusMessage(actor.getName() + " doesn't have enough mana."));
        else {
            actor.setCurMP(actor.getCurMP() - MP_COST::SPELL_FLOOD);
            damage.magic    = 0.3f;
            damage.cold     = 0.3f;
            damage.physical = 0.4f;
            SpellDamageAoE(damage, sf::Color(30,144,255), " submerges "); // dodgerblue
        }
        break;
    case Special::SPELL_VOLCANO:
        if(actor.getCurMP() < MP_COST::SPELL_VOLCANO) m_messages.push_back(StatusMessage(actor.getName() + " doesn't have enough mana."));
        else {
            actor.setCurMP(actor.getCurMP() - MP_COST::SPELL_VOLCANO);
            damage.magic    = 0.3f;
            damage.physical = 0.3f;
            damage.heat     = 0.4f;
            SpellDamageAoE(damage, sf::Color(255,140,0), " causes a mountain to erupt under ");
        }
        break;
    case Special::SPELL_ASTEROID:
        if(actor.getCurMP() < MP_COST::SPELL_ASTEROID) m_messages.push_back(StatusMessage(actor.getName() + " doesn't have enough mana."));
        else {
            actor.setCurMP(actor.getCurMP() - MP_COST::SPELL_ASTEROID);
            damage.magic    = 0.2f;
            damage.physical = 0.5f;
            damage.heat     = 0.3f;
            SpellDamageAoE(damage, sf::Color(255,105,180), " draws a massive star down on "); // hotpink
        }
        break;
    ///[======================]///
    ///[      Statuses        ]///
    ///[======================]///
    case Special::SPELL_SLOW:
        break;
    case Special::SPELL_HASTE:
        break;
    case Special::SPELL_PETRIFY:
        break;

    // Cleric
    case Special::SPELL_HEAL:
        SpellHeal();
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
//        if(actor.getCurMP() < 3) m_messages.push_back(StatusMessage(actor.getName() + " doesn't have enough mana to cast regen."));
//        else if (target.isDead()) m_messages.push_back(StatusMessage(actor.getName() + "'s regen is too late, as " + target.getName() + " is already dead."));
//        else
//        {
//            actor.setCurMP(actor.getCurMP() - 3.0f);
//            ss << actor.getName() << " casts regenerate on " << target.getName() << ".";
//            m_messages.push_back(StatusMessage(ss.str()));
//            target.TakeStatus(Status(STATUS_REGEN, spellpower, 3));
//        }
        break;
    case Special::SPELL_POISON:
        break;
    case Special::SPELL_RESURRECT:
//        if(actor.getCurMP() < 8) m_messages.push_back(StatusMessage(actor.getName() + " doesn't have enough mana to cast resurrect."));
//        else if (target.isAlive()) m_messages.push_back(StatusMessage(actor.getName() + "'s resurrect is premature, as " + target.getName() + " is not dead... yet."));
//        else
//        {
//            float amount = (target.getMaxHP() / 10.0f) + 1;
//            actor.setCurMP(actor.getCurMP() - 8.0f);
//            ss << actor.getName() << " raises " << target.getName() << " from death, miraculous!";
//            //std::cout << "Resurrecting: " << target.getName() << std::endl;
//            target.getSprite().setTextureRect(sf::IntRect(target.getTextureSheetBase().x + (1 * TILE_SIZE_X), target.getTextureSheetBase().y + (1 * TILE_SIZE_Y), TILE_SIZE_X, TILE_SIZE_Y));
//            m_messages.push_back(StatusMessage(ss.str()));
//            m_arena[m_action.target].TakeHeal(amount);
//        }
        break;
    case Special::SPELL_BLESS_ALL:
        break;
    case Special::SPELL_HEAL_ALL:
        SpellHeal(true);
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
}

void Combat::SpellHeal(bool AoE)
{
    Hero& actor = m_arena[m_action.actor];
    Hero& target = m_arena[m_action.target];
    if (!AoE) {
        if(actor.getCurMP() < MP_COST::SPELL_HEAL) m_messages.push_back(StatusMessage(actor.getName() + " doesn't have enough mana."));
        else if (target.isDead()) m_messages.push_back(StatusMessage(actor.getName() + "'s heal is too late, as " + target.getName() + " is already dead."));
        else {
            // Charge the mana cost
            actor.setCurMP(actor.getCurMP() - MP_COST::SPELL_HEAL);
            // Figure the amount healed
            //float healed = std::max(actor.getAtkMagi(), target.getMaxHP() - target.getCurHP());
            // Apply the heal, return the actual amount healed
            float healed = target.TakeHeal(actor.getAtkMagi());
            // Announce the heal
            ReportDamage(healed, m_action.target, " heals ");
            // Show the heal effect
            std::stringstream ss;
            ss << healed;
            EffectsPlace(m_action.target, ss.str(), sf::Color::Green);
        }
    } else {
        if(actor.getCurMP() < MP_COST::SPELL_HEAL_ALL) m_messages.push_back(StatusMessage(actor.getName() + " doesn't have enough mana."));
        else {
            // Charge the mana cost
            actor.setCurMP(actor.getCurMP() - MP_COST::SPELL_HEAL_ALL);
            /// Which group? Enemies or allies?
            if(m_action.target < 15) {
                /// Heal Enemy Party
                std::stringstream ss;
                ss << m_arena[m_action.actor].getName() << " heals the enemy group for " << (int)actor.getAtkMagi() << " health!?";
                m_messages.push_back(StatusMessage(ss.str()));
                for(int i = 0; i < 15; ++i) {
                    if(m_arena[i].isInactive() || m_arena[i].isDead()) continue;
                    float healed = m_arena[i].TakeHeal(actor.getAtkMagi());
                    std::stringstream ss;
                    ss << healed;
                    EffectsPlace(i, ss.str(), sf::Color::Green);
                }
            } else {
                /// Heal Good Guys
                std::stringstream ss;
                ss << m_arena[m_action.actor].getName() << " heals the party for " << (int)actor.getAtkMagi() << " health.";
                m_messages.push_back(StatusMessage(ss.str()));
                for(int i = 15; i < 23; ++i) {
                    if(m_arena[i].isInactive() || m_arena[i].isDead()) continue;
                    float healed = m_arena[i].TakeHeal(actor.getAtkMagi());
                    std::stringstream ss;
                    ss << healed;
                    EffectsPlace(i, ss.str(), sf::Color::Green);
                }
            }
        }
    }
}

void Combat::DoAttack()
{
    Effect damage = m_arena[m_action.actor].DealMeleeDamage();

    float absoluteDamage = m_arena[m_action.target].TakeDamage(damage);
    ReportDamage(absoluteDamage, m_action.target);

    //if(m_arena[m_action.target].isDead()) DoDeath(m_arena[m_action.target]);

    std::stringstream ss;
    ss << absoluteDamage;
    EffectsPlace(m_action.target, ss.str(), sf::Color::White);
}

void Combat::SpellDamageAoE(Effect dam, sf::Color col, std::string desc)
{
    Effect damage = m_arena[m_action.actor].DealMagicDamage(dam);

    for(int i = 0; i < 15; ++i) {
        if(m_arena[i].isAlive() && m_arena[i].isActive()) {
            float absoluteDamage = m_arena[i].TakeDamage(damage);
            ReportDamage(absoluteDamage, i, desc);

            //if(m_arena[i].isDead()) DoDeath(m_arena[i]);

            std::stringstream ss;
            ss << absoluteDamage;
            EffectsPlace(i, ss.str(), col);

        }
    }
}

void Combat::SpellDamage(Effect dam, sf::Color col, std::string desc)
{
    Effect damage = m_arena[m_action.actor].DealMagicDamage(dam);

    float absoluteDamage = m_arena[m_action.target].TakeDamage(damage);
    ReportDamage(absoluteDamage, m_action.target, desc);

    //if(m_arena[m_action.target].isDead()) DoDeath(m_arena[m_action.target]);

    std::stringstream ss;
    ss << absoluteDamage;
    EffectsPlace(m_action.target, ss.str(), col);
}
