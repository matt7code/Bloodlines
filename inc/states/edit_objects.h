#ifndef EditObject_H
#define EditObject_H

#include <iostream>
#include <sstream>
#include <SFML/Graphics.hpp>
#include "object.h"
#include "gamestate.h"
#include "boxes.h"
#include "tool.h"

class EditObject : public GameState {
public:
    void Init(Engine* eng);
    void Cleanup(Engine* eng);
    void Pause(Engine* eng) {}
    void Resume(Engine* eng) {}
    void HandleEvents(Engine* eng);
    void Update(Engine* eng);
    void Draw(Engine* eng);

    static EditObject* Instance() {
        return &m_EditObject;
    }

protected:

    EditObject() {}

private:
    void                UpdateCamera();
    void                UpdateHud();
    bool                Ctrl();
    bool                Shift();
    void                CreateNewObject(Object* m_copyObject = nullptr);
    Object*             GetHoverTarget();
    void                BeginDrag(Object* object);
    void                EndDrag();
    void                UpdateDrag();
    void                DeleteObject(Object* object);
    void                CopyObject(Object* object);
    void                UpdateActiveObject(Object* object = nullptr);
    void                InspectObject(Object* object);
    void                RotateObject(Object* object, float amount);
    void                ScaleObject(Object* object, float amount);
    void                ResetObject(Object* object);
    void                ChooseTile();
    void                ScrollTileSheet(int delta);

    Boxes               m_Boxes;
    sf::View            m_View;
    static EditObject   m_EditObject;
    Engine*        e;
    sf::Vector2f        m_viewPos;
    sf::Vector2i        m_mousePos;
    sf::Vector2f        m_worldPos;
    sf::Time            m_delta;

    int                 m_currTile = 777;
    Object*             m_dragPtr;
    Object              m_dragObject;

    Object              m_activeObject;
    std::string         m_InspectDetails;

    bool                m_showTiles = false;
    sf::Sprite          m_tilesheet;

    const float         SCALE_INCREMENT = 0.1f;
    const float         ROTATE_INCREMENT = 22.5;
};

#endif
