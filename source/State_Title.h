#pragma once

#include <SDL.h>
#include <SDL_mixer.h>

#include "LTexture.h"
#include "State_Game.h"

class CTitleState : public CGameState {
 public:
  void Init(CGameEngine* game);
  void Cleanup();

  void Pause();
  void Resume();

  void HandleEvents(CGameEngine* game);
  void Update(CGameEngine* game);
  void Draw(CGameEngine* game);

  static CTitleState* Instance() { return &m_TitleState; }

 protected:
  CTitleState() {}

 private:
  static CTitleState m_TitleState;

  LTexture back_image;
};
