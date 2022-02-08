#include "states/edit_objects.h"

EditObject EditObject::m_EditObject;

void EditObject::Init(Engine* eng) {
    e = eng;
    m_viewPos = e->getParty().getPos();
    m_Boxes.getBoxes().clear();
    UpdateHud();
    m_tilesheet.setTexture(e->m_Level.texture);
}

void EditObject::HandleEvents(Engine* eng) {
    // Process events
    sf::Event event;
    while (e->m_RenderWindow.pollEvent(event)) {
        // Close window : exit
        if (event.type == sf::Event::Closed) e->Quit();
        else if (event.type == sf::Event::KeyPressed) {
            switch(event.key.code) {
            case sf::Keyboard::Escape:
                if(m_showTiles) m_showTiles = false;
                else e->PopState();
                break;

            case sf::Keyboard::C:
                //std::string = Tool::EnterText("Enter a new color in the form \"r g b a\":");
                break;

            default:
                break;
            }
        } else if (event.type == sf::Event::MouseButtonReleased) {
            if (event.mouseButton.button == sf::Mouse::Button::Left) {
                if (m_dragPtr != nullptr) EndDrag();
            }
        } else if (event.type == sf::Event::MouseButtonPressed) {
            switch (event.mouseButton.button) {
            case sf::Mouse::Button::Left:
                if(m_showTiles) ChooseTile();
                else if(Ctrl()) CreateNewObject();
                else BeginDrag(GetHoverTarget());
                break;

            case sf::Mouse::Button::Right:
                if(Ctrl()) DeleteObject(GetHoverTarget());
                else CopyObject(GetHoverTarget());
                break;

            case sf::Mouse::Button::Middle:
                if (Ctrl()) ResetObject(GetHoverTarget());
                else InspectObject(GetHoverTarget());
                break;

            default:
                break;
            }
        } else if (event.type == sf::Event::MouseWheelScrolled) {
            if(m_showTiles) ScrollTileSheet(event.mouseWheelScroll.delta);
            else if (Ctrl()) ScaleObject(GetHoverTarget(), SCALE_INCREMENT * event.mouseWheelScroll.delta);
            else if (Shift()) RotateObject(GetHoverTarget(), 5 * event.mouseWheelScroll.delta);
            else RotateObject(GetHoverTarget(), ROTATE_INCREMENT * event.mouseWheelScroll.delta);
        }
    }
}

void EditObject::Update(Engine* eng) {
    m_mousePos = sf::Mouse::getPosition(e->m_RenderWindow);
    m_worldPos = e->m_RenderWindow.mapPixelToCoords(sf::Mouse::getPosition(e->m_RenderWindow), e->m_View);

    UpdateCamera();
    if (m_dragPtr) UpdateDrag();
    e->m_View.setCenter(m_viewPos);
    e->m_Level.Update(e->getTimeRunning(), m_viewPos, e->viewExtents);

    if (!m_showTiles && m_mousePos.x < 25 && m_mousePos.y < 25) m_showTiles = true;
    else if (m_showTiles && m_mousePos.x > 768) m_showTiles = false;
}

void EditObject::Draw(Engine* eng) {
    e->m_RenderWindow.setView(e->m_View);
    e->m_Level.Draw(e->m_RenderWindow);
    if (m_dragPtr) e->m_RenderWindow.draw(m_dragPtr->m_sprite);
    e->m_RenderWindow.setView(e->m_ViewBase);
    m_Boxes.DrawBoxes(e->m_RenderWindow);
    sf::Sprite decoy = m_activeObject.m_sprite;
    decoy.setScale(3.0, 3.0);
    e->m_RenderWindow.draw(decoy);

    if (m_showTiles) e->m_RenderWindow.draw(m_tilesheet);

}

void EditObject::UpdateCamera() {
    float moveDelta = (e->getTimeRunning().getElapsedTime().asSeconds() - m_delta.asSeconds()) * MAP_EDIT_MOVE_SPEED;
    m_delta = e->getTimeRunning().getElapsedTime();

    if(sf::Keyboard::isKeyPressed(sf::Keyboard::Up) || sf::Keyboard::isKeyPressed(sf::Keyboard::W))     m_viewPos.y -= moveDelta;
    if(sf::Keyboard::isKeyPressed(sf::Keyboard::Down) || sf::Keyboard::isKeyPressed(sf::Keyboard::S))   m_viewPos.y += moveDelta;
    if(sf::Keyboard::isKeyPressed(sf::Keyboard::Left) || sf::Keyboard::isKeyPressed(sf::Keyboard::A))   m_viewPos.x -= moveDelta;
    if(sf::Keyboard::isKeyPressed(sf::Keyboard::Right) || sf::Keyboard::isKeyPressed(sf::Keyboard::D))  m_viewPos.x += moveDelta;

    if(m_viewPos.x < 1000) m_viewPos.x = 1000;
    if(m_viewPos.y < 1000) m_viewPos.y = 1000;

    /* Not sure about below... */
    // Working in the UI?
    //ConvertClickToScreenPos(m_mousePos, e->m_RenderWindow);
    //m_mousePos = e->m_RenderWindow.mapPixelToCoords(sf::Mouse::getPosition(e->m_RenderWindow), e->m_ViewBase);
}

