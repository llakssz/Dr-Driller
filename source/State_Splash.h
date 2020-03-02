#pragma once

#include <SDL.h>
#include <memory>
#include "LTexture.h"
#include "State_Game.h"

class CSplashState : public CGameState {
 public:
  void Init(CGameEngine* game);
  void Cleanup();

  void Pause();
  void Resume();

  void HandleEvents(CGameEngine* game);
  void Update(CGameEngine* game);
  void Draw(CGameEngine* game);

  static CSplashState* Instance() { return &m_SplashState; }

 protected:
  CSplashState() {}

 private:
  static CSplashState m_SplashState;

  // flag will be true when splash screen is finished
  bool splash_complete;

  std::vector<std::shared_ptr<LTexture>> splash_images;
};