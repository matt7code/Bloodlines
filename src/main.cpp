#include <iostream>

#include "constants.h"
#include "gameengine.h"
#include "states/intro.h"

/// TEST
//#include "states/edit_anim.h"

int main ( int argc, char *argv[] ) {
    Engine game;
#ifdef DEBUG
    game.Init( "Bloodlines v2.0 (Debug)", SCREEN_X_DEBUG, SCREEN_Y_DEBUG, BPP, false);
#endif
#ifdef FINAL
    game.Init( "Bloodlines v2.0 (Final)", SCREEN_X_FINAL, SCREEN_Y_FINAL, BPP, true );
#endif
/// TEST
    game.ChangeState( CIntroState::Instance() );
    //game.ChangeState( Edit_Anim::Instance() );

    while ( game.Running() ) {
        game.HandleEvents();
        game.Update();
        game.Draw();
    }
    game.Cleanup();

#ifdef DEBUG
    std::cout << "You took " << game.getTimeRunningString() << "to move " << game.getParty().getPartySteps() << " steps this session." << std::endl;
#endif
    return 0;
}
