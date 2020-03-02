#include "State_Splash.h"
#include "gameengine.h"

int main(int argc, char *argv[]) {
  CGameEngine game;

  // initialize the engine
  game.Init("drill", 1280, 720);

  // load the first state
  game.ChangeState(CSplashState::Instance());

  // main loop
  while (game.Running()) {
    game.HandleEvents();
    game.Update();
    game.Draw();
  }

  // cleanup the engine
  game.Cleanup();

  return 0;
}