void EditObject::UpdateHud() {
    m_Boxes.removeBox("Desc");
    m_Boxes.addBox("Desc", sf::Vector2f(100,100), sf::Vector2f(50,50), Box::BOX_NONE).addText("Object Editor", sf::Vector2f(0,0), 36);
    std::stringstream ss;
    ss << "Ctrl-Middle      Reset" << std::endl;
    ss << "Left-Click       Drag" << std::endl;
    ss << "Right            Copy" << std::endl;
    ss << "Ctrl-Left        New" << std::endl;
    ss << "Ctrl-Right       Delete" << std::endl;
    ss << "Middle-Click     Information" << std::endl;
    ss << "MouseWheel       Rotate by 22.5 degrees (1/16th)" << std::endl;
    ss << "Shift-Wheel      Rotate by 5 degrees (1/72)" << std::endl;
    ss << "Ctrl-Wheel       Scale" << std::endl;
    //ss << "Shift-Ctrl-Wheel Scale (10 degrees)" << std::endl;
    ss << "C                Enter new shade/color" << std::endl;
    ss << m_InspectDetails;
    m_Boxes.getBox("Desc").addText(ss.str(), sf::Vector2f(0,180), 24);

    sf::RectangleShape ShadedRectangle;
    ShadedRectangle.setFillColor(sf::Color(0, 0, 0, 130));
    ShadedRectangle.setPosition(50, 110);
    ShadedRectangle.setSize(sf::Vector2f(120, 120));
    m_Boxes.getBox("Desc").getRectangles().push_back(ShadedRectangle);

    UpdateActiveObject();
}

bool EditObject::Ctrl() {
    return sf::Keyboard::isKeyPressed(sf::Keyboard::RControl) || sf::Keyboard::isKeyPressed(sf::Keyboard::LControl);
}

bool EditObject::Shift() {
    return sf::Keyboard::isKeyPressed(sf::Keyboard::RShift) || sf::Keyboard::isKeyPressed(sf::Keyboard::LShift);
}

void EditObject::CreateNewObject(Object* m_copyObject) {

    if(m_copyObject == nullptr) {
        Object newObject;
        std::cout << "\tAdding Object #" << m_currTile << " at " << m_worldPos.x << ", " << m_worldPos.y << std::endl;
        newObject = m_activeObject;
        newObject.m_sprite.setPosition(m_worldPos);
        //newObject.m_sprite.setScale(1.0f, 1.0f);
        newObject.m_zLevel = e->m_Level.z_Level;
        e->m_Level.getZone(m_worldPos).m_objects.push_back(newObject);
        std::cout << "\tNew object added to array: " << newObject.m_ID << std::endl;
    } else {
        m_copyObject->m_sprite.setPosition(m_worldPos);

        e->m_Level.getZone(m_worldPos).m_objects.push_back(*m_copyObject);
        std::cout << "\tOld object dragged into array: " << m_copyObject->m_ID << std::endl;
        m_copyObject = nullptr;
    }
    e->m_Level.getZone(m_worldPos).setChanged(true);
}

Object* EditObject::GetHoverTarget() {
    int n = 0;
    Object* ptr = nullptr;
    for(auto& zone : e->m_Level.area) {
        std::cout << "zone.m_objects.size(): " << zone.m_objects.size() << std::endl;
        for(auto& object : zone.m_objects) {
            if(object.m_sprite.getGlobalBounds().contains(m_worldPos)) {
                std::cout << "Object found!" << std::endl;
                ptr = &object;
            }
            ++n;
        }
    }
    std::cout << "Total Objects in Area: " << n << "\nArea Size: " << e->m_Level.area.size() << "\nZone Vector: " << e->m_Level.getZone(m_worldPos).m_objects.size() << std::endl;
    return ptr;
}

