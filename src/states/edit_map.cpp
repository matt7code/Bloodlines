#include "states/edit_map.h"

StateEditMap StateEditMap::m_StateEditMap;

void StateEditMap::Init(Engine* e) {
    e->m_RenderWindow.setMouseCursorVisible(true);

    e->m_View.reset(sf::FloatRect(0, 0, 1920, 1080));
    e->m_View.zoom(0.5f);
    e->m_RenderWindow.setView(e->m_View);
    m_viewPos = e->getParty().getPos();

    // Load a sprite to display
    if (!m_mapEditTexture.loadFromFile("graphics/map_edit_texture.png")) {
        std::cerr << "Error loading graphics/map_edit_texture.png" << std::endl;
    }
    m_Sprite_Selection.setTexture(m_mapEditTexture);
    m_Sprite_Selection.setTextureRect(sf::IntRect(0, 0, 41, 41));
    m_Sprite_Selection.setOrigin(Tool::centerSprite(m_Sprite_Selection));

    m_TileSet = e->m_Level.texture;
    m_Sprite_Tileset.setTexture(m_TileSet);

    Hud.setFont(m_Boxes.getFont(Box::FONT_NORMAL));
    Hud.setCharacterSize(32);

    ShadedRectangle.setFillColor(sf::Color(0, 0, 0, 130));
    ShadedRectangle.setPosition(50, 110);
    ShadedRectangle.setSize(sf::Vector2f(120, 120));

    m_activeTile.getSprite().setTexture(e->m_Level.texture);
    m_activeTile.getSprite().setTextureRect(sf::IntRect(0, 0, 32, 32));

    LoadAnimatedTiles(e);
    LoadBeaconLocations(e);
}

void StateEditMap::Cleanup(Engine* e) {
    e->m_RenderWindow.setMouseCursorVisible(false);
    SaveAnimatedTiles();

    beaconMode = false;
    fillMode = false;
//    objectMode = false;

    bShowAnimDesigner = false;
    bShowTiles = false;

    for (auto z : e->m_Level.area) {
        z.setChanged(true);
        z.Save();
    }
    e->getParty().MovePartyTo(m_worldPos);
}

void StateEditMap::Pause(Engine* e) {
    //std::cout << "EditMap Pause" << std::endl;
    e->getParty().MovePartyTo(m_viewPos);
}

void StateEditMap::Resume(Engine* e) {
    LoadBeaconLocations(e);
    m_viewPos = e->getParty().getPos();
    if(e->getInputString() == "CaNcElEd") {
        m_activeTile.setKey(0);
    } else {
        std::stringstream ss;
        ss << e->getInputString();
        unsigned short key;
        ss >> key;
        m_activeTile.setKey(key);
    }
}

void StateEditMap::HandleEvents(Engine* e) {
    sf::Event event;
    while (e->m_RenderWindow.pollEvent(event)) {
        if (event.type == sf::Event::Closed) e->Quit();
        if (event.type == sf::Event::MouseWheelScrolled) {
            if(bShowTiles) ScrollTileSheet(event.mouseWheelScroll.delta);
            else if (bShowAnimDesigner) DesignerClickHandler(m_mousePos, -1, event.mouseWheelScroll.delta, e);
            else {
                if(sf::Keyboard::isKeyPressed(sf::Keyboard::RControl) || sf::Keyboard::isKeyPressed(sf::Keyboard::LControl)) ChangeSelectorSize(event.mouseWheelScroll.delta);
                else {
                    CurrentTile += event.mouseWheelScroll.delta;
                    if (CurrentTile < 0) CurrentTile = e->m_Level.tileset.size() - 1;
                    else if ((unsigned)CurrentTile > e->m_Level.tileset.size() - 1) CurrentTile = 0;
                    UpdateActiveTile(CurrentTile, e);
                }
            }
        }

        if (event.type == sf::Event::MouseButtonPressed) {
            switch (event.mouseButton.button) {
            case sf::Mouse::Button::Left:
                if(beaconMode) {
                    beaconMode = false;
                    e->BeaconPos = sf::Vector3i(m_worldPos.x, m_worldPos.y, e->m_Level.z_Level);
                    e->PushState( EditBeacon::Instance() );
                } else if(fillMode && !bShowTiles && !bShowAnimDesigner) floodFill(m_worldPos, e);
                else if(bShowAnimDesigner) DesignerClickHandler(m_mousePos, event.mouseButton.button, 0, e);
                break;

            case sf::Mouse::Button::Right:
                if(beaconMode) beaconMode = false;
                if(bShowAnimDesigner) DesignerClickHandler(m_mousePos, event.mouseButton.button, 0, e);
                break;

            case sf::Mouse::Button::Middle:
                if(bShowAnimDesigner) DesignerClickHandler(m_mousePos, event.mouseButton.button, 0, e);
                else TileInspect(e->m_Level.getZone(m_worldPos).getTile(m_worldPos));
                break;

            default:
                break;
            }
        }

        if (event.type == sf::Event::KeyPressed) {
            switch(event.key.code) {
            case sf::Keyboard::Escape:
                if(bShowTiles)
                    bShowTiles = false;
                else if(bShowAnimDesigner)
                    TurnOffAnimDesigner();
                else e->PopState();
                break;
            case sf::Keyboard::Num0:
                m_activeTile.setIsWalkable(false);
                m_activeTile.setIsLocked(false);
                break;
            case sf::Keyboard::Num1:
                m_activeTile.setIsWalkable(!m_activeTile.getIsWalkable());
                break;
            case sf::Keyboard::Num2:
                m_activeTile.setIsLocked(!m_activeTile.getIsLocked());
                break;
            case sf::Keyboard::F:
                beaconMode = false;
                fillMode = !fillMode;
                break;
            case sf::Keyboard::B:
                fillMode = false;
                beaconMode = !beaconMode;
                break;
            case sf::Keyboard::O:
                beaconMode = false;
                fillMode = false;
                e->PushState( EditObject::Instance() );
                break;
            case sf::Keyboard::T:
                beaconMode = false;
                fillMode = false;
                e->PushState( EditTriggers::Instance() );
                break;
            case sf::Keyboard::LBracket:
                e->m_Level.z_Level--;
                LoadBeaconLocations(e);
                break;
            case sf::Keyboard::RBracket:
                e->m_Level.z_Level++;
                LoadBeaconLocations(e);
                break;
            case sf::Keyboard::K:
                e->setInputString("Key Value:Enter a numeric value for the item (key) which unlocks this tile.");
                e->PushState(Input_String::Instance());
                break;
            default:
                break;
            }
        }
    }
}

