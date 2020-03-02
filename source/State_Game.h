#pragma once

#include <SDL.h>
#include "Stage.h"
#include "State.h"
#include "State_Game.h"

class CPlayState : public CGameState {
 public:
  void Init(CGameEngine* game);
  void Cleanup();

  void Pause();
  void Resume();

  void HandleEvents(CGameEngine* game);
  void Update(CGameEngine* game);
  void Draw(CGameEngine* game);

  static CPlayState* Instance() { return &m_PlayState; }

 protected:
  CPlayState() {}

 private:
  static CPlayState m_PlayState;

  SDL_Surface* bg;

  Stage drillgame;
};
