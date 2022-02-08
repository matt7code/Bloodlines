#ifndef EditNpcWaypoint_H
#define EditNpcWaypoint_H

#include <iostream>
#include <SFML/Graphics.hpp>
#include "constants.h"
#include "gamestate.h"
#include "boxes.h"
#include "tool.h"
#include "npc.h"

class EditNpcWaypoint : public GameState {
public:
    void Init(Engine* e);
    void Cleanup(Engine* e);
    void Pause(Engine* e) {}
    void Resume(Engine* e) {}
    void HandleEvents(Engine* e);
    void Update(Engine* e);
    void Draw(Engine* e);

    void MoveView(Engine* e);
    static EditNpcWaypoint* Instance() {
        return &m_EditNpcWaypoint;
    }
    void RefreshEditBox(Npc& npc);
    void UpdateStartPosition(Npc* ptr);

protected:
    EditNpcWaypoint() {}

private:
    enum {FIELD_NONE, FIELD_NAME, FIELD_TAG, FIELD_POSX, FIELD_POSY, FIELD_MOVEMODE, FIELD_AVATAR,
          FIELD_SPEED, FIELD_FACING, FIELD_COLOR, FIELD_VAL1, FIELD_VAL2, FIELD_VAL3, FIELD
         };
    enum {WP_FIELD_TAG, WP_FIELD_DIALOG, WP_FIELD_POSX, WP_FIELD_POSY, WP_FIELD_PAUSE, WP_FIELD_SPEED};
    enum {MODE_NONE, MODE_EDIT, MODE_INPUT, MODE_EDIT_WAYPOINT, MODE_INPUT_WAYPOINT};

    std::string getModeString(int mode);
    std::string getMoveModeString(int mode);
    void UpdateInput();

    void EditNpc(Engine *e);
    void GrabNpc(Engine *e);
    void DropNpc(Engine *e);
    void CreateNpc(Engine *e);

    void EditWaypoint(Engine *e);
    void GrabWaypoint(Engine *e);
    void DropWaypoint(Engine *e);
    void CreateWaypoint(Engine *e);

    void DrawWaypoints(Engine *e);
    void LoadWaypoints(Engine *e);
    void SaveWaypoints(Engine *e);

    void RefreshEditBoxWP(Waypoint& waypoint);

    sf::Vector2f GetMouse(Engine *e);

    Boxes                       m_Boxes;
    sf::View                    m_View;
    static EditNpcWaypoint      m_EditNpcWaypoint;
    sf::Time                    m_delta;
    sf::Vector2f                m_viewPos;
    Npc*                        m_npcPtr = nullptr;
    sf::Time                    m_LastClickTime;
    sf::Vector2f                m_LastClickPos;
    std::string                 m_tip;

    const int EDIT_BOX_TOP      = 600;
    const int EDIT_BOX_LEFT     = 50;
    const int EDIT_BOX_WIDTH    = 400;
    const int EDIT_BOX_HEIGHT   = 430;

    const int INPUT_BOX_TOP     = 500;
    const int INPUT_BOX_LEFT    = 760;
    const int INPUT_BOX_WIDTH   = 400;
    const int INPUT_BOX_HEIGHT  = 50;

    const int TIP_BOX_TOP       = 575;
    const int TIP_BOX_LEFT      = 800;

    /// Text Entry
    std::string m_input         = "";
    int m_inputField            = FIELD_NONE;
    int m_mode                  = MODE_NONE;

    sf::Sprite avatar_sheet;
    sf::Texture avatar_sheet_texture;
    //sf::Vertex v1, v2;
    //sf::VertexArray line(sf::Lines, 2);
    // line[0] = v1;
    // line[1] = v2;
    //sf::VertexArray         m_WP_Lines;
    unsigned                m_wp_color = 0;
    sf::CircleShape         waypoint;
    Waypoint*               m_wpPtr = nullptr;
};

#endif