void StateEditMap::Update(Engine* e) {
    float moveDelta = (e->getTimeRunning().getElapsedTime().asSeconds() - m_delta.asSeconds()) * MAP_EDIT_MOVE_SPEED;
    m_delta = sf::microseconds(e->getTimeRunning().getElapsedTime().asMicroseconds());

    if(sf::Keyboard::isKeyPressed(sf::Keyboard::Up) || sf::Keyboard::isKeyPressed(sf::Keyboard::W))     m_viewPos.y -= moveDelta;
    if(sf::Keyboard::isKeyPressed(sf::Keyboard::Down) || sf::Keyboard::isKeyPressed(sf::Keyboard::S))   m_viewPos.y += moveDelta;
    if(sf::Keyboard::isKeyPressed(sf::Keyboard::Left) || sf::Keyboard::isKeyPressed(sf::Keyboard::A))   m_viewPos.x -= moveDelta;
    if(sf::Keyboard::isKeyPressed(sf::Keyboard::Right) || sf::Keyboard::isKeyPressed(sf::Keyboard::D))  m_viewPos.x += moveDelta;

    if(m_viewPos.x < 1000) m_viewPos.x = 1000;
    if(m_viewPos.y < 1000) m_viewPos.y = 1000;

    m_mousePos = sf::Mouse::getPosition(e->m_RenderWindow);
    Tool::ConvertClickToScreenPos(m_mousePos, e->m_RenderWindow);
    m_worldPos = e->m_RenderWindow.mapPixelToCoords(sf::Mouse::getPosition(e->m_RenderWindow), e->m_View);

    if(sf::Mouse::isButtonPressed(sf::Mouse::Button::Left)) {
        if(bShowTiles && m_mousePos.x < 768) {
            TilePick = sf::Vector2i(m_mousePos.x / 32, (m_mousePos.y + (tileSheetY * -1)) / 32);
            CurrentTile = (TilePick.y * 24) + TilePick.x;
            UpdateActiveTile(CurrentTile, e);
        } else if (bShowAnimDesigner) {
            // Do Nothing.
        } else {
            if (!fillMode && !beaconMode) {
                //std::cout << "Active Tile Key: " << m_activeTile.getKey() << std::endl;
                if(selectorSize == SELECTOR_TINY)     InsertTiles(m_activeTile, e, m_worldPos, 1);
                if(selectorSize == SELECTOR_SMALL)    InsertTiles(m_activeTile, e, m_worldPos, 1);
                if(selectorSize == SELECTOR_MEDIUM)   InsertTiles(m_activeTile, e, m_worldPos, 2);
                if(selectorSize == SELECTOR_LARGE)    InsertTiles(m_activeTile, e, m_worldPos, 3);
                if(selectorSize == SELECTOR_HUGE)     InsertTiles(m_activeTile, e, m_worldPos, 4);
            }
        }
    }
    if(sf::Mouse::isButtonPressed(sf::Mouse::Button::Right)) {
        if(!bShowTiles && !bShowAnimDesigner) {
            CurrentTile = e->m_Level.getZone(m_worldPos).getTile(m_worldPos).getType();
            m_activeTile.setAttributes(GetTileAtWorldPos(m_worldPos, e).getAttributes());
            if(e->m_Level.getZone(m_worldPos).getTile(m_worldPos).getIsAnimated()) {
                m_activeTile = e->m_Level.getZone(m_worldPos).getTile(m_worldPos);
            }
            else {
                UpdateActiveTile(CurrentTile, e);
            }
        }
    }

    if (!bShowAnimDesigner && m_mousePos.x > 1895 && m_mousePos.y < 25) {
        TurnOnAnimDesigner();
    } else {
        if (!bShowTiles && m_mousePos.x < 25 && m_mousePos.y < 25) bShowTiles = true;
        else if (bShowTiles && m_mousePos.x > 768) bShowTiles = false;
    }

    // Actual Updates
    e->m_Level.Update(e->getTimeRunning(), m_viewPos, e->viewExtents);
    e->m_View.setCenter(m_viewPos);

    if (!bShowTiles) {
        if(fillMode   && (int)(e->getTimeRunning().getElapsedTime().asSeconds() * 10) % 2 == 0)
            HudString << " FILL MODE ";
        if(beaconMode && (int)(e->getTimeRunning().getElapsedTime().asSeconds() * 10) % 2 == 0)
            HudString << " BEACON MODE ";
        HudString << std::endl;
        HudString << "ViewPos: " << (int)m_viewPos.x << "x, " << (int)m_viewPos.y << "y MousePos: " << m_mousePos.x << "x, " << m_mousePos.y << "y World Pos: " << (int)m_worldPos.x << ", " << (int)m_worldPos.y << "y Zone: " << e->m_Level.getZone(m_worldPos).getName();
        HudString << std::endl << "\t\tF: Toggle Fill Mode"
                  << std::endl << "\t\tB: Edit Beacons"
                  << std::endl << "\t\tO: Edit Objects"
                  << std::endl << "\t\tT: Edit Triggers"
                  << std::endl << "\t\tMiddle-Click: Tile Info"
                  << std::endl << std::endl
                  << std::endl << "1: Walkable?  " << (m_activeTile.getIsWalkable() ? "True" : "False")
                  << std::endl << "2: Locked?    " << (m_activeTile.getIsLocked() ? "True" : "False")
                  << std::endl
                  << std::endl << "K: Set Key (Active Key: ";
        if(m_activeTile.getKey() == 0) HudString << "None";
        else HudString << m_activeTile.getKey();
        HudString << ")"
                  << std::endl << "[ Drop one floor/level"
                  << std::endl << "] Rise one floor/level" << std::endl
                  << m_InspectDetails;
    }
    Hud.setString(HudString.str());
    HudString.clear();
    HudString.str("");

    if(bShowAnimDesigner) {
        for(auto& t : AnimatedTiles)
            if(t.getIsAnimated()) t.Update(e->getTimeRunning());
    }

    if(m_activeTile.getIsAnimated()) {
        for (auto& t : m_activeTile.getAnimTiles()) {
            t.getSprite().setPosition(62, 174);
            t.getSprite().setScale(3.0f, 3.0f);
        }
        m_activeTile.Update(e->getTimeRunning());
    }
    m_Sprite_Selection.setPosition(static_cast<sf::Vector2f>(m_mousePos));
}

