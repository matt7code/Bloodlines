#include "states/edit_beacon.h"

EditBeacon EditBeacon::m_EditBeacon;

void EditBeacon::Init(Engine* e) {
    m_Boxes.getBoxes().clear();

    m_Boxes.addBox("Body", sf::Vector2f(1520, 680), sf::Vector2f(200, 200));
    m_Boxes.addBox("BodyLoc", sf::Vector2f(1200, 75), sf::Vector2f(360, 150));

    m_Boxes.addBox("Erase", sf::Vector2f(200, 50), sf::Vector2f(250, 300), Box::BOX_PLAIN, sf::Color::Red);
    m_Boxes.getBox("Erase").addText("Erase", sf::Vector2f(56, 0), 36, Box::FONT_NORMAL, sf::Color::Black, 0);

    m_Boxes.addBox("Inactive", sf::Vector2f(200, 50), sf::Vector2f(555, 300), Box::BOX_PLAIN, sf::Color::Yellow);
    m_Boxes.getBox("Inactive").addText("Inactive", sf::Vector2f(29, 0), 36, Box::FONT_NORMAL, sf::Color::Black, 0);

    m_Boxes.addBox("Combat", sf::Vector2f(200, 50), sf::Vector2f(860, 300), Box::BOX_PLAIN, sf::Color::Yellow);
    m_Boxes.getBox("Combat").addText("Combat", sf::Vector2f(48, 0), 36, Box::FONT_NORMAL, sf::Color::Black, 0);

    m_Boxes.addBox("Search", sf::Vector2f(200, 50), sf::Vector2f(1165, 300), Box::BOX_PLAIN, sf::Color::Yellow);
    m_Boxes.getBox("Search").addText("Search", sf::Vector2f(44, 0), 36, Box::FONT_NORMAL, sf::Color::Black, 0);

    m_Boxes.addBox("Location", sf::Vector2f(200, 50), sf::Vector2f(1470, 300), Box::BOX_PLAIN, sf::Color::Yellow);
    m_Boxes.getBox("Location").addText("Location", sf::Vector2f(31, 0), 36, Box::FONT_NORMAL, sf::Color::Black, 0);

    m_Boxes.addBox("val1", sf::Vector2f(200, 50), sf::Vector2f( 250, 375), Box::BOX_PLAIN, sf::Color::Black);
    m_Boxes.addBox("val2", sf::Vector2f(200, 50), sf::Vector2f( 555, 375), Box::BOX_PLAIN, sf::Color::Black);
    m_Boxes.addBox("val3", sf::Vector2f(200, 50), sf::Vector2f( 860, 375), Box::BOX_PLAIN, sf::Color::Black);
    m_Boxes.addBox("val4", sf::Vector2f(200, 50), sf::Vector2f(1165, 375), Box::BOX_PLAIN, sf::Color::Black);
    m_Boxes.addBox("val5", sf::Vector2f(200, 50), sf::Vector2f(1470, 375), Box::BOX_PLAIN, sf::Color::Black);

    m_Boxes.addBox("tex1", sf::Vector2f(1420, 150), sf::Vector2f(250, 450), Box::BOX_PLAIN, sf::Color(128, 128, 128, 128));
    m_Boxes.addBox("tex2", sf::Vector2f(1420, 150), sf::Vector2f(250, 635), Box::BOX_PLAIN, sf::Color(128, 128, 128, 128));

    m_screenMap[0] = sf::IntRect( 250, 300, 200, 50);
    m_screenMap[1] = sf::IntRect( 555, 300, 200, 50);
    m_screenMap[2] = sf::IntRect( 860, 300, 200, 50);
    m_screenMap[3] = sf::IntRect(1165, 300, 200, 50);
    m_screenMap[4] = sf::IntRect(1470, 300, 200, 50);

    m_screenMap[5] = sf::IntRect( 250, 375, 200, 50);
    m_screenMap[6] = sf::IntRect( 555, 375, 200, 50);
    m_screenMap[7] = sf::IntRect( 860, 375, 200, 50);
    m_screenMap[8] = sf::IntRect(1165, 375, 200, 50);
    m_screenMap[9] = sf::IntRect(1470, 375, 200, 50);

    m_screenMap[10] = sf::IntRect(250, 450, 1420, 150);
    m_screenMap[11] = sf::IntRect(250, 635, 1420, 150);

    BeaconPtr = nullptr;
    BeaconPtr = getNearestBeacon(e);
    if (BeaconPtr != nullptr) {
        currType = BeaconPtr->getType();
        std::stringstream ss;
        ss << BeaconPtr->getValue1();
        ss >> m_t5;
        ss.str(std::string());
        ss.clear();
        ss << BeaconPtr->getValue2();
        ss >> m_t6;
        ss.str(std::string());
        ss.clear();
        ss << BeaconPtr->getValue3();
        ss >> m_t7;
        ss.str(std::string());
        ss.clear();
        ss << BeaconPtr->getValue4();
        ss >> m_t8;
        ss.str(std::string());
        ss.clear();
        ss << BeaconPtr->getValue5();
        ss >> m_t9;
        ss.str(std::string());
        ss.clear();
        m_t10 = BeaconPtr->getText1();
        m_t11 = BeaconPtr->getText2();
        e->BeaconPos = BeaconPtr->getPosition();
    } else {
        currType = Beacon::INACTIVE;
        m_t5 = "0";
        m_t6 = "0";
        m_t7 = "0";
        m_t8 = "0";
        m_t9 = "0";
        m_t10 = "";
        m_t11 = "";
        m_clickPos = e->BeaconPos;
    }

    std::stringstream ss;
    ss << "Beacon Location: " << e->BeaconPos.x << "x, " << e->BeaconPos.y << "y, " << e->BeaconPos.z << "z";
    m_Boxes.getBox("BodyLoc").addText(ss.str(), sf::Vector2f(10, 0), 48);

    m_Boxes.addBox("Hint", sf::Vector2f(0, 0), sf::Vector2f(260, 800), Box::BOX_PLAIN);

    UpdateBoxes();
}

