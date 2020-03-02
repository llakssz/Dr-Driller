#pragma once

#include <SDL.h>
#include "LTexture.h"
#include "State_Game.h"

class CCharaSelectState : public CGameState {
 public:
  void Init(CGameEngine* game);
  void Cleanup();

  void Pause();
  void Resume();

  void HandleEvents(CGameEngine* game);
  void Update(CGameEngine* game);
  void Draw(CGameEngine* game);

  static CCharaSelectState* Instance() { return &m_CharaSelectState; }

 protected:
  CCharaSelectState() {}

 private:
  static CCharaSelectState m_CharaSelectState;

  LTexture back_image;
  LTexture curr_selection;
  LTexture chara_icons[7];

  // center point of each circle where character icon is drawn
  SDL_Point circle_pos[7];

  // map to facilitate moving around the screen to select a character
  int select_map[3][3] = {{0, -1, 1}, {2, 6, 5}, {3, -1, 4}};
  int char_row = 0;
  int char_col = 0;

  int input_delay = 0;
};