void StateEditMap::Draw(Engine* e) {
    e->m_RenderWindow.setView(e->m_View);
    e->m_Level.Draw(e->m_RenderWindow);

    if(beaconMode) {
        for(auto b : m_Beacons) {
            e->m_RenderWindow.draw(b);
        }
    }

    e->m_RenderWindow.setView(e->m_ViewBase);

    if (bShowAnimDesigner) {
        DrawDesigner(e->m_RenderWindow);
        e->m_RenderWindow.draw(m_Sprite_Tileset);

        for (auto& t : AnimatedTileCells)
            e->m_RenderWindow.draw(t.getSprite());

        for (auto& t : AnimatedTiles)
            e->m_RenderWindow.draw(t.getSprite());
    } else if (bShowTiles) {
        e->m_RenderWindow.draw(m_Sprite_Tileset);
    } else {
        Hud.setPosition(sf::Vector2f(52, 22));
        Hud.setColor(sf::Color::Black);
        e->m_RenderWindow.draw(Hud);
        Hud.setPosition(sf::Vector2f(50, 20));
        Hud.setColor(sf::Color::White);
        e->m_RenderWindow.draw(Hud);
        e->m_RenderWindow.draw(ShadedRectangle);
        e->m_RenderWindow.draw(m_activeTile.getSprite());
    }
    m_Boxes.DrawBoxes(e->m_RenderWindow);
    DrawSelector(e->m_RenderWindow);
}

void StateEditMap::DrawSelector(sf::RenderWindow& window) {
    float scale;
    if(selectorSize == SELECTOR_TINY)   scale = 1.0f;
    if(selectorSize == SELECTOR_SMALL)  scale = 2.0f;
    if(selectorSize == SELECTOR_MEDIUM) scale = 3.0f;
    if(selectorSize == SELECTOR_LARGE)  scale = 5.0f;
    if(selectorSize == SELECTOR_HUGE)   scale = 7.0f;
    m_Sprite_Selection.setScale(scale, scale);
    window.draw(m_Sprite_Selection);
}

Tile StateEditMap::BuildTile(Engine* e, sf::Vector2f worldPos, int index,
                             float duration, std::vector<int> animTiles) {
    Tile t;
    t.getSprite().setPosition(getWorldTilePos(worldPos));
    t.setType(index);
    t.setAttributes(m_activeTile.getAttributes());
    t.getSprite().setTexture(e->m_Level.texture);

    int x, y;
    x = index % 24;
    y = index / 24;
    t.getSprite().setTextureRect(sf::IntRect(x * TILE_SIZE_X, y * TILE_SIZE_Y, TILE_SIZE_X, TILE_SIZE_Y));
    return t;
}