Beacon* EditBeacon::getNearestBeacon(Engine* e) {
    Beacon *i = nullptr, *c = nullptr, *s = nullptr, *l = nullptr;
    float c_dist = 10000000.0f, s_dist = 10000000.0f, l_dist = 10000000.0f, i_dist = 10000000.0f;

    i = e->m_Level.getNearestInactiveBeacon(e->BeaconPos);
    if(i) i_dist = Tool::dist(i->getPosition(), e->BeaconPos);

    c = e->m_Level.getNearestCombatBeacon(e->BeaconPos);
    if(c) c_dist = Tool::dist(c->getPosition(), e->BeaconPos);

    s = e->m_Level.getNearestSearchBeacon(e->BeaconPos);
    if (s) s_dist = Tool::dist(s->getPosition(), e->BeaconPos);

    l = e->m_Level.getNearestLocationBeacon(e->BeaconPos);
    if (l) l_dist = Tool::dist(l->getPosition(), e->BeaconPos);

    if (c_dist < s_dist &&
            c_dist < l_dist &&
            c_dist < i_dist) {
        if(c_dist < 32) return c;
    } else if (s_dist < c_dist &&
               s_dist < l_dist &&
               s_dist < i_dist) {
        if(s_dist < 32) return s;
    } else if (l_dist < c_dist &&
               l_dist < s_dist &&
               l_dist < i_dist) {
        if(l_dist < 32) return l;
    } else if (i_dist < c_dist &&
               i_dist < s_dist &&
               i_dist < l_dist) {
        if(i_dist < 32) return i;
    }
    return nullptr;
}

void EditBeacon::Cleanup(Engine* e) {
    e->m_Level.SaveBeacons(e->m_Level.getName());
    e->m_Level.LoadBeacons(e->m_Level.getName());
}