void EditObject::BeginDrag(Object* object) {
    if(!object) return;

    std::cout << "Beginning drag on item." << std::endl;
    m_dragObject = *object;
    m_dragPtr = &m_dragObject;
    DeleteObject(object);

    e->m_Level.getZone(m_worldPos).setChanged(true);
}

void EditObject::EndDrag() {
    if(!m_dragPtr) return;
    {
        std::cout << "Ending drag." << std::endl;
        CreateNewObject(m_dragPtr);
        m_dragPtr = nullptr;
    }
    e->m_Level.getZone(m_worldPos).setChanged(true);
}

void EditObject::UpdateDrag() {
    if(m_dragPtr == nullptr) return;
    m_dragPtr->m_sprite.setPosition(m_worldPos);
}

void EditObject::CopyObject(Object* object) {
    if(object == nullptr) return;
    std::cout << "Copying Object" << std::endl;
    UpdateActiveObject(object);
}

void EditObject::DeleteObject(Object* object) {
    if(!object) return;
    std::cout << "Deleting Object" << std::endl;

    auto& myVec = e->m_Level.getZone(m_worldPos).m_objects;

    for (auto it = myVec.begin(); it != myVec.end();) {
        if(it->m_sprite.getPosition() == object->m_sprite.getPosition() && it->m_zLevel == object->m_zLevel) {
            myVec.erase(it);
            return;
        } else ++it;
    }
    e->m_Level.getZone(m_worldPos).setChanged(true);
}

void EditObject::UpdateActiveObject(Object* object) {
    if(!object) {
        m_activeObject.m_ID         = m_currTile;
        m_activeObject.m_sprite     = e->m_Level.tileset[m_currTile].getSprite();
        Tool::centerSprite(m_activeObject.m_sprite);
    } else {
        m_activeObject = *object;
        m_currTile = object->m_ID;
    }
    m_activeObject.m_zLevel = e->m_Level.z_Level;
    m_activeObject.m_sprite.setPosition(sf::Vector2f(110, 170));
}

void EditObject::InspectObject(Object* object) {
    if(!object) {
        m_InspectDetails.clear();
        UpdateHud();
        return;
    }
    std::stringstream ss;
    ss << "\nObject Inspector" << std::endl;
    ss << "Texture     " << object->m_sprite.getTexture() << std::endl;
    ss << "Type        " << object->m_ID << std::endl;
    ss << "Position    " << object->m_sprite.getPosition().x << "x, " << object->m_sprite.getPosition().y << "y" << std::endl;
    ss << "Z Level     " << object->m_zLevel << std::endl;
    ss << "Scale       " << (float)object->m_sprite.getScale().x << "x, " << (float)object->m_sprite.getScale().y << "y" << std::endl;
    ss << "Rotation    " << (float)object->m_sprite.getRotation() << "f" << std::endl;
    sf::Color temp = object->m_sprite.getColor();
    ss << "Color       R" << (unsigned)temp.r << " G" << (unsigned)temp.g << " B" << (unsigned)temp.b << " A" << (unsigned)temp.a << std::endl;
    m_InspectDetails = ss.str();
    UpdateHud();
}

void EditObject::ScaleObject(Object* object, float amount) {
    if(!object) return;
    float x = object->m_sprite.getScale().x + amount;
    float y = object->m_sprite.getScale().y + amount;
    if(x < 0.33333) return;
    object->m_sprite.setScale(x, y);
}

void EditObject::RotateObject(Object* object, float amount) {
    if(!object) return;
    object->m_sprite.rotate(amount);
}

void EditObject::Cleanup(Engine* eng) {
    std::cout << "EditObject Cleanup" << std::endl;
    e->getParty().MovePartyTo(m_viewPos);
}


void EditObject::ResetObject(Object* object) {
    if(!object) return;
    object->m_sprite.setScale(1.0f, 1.0f);
    object->m_sprite.setRotation(0);
    object->m_sprite.setColor(sf::Color::White);
}

void EditObject::ScrollTileSheet(int delta) {
    int tileSheetY = m_tilesheet.getPosition().y;
    tileSheetY += 160 * delta;
    if (tileSheetY < -264) tileSheetY = -264;
    else if (tileSheetY > 0) tileSheetY = 0;
    m_tilesheet.setPosition(0, tileSheetY);
}

void EditObject::ChooseTile() {
    Tool::ConvertClickToScreenPos(m_mousePos, e->m_RenderWindow);
    sf::Vector2i TilePick = sf::Vector2i(m_mousePos.x / 32, (m_mousePos.y + (m_tilesheet.getPosition().y * -1)) / 32);
    m_currTile = (TilePick.y * 24) + TilePick.x;
    UpdateActiveObject();
}