sf::Vector2f StateEditMap::getWorldTilePos(sf::Vector2f worldPos) {
    return sf::Vector2f(((int)worldPos.x / TILE_SIZE_X) * TILE_SIZE_X, ((int)worldPos.y / TILE_SIZE_Y) * TILE_SIZE_Y);
}

bool StateEditMap::InsertTiles(Tile tile, Engine* e, sf::Vector2f pos, int dim) {
    /// Get locations.
    int dim_offset = 16 * (dim - 1);

    sf::Vector2f wtp = sf::Vector2f((int)pos.x - dim_offset, (int)pos.y - dim_offset);
    std::vector<sf::Vector2f> locations;
    for (int i = 0; i < (dim * dim); ++i) {
        int x = (i % dim) * TILE_SIZE_X;
        int y = (i / dim) * TILE_SIZE_Y;
        locations.push_back(ConvertWorldPosToWorldTileCoordinate(sf::Vector2f( wtp.x + x, wtp.y + y )));
    }
    /// Done getting locations.

    std::string zoneName;
    Zone* zp = nullptr;
    for (auto p : locations) {
        zoneName = Zone::BuildName(sf::Vector3f(p.x, p.y, e->m_Level.z_Level));
        for (auto& z : e->m_Level.area) if (z.getName() == zoneName) zp = &z;

        tile.getSprite().setScale(1.0f, 1.0f);
        tile.getSprite().setPosition(p);
        tile.setAttributes(m_activeTile.getAttributes());

        if(tile.getIsAnimated()) {
            for (auto& t : tile.getAnimTiles()) {
                t.getSprite().setScale(1.0f, 1.0f);
                t.getSprite().setPosition(p);
                t.setAttributes(m_activeTile.getAttributes());
                t.setIsAnimated(false);
            }
        }
        zp->setChanged(true);
        zp->getTilesRef()[Zone::getIndex(p)] = tile;
    }
    return true;
}

void StateEditMap::pv(sf::Vector2f pos, std::string s) {
    if (s == "")
        std::cout << "Position: " << pos.x << "x, " << pos.y << "y" << std::endl;
    else
        std::cout << s << ": " << pos.x << "x, " << pos.y << "y" << std::endl;
}

sf::Vector2f StateEditMap::ConvertWorldPosToWorldTileCoordinate(sf::Vector2f pos) {
    int x = pos.x / TILE_SIZE_X;
    int y = pos.y / TILE_SIZE_Y;
    return sf::Vector2f(x * TILE_SIZE_X, y * TILE_SIZE_Y);
}

//std::string StateEditMap::Choice(int attr) {
//    if (attr == 0) return "AUTO";
//    if (attr == 1) return "YES";
//    if (attr == 2) return "NO";
//    return "UNKNOWN";
//}

Tile& StateEditMap::GetTileAtWorldPos(sf::Vector2f worldPos, Engine* e) {
    std::string targetZone = Zone::BuildName(sf::Vector3f(worldPos.x, worldPos.y, e->m_Level.z_Level));
    Zone* zp = nullptr;
    for (auto& z : e->m_Level.area) if (z.getName() == targetZone) zp = &z;
    std::vector<Tile>& vt = zp->getTilesRef();
    int temp = Zone::getIndex(worldPos);
    return vt[temp];
}

void StateEditMap::floodFill(sf::Vector2f worldPos, Engine* e) {
    /// Return if the tile clicked on matches the active tile.
    int prevType    = e->m_Level.getZone(worldPos).getTile(worldPos).getType();
    int newType     = m_activeTile.getType();

    if(newType == prevType) return;

    /// Get the tile Coordinate.
    sf::Vector2f pos = Tool::convertToTileCoords(worldPos);
    sf::Vector2f areaOrigin = e->m_Level.getAreaOrigin();

    /// Prepare array to hold Types - Size of all 4 loaded Zones
    int TypeArray[64][40];

    /// Copy the 4 zone Tile types to the TypeArray
    for(int k = 0; k < 40; ++k) {
        for(int j = 0; j < 64; ++j) {
            sf::Vector2f localPos = sf::Vector2f(areaOrigin.x + (j * 32), areaOrigin.y + (k * 32));
            TypeArray[j][k] = e->m_Level.getZone(localPos).getTile(localPos).getType();
        }
    }

    int x = pos.x - areaOrigin.x;
    int y = pos.y - areaOrigin.y;
    x /= 32;
    y /= 32;

    floodFillUtil(TypeArray, x, y, prevType, newType);

    int m_count = 0;
    for(int k = 0; k < 40; ++k) {
        for(int j = 0; j < 64; ++j) {
            sf::Vector2f localPos = sf::Vector2f(areaOrigin.x + (j * 32), areaOrigin.y + (k * 32));
            if (TypeArray[j][k] != e->m_Level.getZone(localPos).getTile(localPos).getType()) {
                //activeTile.setAttributes(m_activeAttributes.to_ulong());
                m_activeTile.setWillBeDrawn(true);
                e->m_Level.getZone(localPos).getTilesRef()[Zone::getIndex(localPos)] = m_activeTile;
                e->m_Level.getZone(localPos).getTilesRef()[Zone::getIndex(localPos)].getSprite().setScale(1.0f, 1.0f);
                e->m_Level.getZone(localPos).getTilesRef()[Zone::getIndex(localPos)].getSprite().setPosition(localPos);

                for (auto& t : e->m_Level.getZone(localPos).getTilesRef()[Zone::getIndex(localPos)].getAnimTiles()) {
                    //t.setWillBeDrawn(true);
                    t.getSprite().setPosition(localPos);
                    t.getSprite().setScale(1.0f, 1.0f);
                }

                e->m_Level.getZone(localPos).setChanged(true);
                ++m_count;
            }
        }
    }
    fillMode = false;
}