void EditBeacon::HandleEvents(Engine* e) {
    sf::Event event;
    while (e->m_RenderWindow.pollEvent(event)) {
        // Close window : exit
        if (event.type == sf::Event::TextEntered && event.text.unicode < 128) {
            if(event.text.unicode == 9) { // Tab
                m_choice++;
                if(m_choice > 11) m_choice = 5;
                HandleChoice(e);
            } else {

                if (m_5) {
                    if(event.text.unicode == 8) {
                        if(!m_t5.empty()) m_t5.resize(m_t5.size() - 1);
                    } else m_t5 += static_cast<char>(event.text.unicode);
                }
                if (m_6) {
                    if(event.text.unicode == 8) {
                        if(!m_t6.empty()) m_t6.resize(m_t6.size() - 1);
                    } else m_t6 += static_cast<char>(event.text.unicode);
                }
                if (m_7) {
                    if(event.text.unicode == 8) {
                        if(!m_t7.empty()) m_t7.resize(m_t7.size() - 1);
                    } else m_t7 += static_cast<char>(event.text.unicode);
                }
                if (m_8) {
                    if(event.text.unicode == 8) {
                        if(!m_t8.empty()) m_t8.resize(m_t8.size() - 1);
                    } else m_t8 += static_cast<char>(event.text.unicode);
                }
                if (m_9) {
                    if(event.text.unicode == 8) {
                        if(!m_t9.empty()) m_t9.resize(m_t9.size() - 1);
                    } else m_t9 += static_cast<char>(event.text.unicode);
                }
                if (m_10) {
                    if(event.text.unicode == 8) {
                        if(!m_t10.empty()) m_t10.resize(m_t10.size() - 1);
                    } else m_t10 += static_cast<char>(event.text.unicode);
                }
                if (m_11) {
                    if(event.text.unicode == 8) {
                        if(!m_t11.empty()) m_t11.resize(m_t11.size() - 1);
                    } else m_t11 += static_cast<char>(event.text.unicode);
                }
                UpdateBoxes();
            }
        }
        if (event.type == sf::Event::Closed) {
            e->Quit();
        }
        if (event.type == sf::Event::KeyPressed) {
            switch(event.key.code) {
            case sf::Keyboard::Escape:
                e->PopState();
                break;

            case sf::Keyboard::Return:
                SaveBeacon(e);
                e->PopState();
                break;

            default:
                break;
            }
        }
        if (event.type == sf::Event::MouseButtonPressed) {
            sf::Vector2i click = ConvertClickToScreenPos(sf::Vector2i(event.mouseButton.x, event.mouseButton.y), e->m_RenderWindow);
            m_choice = GetChoice(click);
            HandleChoice(e);
        }
    }
}

void EditBeacon::Update(Engine * e) {
    e->m_View.setCenter(sf::Vector2f(e->BeaconPos.x, e->BeaconPos.y));
    e->m_Level.Update(e->getTimeRunning(), sf::Vector2f(e->BeaconPos.x, e->BeaconPos.y), e->viewExtents);
}

void EditBeacon::Draw(Engine * e) {
    e->m_RenderWindow.setView(e->m_View);
    e->m_Level.Draw(e->m_RenderWindow);
    e->getParty().Draw(e->m_RenderWindow);
    e->m_RenderWindow.setView(e->m_ViewBase);

    m_Boxes.DrawBoxes(e->m_RenderWindow);

    // HighLight
    sf::RectangleShape typeBox;
    typeBox.setOutlineColor(sf::Color::Green);
    typeBox.setFillColor(sf::Color::Transparent);
    typeBox.setOutlineThickness(8);
    typeBox.setSize(sf::Vector2f(200, 50));
    typeBox.setPosition(sf::Vector2f(250 + (currType * 305), 300));
    e->m_RenderWindow.draw(typeBox);
}

sf::Vector2i EditBeacon::ConvertClickToScreenPos(sf::Vector2i click, sf::RenderWindow& window) {
    return sf::Vector2i((1920.0f / window.getSize().x) * click.x, (1080.0f / window.getSize().y) * click.y);
}

int EditBeacon::GetChoice(sf::Vector2i click) {
    for (auto it = m_screenMap.begin(); it != m_screenMap.end(); ++it) {
        if(it->second.contains(click)) return it->first;
    }
    return -1;
}

