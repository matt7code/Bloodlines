#ifndef GAMESTATE_H
#define GAMESTATE_H

#include "gameengine.h"

class GameState {
public:
    virtual void Init(Engine* Ptr_engine)          = 0;
    virtual void Cleanup(Engine* Ptr_engine)       = 0;
    virtual void Pause(Engine* Ptr_engine)         = 0;
    virtual void Resume(Engine* Ptr_engine)        = 0;
    virtual void HandleEvents(Engine* Ptr_engine)  = 0;
    virtual void Update(Engine* Ptr_engine)        = 0;
    virtual void Draw(Engine* Ptr_engine)          = 0;
    void ChangeState(Engine* Ptr_engine, GameState* state) {
        Ptr_engine->ChangeState(state);
    }
protected:
    GameState() {}
};

#endif