void StateEditMap::floodFillUtil(int (&TypeArray)[64][40], int x, int y, int prevType, int newType) {
    // Base cases
    if(x < 0 || x >= 64 || y < 0 || y >= 40) return;
    if(TypeArray[x][y] != prevType) return;

    TypeArray[x][y] = newType;

    floodFillUtil(TypeArray, x + 1, y, prevType, newType);
    floodFillUtil(TypeArray, x - 1, y, prevType, newType);
    floodFillUtil(TypeArray, x, y + 1, prevType, newType);
    floodFillUtil(TypeArray, x, y - 1, prevType, newType);
}

void StateEditMap::TileInspect(Tile& tile) {
    std::stringstream ss;
    ss << "\nTile Inspector" << std::endl;
    ss << "Texture:    " << tile.getSprite().getTexture() << std::endl;
    ss << "Type:       " << tile.getType() << std::endl;
    std::bitset<32> temp_bit = tile.getAttributes();
    ss << "Attributes: " << temp_bit.to_string() << std::endl;
    ss << "Key         " << tile.getKey() << std::endl;
    ss << "Animated:   " << tile.getIsAnimated() << std::endl;
    ss << "Walkable:   " << tile.getIsWalkable() << std::endl;
    ss << "Position:   " << tile.getSprite().getPosition().x << "x, " << tile.getSprite().getPosition().y << "y" << std::endl;
    ss << "TileCoords: " << "T:" << tile.getSprite().getTextureRect().top << " L:" << tile.getSprite().getTextureRect().left
       << " H:" << tile.getSprite().getTextureRect().height << " W:" << tile.getSprite().getTextureRect().width << std::endl;
    m_InspectDetails = ss.str();
}

void StateEditMap::UpdateActiveTile(int CurrentTile, Engine* e) {
    if (CurrentTile < 0) CurrentTile = e->m_Level.tileset.size() - 1;
    else if ((unsigned)CurrentTile > e->m_Level.tileset.size() - 1) CurrentTile = 0;
    m_activeTile.getSprite() = e->m_Level.tileset[CurrentTile].getSprite();
    m_activeTile.getSprite().setPosition(62, 124);
    m_activeTile.getSprite().setScale(3.0f, 3.0f);
    m_activeTile.setType(CurrentTile);
    //activeTile.setAttributes(m_activeAttributes.to_ulong());
}

void StateEditMap::LoadAnimatedTiles(Engine* e) {
    AnimatedTileCells.resize(22);
    AnimatedTiles.clear();
    std::ifstream in("animated.tiles");
    std::string line;

    int cnt = 0, xpos = 0, ypos = 0;
    while(std::getline(in, line)) {
        //std::cout << line << std::endl;
        std::stringstream ss;
        ss.str(line);
        Tile newTile;

        xpos = 805 + ((cnt % 22) * 50);
        ypos = 125  + (cnt / 22 * 50);

        float dura;
        ss >> dura;
        newTile.setType(0);
        newTile.setDuration(dura);
        newTile.setIsAnimated(true);
        newTile.getSprite().setPosition(xpos, ypos);

        int n;
        while (ss >> n) {
            Tile subTile;
            subTile.getSprite() = e->m_Level.tileset[n].getSprite();
            subTile.setType(n);
            subTile.setAttributes(13);
            subTile.getSprite().setPosition(xpos, ypos);
            newTile.getAnimTiles().push_back(subTile);
        }
        AnimatedTiles.push_back(newTile);
        ++cnt;
    }

    // Set up the animated cell's click map.
    for (int i = 0; i < 418; i++) {
        int xpos = 801 + ((i % 22) * 50);
        int ypos = 121 + ((i / 22) * 50);
        sf::IntRect rect(xpos, ypos, 40, 40);
        ClickMap[i] = rect;
    }

    // Set up click map for cell frames.
    for (int i = 0; i < 22; i++) {
        sf::IntRect rect(802 + (i * 50), 68, 40, 40);
        ClickMapFrames[i] = rect;
    }
}

void StateEditMap::SaveAnimatedTiles() {
    std::ofstream of("animated.tiles");
    for (auto t : AnimatedTiles) {
        of << t.getDuration() << " ";
        for (auto u : t.getAnimTiles())
            of << u.getType() << " ";
        of << std::endl;
    }
}