void EditBeacon::HandleChoice(Engine *e) {
    m_Boxes.getBox("Hint").getText().clear();
    if (m_choice == -1) return;
    if (m_choice == 0) {
        if(BeaconPtr) BeaconPtr->setType(Beacon::NONE);
        e->PopState();
    }
    if (m_choice == 1) currType = Beacon::INACTIVE;
    if (m_choice == 2) currType = Beacon::COMBAT;
    if (m_choice == 3) currType = Beacon::SEARCH;
    if (m_choice == 4) currType = Beacon::LOCATION;

    if (m_choice > 4) m_5 = m_6 = m_7 = m_8 = m_9 = m_10 = m_11 = false;
    const int text_size = 24;

    if (m_choice == 5) {
        if(currType == Beacon::COMBAT) m_Boxes.getBox("Hint").addText("Radius of the effected area.", sf::Vector2f(0, 0), text_size);
        if(currType == Beacon::SEARCH) m_Boxes.getBox("Hint").addText("Radius within which the search goal can be discovered.", sf::Vector2f(0, 0), text_size);
        if(currType == Beacon::LOCATION) m_Boxes.getBox("Hint").addText("Radius of the area.", sf::Vector2f(0, 0), text_size);
        m_5 = true;
    }
    if (m_choice == 6) {
        if(currType == Beacon::COMBAT) m_Boxes.getBox("Hint").addText("Frequency of combat out of 1000 steps.", sf::Vector2f(0, 0), text_size);
        if(currType == Beacon::SEARCH) m_Boxes.getBox("Hint").addText("ID of item to be found.", sf::Vector2f(0, 0), text_size);
        if(currType == Beacon::LOCATION) m_Boxes.getBox("Hint").addText("Background ID used for combat.\n0-None 1-Dungeon 2-Forest 3-Plain 4-Town 5-Underwater 6-Ocean 7-Castle 8-Cabin", sf::Vector2f(0, 0), text_size);
        m_6 = true;
    }
    if (m_choice == 7) {
        if(currType == Beacon::COMBAT) m_Boxes.getBox("Hint").addText("Difficulty of combat in levels.", sf::Vector2f(0, 0), text_size);
        if(currType == Beacon::SEARCH) m_Boxes.getBox("Hint").addText("Amount of items to be found.", sf::Vector2f(0, 0), text_size);
        if(currType == Beacon::LOCATION) m_Boxes.getBox("Hint").addText("(TODO) Weather/Overlay ID of the area.", sf::Vector2f(0, 0), text_size);
        m_7 = true;
    }
    if (m_choice == 8) {
        if(currType == Beacon::COMBAT) m_Boxes.getBox("Hint").addText("A multiplier applied to the amound of gold recieved.", sf::Vector2f(0, 0), text_size);
        if(currType == Beacon::SEARCH) m_Boxes.getBox("Hint").addText("Quality of item or items found.", sf::Vector2f(0, 0), text_size);
        if(currType == Beacon::LOCATION) m_Boxes.getBox("Hint").addText("(TODO) Current health change per step.", sf::Vector2f(0, 0), text_size);
        m_8 = true;
    }
    if (m_choice == 9) {
        if(currType == Beacon::COMBAT) m_Boxes.getBox("Hint").addText("A multiplier applied to the amound of experience recieved.", sf::Vector2f(0, 0), text_size);
        if(currType == Beacon::SEARCH) m_Boxes.getBox("Hint").addText("The number of times the item can be discovered (0 for infinite).", sf::Vector2f(0, 0), text_size);
        if(currType == Beacon::LOCATION) m_Boxes.getBox("Hint").addText("(TODO) Current magic change per step.", sf::Vector2f(0, 0), text_size);
        m_9 = true;
    }
    if (m_choice == 10) {
        if(currType == Beacon::COMBAT) m_Boxes.getBox("Hint").addText("An optional description of the combat encounter.", sf::Vector2f(0, 0), text_size);
        if(currType == Beacon::SEARCH) m_Boxes.getBox("Hint").addText("The search message returned when the item is discovered.", sf::Vector2f(0, 0), text_size);
        if(currType == Beacon::LOCATION) m_Boxes.getBox("Hint").addText("The name of the location.", sf::Vector2f(0, 0), text_size);
        m_10 = true;
    }
    if (m_choice == 11) {
        if(currType == Beacon::COMBAT) m_Boxes.getBox("Hint").addText("An optional description of the combat encounter.", sf::Vector2f(0, 0), text_size);
        if(currType == Beacon::SEARCH) m_Boxes.getBox("Hint").addText("The search message returned after the item has been exhausted.", sf::Vector2f(0, 0), text_size);
        if(currType == Beacon::LOCATION) m_Boxes.getBox("Hint").addText("The larger country or zone name of the location.", sf::Vector2f(0, 0), text_size);
        m_11 = true;
    }

    UpdateBoxes();
}

