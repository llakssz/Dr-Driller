#pragma once

#include <SDL.h>
#include "State_Game.h"

#include "LTexture.h"

class CPauseState : public CGameState {
 public:
  void Init(CGameEngine* game);
  void Cleanup();

  void Pause();
  void Resume();

  void HandleEvents(CGameEngine* game);
  void Update(CGameEngine* game);
  void Draw(CGameEngine* game);

  static CPauseState* Instance() { return &m_PauseState; }

 protected:
  CPauseState() {}

 private:
  static CPauseState m_PauseState;

  //	SDL_Surface* bg;
  LTexture pausebg;
};