void StateEditMap::DrawDesigner(sf::RenderWindow& window) {
    // Light Blue Backdrop
    sf::RectangleShape background(sf::Vector2f(1152, 1080));
    background.setFillColor(sf::Color(0, 128, 255, 200));
    background.setPosition(768, 0);

    // Yellow Bar
    sf::RectangleShape yellowBar(sf::Vector2f(1152, 52));
    yellowBar.setFillColor(sf::Color::Yellow);
    yellowBar.setPosition(768, 60);

    window.draw(background);
    window.draw(yellowBar);

    // activeAnimFrameCellBackground
    sf::RectangleShape activeAnimFrameCellBackground(sf::Vector2f(40, 40));
    activeAnimFrameCellBackground.setFillColor(sf::Color::Black);
    activeAnimFrameCellBackground.setOutlineColor(sf::Color::Red);
    activeAnimFrameCellBackground.setOutlineThickness(1.0f);
    int xpos = 800 + ((activeAnimFrameCell % 22) * 50);
    activeAnimFrameCellBackground.setPosition(xpos, 66);
    window.draw(activeAnimFrameCellBackground);


    for(int i = 0; i < 22; i++) {
        sf::RectangleShape box(sf::Vector2f(36, 36));
        box.setOutlineColor(sf::Color::Black);
        box.setOutlineThickness(1.0f);
        box.setPosition(802 + (i * 50), 68);
        window.draw(box);
    }

    if (activeAnimCell > -1) {
        sf::RectangleShape activeAnimCellBackground(sf::Vector2f(40, 40));
        activeAnimCellBackground.setFillColor(sf::Color(0, 0, 0, 0));
        activeAnimCellBackground.setOutlineColor(sf::Color::Red);
        activeAnimCellBackground.setOutlineThickness(1.0f);

        int xpos = 801 + ((activeAnimCell % 22) * 50);
        int ypos = 121  + (activeAnimCell / 22 * 50);
        activeAnimCellBackground.setPosition(xpos, ypos);
        window.draw(activeAnimCellBackground);
    }

    for(auto t : AnimatedTileCells) {
        window.draw(t.getSprite());
    }
}

void StateEditMap::TurnOnAnimDesigner() {
    selectorSize = SELECTOR_TINY;
    bShowAnimDesigner = true;
    bShowTiles = true;

    m_Boxes.addBox("AnimationBackdrop");
    m_Boxes.getBox("last").setPos(sf::Vector2f(768, 0));
    m_Boxes.getBox("last").setSize(sf::Vector2f(1152, 60));
    m_Boxes.getBox("last").setStyle(Box::BOX_PLAIN);

    const int offset_x = 400;
    const int offset_y = 10;

    m_Boxes.addBox("AnimationDuration");
    m_Boxes.getBox("last").setPos(sf::Vector2f(offset_x + 800, offset_y + 5));
    m_Boxes.getBox("last").setSize(sf::Vector2f(135, 30));
    m_Boxes.getBox("last").addText("Duration", sf::Vector2f(15, -1), 24, Box::FONT_NORMAL, sf::Color::White, 0);
    m_Boxes.getBox("last").setColor(sf::Color::Black);
    m_Boxes.getBox("last").setStyle(Box::BOX_PLAIN);

    m_Boxes.addBox("CurrentTens");
    m_Boxes.getBox("last").setPos(sf::Vector2f(offset_x + 950, offset_y + 5));
    m_Boxes.getBox("last").setSize(sf::Vector2f(30, 30));
    m_Boxes.getBox("last").addText("0", sf::Vector2f(10, -1), 24, Box::FONT_NORMAL, sf::Color::White, 0);
    m_Boxes.getBox("last").setColor(sf::Color::Black);
    m_Boxes.getBox("last").setStyle(Box::BOX_PLAIN);

    m_Boxes.addBox("CurrentOnes");
    m_Boxes.getBox("last").setPos(sf::Vector2f(offset_x + 985, offset_y + 5));
    m_Boxes.getBox("last").setSize(sf::Vector2f(30, 30));
    m_Boxes.getBox("last").addText("0", sf::Vector2f(10, -1), 24, Box::FONT_NORMAL, sf::Color::White, 0);
    m_Boxes.getBox("last").setColor(sf::Color::Black);
    m_Boxes.getBox("last").setStyle(Box::BOX_PLAIN);

    m_Boxes.addBox("CurrentDot");
    m_Boxes.getBox("last").setPos(sf::Vector2f(offset_x + 1020, offset_y + 5));
    m_Boxes.getBox("last").setSize(sf::Vector2f(30, 30));
    m_Boxes.getBox("last").addText(".", sf::Vector2f(10, -1), 24, Box::FONT_NORMAL, sf::Color::White, 0);
    m_Boxes.getBox("last").setColor(sf::Color::Black);
    m_Boxes.getBox("last").setStyle(Box::BOX_PLAIN);

    m_Boxes.addBox("CurrentTenths");
    m_Boxes.getBox("last").setPos(sf::Vector2f(offset_x + 1055, offset_y + 5));
    m_Boxes.getBox("last").setSize(sf::Vector2f(30, 30));
    m_Boxes.getBox("last").addText("0", sf::Vector2f(10, -1), 24, Box::FONT_NORMAL, sf::Color::White, 0);
    m_Boxes.getBox("last").setColor(sf::Color::Black);
    m_Boxes.getBox("last").setStyle(Box::BOX_PLAIN);

    SetDuration(tenths);
}