void EditBeacon::UpdateBoxes() {
    m_Boxes.getBox("val1").getText().clear();
    m_Boxes.getBox("val2").getText().clear();
    m_Boxes.getBox("val3").getText().clear();
    m_Boxes.getBox("val4").getText().clear();
    m_Boxes.getBox("val5").getText().clear();
    m_Boxes.getBox("tex1").getText().clear();
    m_Boxes.getBox("tex2").getText().clear();

    m_Boxes.getBox("val1").addText(m_t5, sf::Vector2f(10, 0), 36);
    m_Boxes.getBox("val2").addText(m_t6, sf::Vector2f(10, 0), 36);
    m_Boxes.getBox("val3").addText(m_t7, sf::Vector2f(10, 0), 36);
    m_Boxes.getBox("val4").addText(m_t8, sf::Vector2f(10, 0), 36);
    m_Boxes.getBox("val5").addText(m_t9, sf::Vector2f(10, 0), 36);
    m_Boxes.getBox("tex1").addText(m_t10, sf::Vector2f(10, 0), 36);
    m_Boxes.getBox("tex2").addText(m_t11, sf::Vector2f(10, 0), 36);
}

void EditBeacon::SaveBeacon(Engine* e) {
    if (BeaconPtr) {
        BeaconPtr->setType(currType);

        float n;
        std::stringstream ss;
        ss << m_t5;
        ss >> n;
        BeaconPtr->setValue1(n);

        ss.str(std::string());
        ss.clear();
        ss << m_t6;
        ss >> n;
        BeaconPtr->setValue2(n);

        ss.str(std::string());
        ss.clear();
        ss << m_t7;
        ss >> n;
        BeaconPtr->setValue3(n);

        ss.str(std::string());
        ss.clear();
        ss << m_t8;
        ss >> n;
        BeaconPtr->setValue4(n);

        ss.str(std::string());
        ss.clear();
        ss << m_t9;
        ss >> n;
        BeaconPtr->setValue5(n);

        BeaconPtr->setText1(m_t10);
        BeaconPtr->setText2(m_t11);
        BeaconPtr->setPosition(e->BeaconPos);
    } else {
        Beacon newBeacon;
        newBeacon.setType(currType);
        float n;
        std::stringstream ss;
        ss << m_t5;
        ss >> n;
        newBeacon.setValue1(n);

        ss.str(std::string());
        ss.clear();
        ss << m_t6;
        ss >> n;
        newBeacon.setValue2(n);

        ss.str(std::string());
        ss.clear();
        ss << m_t7;
        ss >> n;
        newBeacon.setValue3(n);

        ss.str(std::string());
        ss.clear();
        ss << m_t8;
        ss >> n;
        newBeacon.setValue4(n);

        ss.str(std::string());
        ss.clear();
        ss << m_t9;
        ss >> n;
        newBeacon.setValue5(n);

        newBeacon.setText1(m_t10);
        newBeacon.setText2(m_t11);
        newBeacon.setPosition(e->BeaconPos);

        switch (currType) {
        case Beacon::INACTIVE:
            e->m_Level.getInactiveBeacons().push_back(newBeacon);
            break;
        case Beacon::COMBAT:
            e->m_Level.getCombatBeacons().push_back(newBeacon);
            break;
        case Beacon::SEARCH:
            e->m_Level.getSearchBeacons().push_back(newBeacon);
            break;
        case Beacon::LOCATION:
            e->m_Level.getLocationBeacons().push_back(newBeacon);
            break;
        default:
            std::cerr << "Unhandled Save Beacon Type Case" << std::endl;
            break;
        }
    }
}