void StateEditMap::TurnOffAnimDesigner() {
    selectorSize = SELECTOR_SMALL;
    bShowAnimDesigner = false;
    m_Boxes.removeBox("AnimationDuration");
    m_Boxes.removeBox("CurrentTens");
    m_Boxes.removeBox("CurrentOnes");
    m_Boxes.removeBox("CurrentTenths");
    m_Boxes.removeBox("CurrentDot");
    m_Boxes.removeBox("AnimationBackdrop");
}

void StateEditMap::DesignerClickHandler(sf::Vector2i MouseClick, int button, int delta, Engine *e) {
    //ConvertClickToScreenPos(MouseClick, e->m_RenderWindow);
    switch(button) {
    case -1: // Rolled
        if(MouseClick.x < 768) {
            ScrollTileSheet(delta);
        } else if (sf::IntRect(1350, 15, 30, 30).contains(MouseClick)) {
            tenths += (delta * 100);
            SetDuration(tenths);
        } else if (sf::IntRect(1385, 15, 30, 30).contains(MouseClick)) {
            tenths += (delta * 10);
            SetDuration(tenths);
        } else if (sf::IntRect(1455, 15, 30, 30).contains(MouseClick)) {
            tenths += delta;
            SetDuration(tenths);
        }
        break;

    case sf::Mouse::Button::Left:
        if(MouseClick.x < 768) {
            TilePick = sf::Vector2i(MouseClick.x / 32, (MouseClick.y + (tileSheetY * -1)) / 32);
            CurrentTile = (TilePick.y * 24) + TilePick.x;

            UpdateActiveTile(CurrentTile, e);

            Tile temp;
            temp = m_activeTile;
            temp.setWillBeDrawn(true);
            temp.getSprite().setPosition(804 + (activeAnimFrameCell * 50), 70);
            temp.getSprite().setScale(1.0, 1.0);

            AnimatedTileCells[activeAnimFrameCell] = temp;
            if(activeAnimFrameCell < 22) activeAnimFrameCell++;
        } else {
            if(MouseClick.y > 60 && MouseClick.y < 112) {
                for (auto it = ClickMapFrames.begin(); it != ClickMapFrames.end(); ++it) {
                    if (it->second.contains(MouseClick)) activeAnimFrameCell = it->first;
                }
            } else {
                for (auto it = ClickMap.begin(); it != ClickMap.end(); ++it) {
                    if (it->second.contains(MouseClick)) {
                        SaveOldFrames(activeAnimCell, e);
                        activeAnimCell = it->first;
                        FillInFrames(it->first);
                    }
                }
            }
        }
        break;
    case sf::Mouse::Button::Right:
        if(MouseClick.x > 768) {
            if(MouseClick.y > 60 && MouseClick.y < 112) {
                for (auto it = ClickMapFrames.begin(); it != ClickMapFrames.end(); ++it) {
                    if (it->second.contains(MouseClick)) {
                        //std::cout << "  Invalidating Frame #" << it->first << std::endl;
                        AnimatedTileCells[it->first].setWillBeDrawn(false);
                    }
                }
            } else {
                for (auto it = ClickMap.begin(); it != ClickMap.end(); ++it) {
                    if (it->second.contains(MouseClick)) {
                        //std::cout << "  Resetting cell #" << it->first << std::endl;
                        ResetCell(it->first, e);
                    }
                }
            }
        }
        break;
    case sf::Mouse::Button::Middle:
        //std::cout << "Designer Middle Clicked @ " << MouseClick.x << ", " << MouseClick.y << std::endl;
        break;
    default:
        break;
    }
}

void StateEditMap::SetDuration(int& _tenths) {
    //std::cout << "  [Setting Duration]" << std::endl;
    if (_tenths > 999) _tenths = 999;
    if (_tenths < 1) _tenths = 1;
    std::stringstream ss;
    int temp;
    ss.str("");
    temp = (_tenths / 100) % 10;
    ss << temp;
    m_Boxes.getBox("CurrentTens").getText().begin()->setString(ss.str());
    ss.str("");
    temp = (_tenths / 10) % 10;
    ss << temp;
    m_Boxes.getBox("CurrentOnes").getText().begin()->setString(ss.str());
    ss.str("");
    temp = (_tenths / 1) % 10;
    ss << temp;
    m_Boxes.getBox("CurrentTenths").getText().begin()->setString(ss.str());
}

void StateEditMap::FillInFrames(int slot) {
    Tile t = AnimatedTiles[slot];
    m_activeTile = t;
    tenths = t.getDuration() * 10;
    SetDuration(tenths);

    AnimatedTileCells.clear();
    int i = 0;
    for (auto u : t.getAnimTiles()) {
        u.getSprite().setPosition(804 + (i * 50), 70);
        u.setWillBeDrawn(true);
        AnimatedTileCells.push_back(u);
        i++;
    }

    while (AnimatedTileCells.size() < 22) {
        Tile temp;
        temp.setWillBeDrawn(false);
        AnimatedTileCells.push_back(temp);
    }
}

void StateEditMap::SaveOldFrames(int cell, Engine *e) {
    if (cell == -1) return;
    activeAnimFrameCell = 0;

    int xpos = 805 + ((cell % 22) * 50);
    int ypos = 125 + ((cell / 22) * 50);

    float dura = tenths / (float)10;

    Tile t;
    t.setType(0);
    t.getSprite() = e->m_Level.tileset[0].getSprite();
    t.getSprite().setPosition(xpos, ypos);
    t.setDuration(dura);
    t.setIsAnimated(true);

    for (auto a : AnimatedTileCells) {
        Tile subTile;
        subTile.setType(a.getType());
        subTile.getSprite() = e->m_Level.tileset[a.getType()].getSprite();
        subTile.getSprite().setPosition(xpos, ypos);
        //std::cout << "WillBeDrawn?" << a.getWillBeDrawn() << std::endl;
        if(a.getWillBeDrawn()) t.getAnimTiles().push_back(subTile);
    }
    if(t.getAnimTiles().empty()) {
        t.getAnimTiles().push_back(Tile());
    }
    AnimatedTiles[cell] = t;
}

void StateEditMap::ResetCell(int cell, Engine *e) {
    Tile t;
    t.setDuration(1.0f);
    t.setIsAnimated(true);
    t.getAnimTiles().push_back(Tile());

    AnimatedTiles[cell] = t;
}

void StateEditMap::pv(float x, float y, std::string s) {
    pv(sf::Vector2f(x, y), s);
}

void StateEditMap::ScrollTileSheet(int delta) {
    tileSheetY += 160 * delta;
    if (tileSheetY < -264) tileSheetY = -264;
    else if (tileSheetY > 0) tileSheetY = 0;
    m_Sprite_Tileset.setPosition(0, tileSheetY);
}

void StateEditMap::ChangeSelectorSize(int delta) {
    selectorSize += delta;
    if (selectorSize < 0) selectorSize = 4;
    if (selectorSize > 4) selectorSize = 0;
}

void StateEditMap::LoadBeaconLocations(Engine* e) {
    m_Beacons.clear();

    sf::CircleShape beacon;
    beacon.setOutlineThickness(1);

    for(auto& b : e->m_Level.getInactiveBeacons()) {
        if(b.getPosition().z != e->m_Level.z_Level) continue;
        beacon.setOutlineColor(sf::Color(128, 128, 128));
        beacon.setPosition(sf::Vector2f(b.getPosition().x, b.getPosition().y));

        beacon.setFillColor(sf::Color(128, 128, 128));
        beacon.setRadius(8);
        beacon.setPosition(sf::Vector2f(b.getPosition().x - 8, b.getPosition().y - 8));
        m_Beacons.push_back(beacon);

        beacon.setFillColor(sf::Color::Transparent);
        beacon.setRadius(b.getValue1());
        beacon.setPosition(sf::Vector2f(b.getPosition().x - b.getValue1(), b.getPosition().y - b.getValue1()));
        m_Beacons.push_back(beacon);
    }
    for(auto& b : e->m_Level.getCombatBeacons()) {
        if(b.getPosition().z != e->m_Level.z_Level) continue;
        beacon.setOutlineColor(sf::Color::Red);
        beacon.setPosition(sf::Vector2f(b.getPosition().x, b.getPosition().y));

        beacon.setFillColor(sf::Color::Red);
        beacon.setRadius(8);
        beacon.setPosition(sf::Vector2f(b.getPosition().x - 8, b.getPosition().y - 8));
        m_Beacons.push_back(beacon);

        beacon.setFillColor(sf::Color::Transparent);
        beacon.setRadius(b.getValue1());
        beacon.setPosition(sf::Vector2f(b.getPosition().x - b.getValue1(), b.getPosition().y - b.getValue1()));
        m_Beacons.push_back(beacon);
    }
    for(auto& b : e->m_Level.getSearchBeacons()) {
        if(b.getPosition().z != e->m_Level.z_Level) continue;
        beacon.setOutlineColor(sf::Color::Blue);
        beacon.setPosition(sf::Vector2f(b.getPosition().x, b.getPosition().y));

        beacon.setFillColor(sf::Color::Blue);
        beacon.setRadius(8);
        beacon.setPosition(sf::Vector2f(b.getPosition().x - 8, b.getPosition().y - 8));
        m_Beacons.push_back(beacon);

        beacon.setFillColor(sf::Color::Transparent);
        beacon.setRadius(b.getValue1());
        beacon.setPosition(sf::Vector2f(b.getPosition().x - b.getValue1(), b.getPosition().y - b.getValue1()));
        m_Beacons.push_back(beacon);
    }
    for(auto& b : e->m_Level.getLocationBeacons()) {
        if(b.getPosition().z != e->m_Level.z_Level) continue;
        beacon.setOutlineColor(sf::Color::Green);
        beacon.setPosition(sf::Vector2f(b.getPosition().x, b.getPosition().y));

        beacon.setFillColor(sf::Color::Green);
        beacon.setRadius(8);
        beacon.setPosition(sf::Vector2f(b.getPosition().x - 8, b.getPosition().y - 8));
        m_Beacons.push_back(beacon);

        beacon.setFillColor(sf::Color::Transparent);
        beacon.setRadius(b.getValue1());
        beacon.setPosition(sf::Vector2f(b.getPosition().x - b.getValue1(), b.getPosition().y - b.getValue1()));
        m_Beacons.push_back(beacon);
    }
}
