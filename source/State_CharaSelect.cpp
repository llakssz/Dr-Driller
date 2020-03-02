
#include <stdio.h>

#include <SDL.h>
#include "Audio.h"
#include "State.h"
#include "State_CharaSelect.h"
#include "State_Game.h"
#include "gameengine.h"

CCharaSelectState CCharaSelectState::m_CharaSelectState;

void CCharaSelectState::Init(CGameEngine* game) {
  circle_pos[0] = {187, 227};
  circle_pos[1] = {393, 227};
  circle_pos[2] = {137, 372};
  circle_pos[3] = {187, 516};
  circle_pos[4] = {393, 516};
  circle_pos[5] = {444, 372};
  circle_pos[6] = {290, 372};

  back_image.loadPNGFromFile("romfs:/chara-select/back.png", game->renderer);
  curr_selection.loadPNGFromFile("romfs:/chara-select/selected.png",
                                 game->renderer);

  chara_icons[0].loadPNGFromFile("romfs:/chara-select/susumu_icon.png",
                                 game->renderer);
  chara_icons[1].loadPNGFromFile("romfs:/chara-select/anna_icon.png",
                                 game->renderer);
  chara_icons[2].loadPNGFromFile("romfs:/chara-select/puchi_icon.png",
                                 game->renderer);
  chara_icons[3].loadPNGFromFile("romfs:/chara-select/hori_icon.png",
                                 game->renderer);
  chara_icons[4].loadPNGFromFile("romfs:/chara-select/taizo_icon.png",
                                 game->renderer);
  chara_icons[5].loadPNGFromFile("romfs:/chara-select/ataru_icon.png",
                                 game->renderer);
  chara_icons[6].loadPNGFromFile("romfs:/chara-select/usagi_icon.png",
                                 game->renderer);
}

void CCharaSelectState::Cleanup() {}

void CCharaSelectState::Pause() {}

void CCharaSelectState::Resume() {}

void CCharaSelectState::HandleEvents(CGameEngine* game) {
  // if cancel pressed, return to title
  // if (game->input.button[0]) {
  // }

  // if ready, go to game
  if (game->input.button[CInput::game_buttons::OK]) {
    game->settings.chara_choice[0] = select_map[char_row][char_col];
    // I didn't finish the spritesheet/support for horinger-z, so set to susumu
    if (game->settings.chara_choice[0] == 3) {
      game->settings.chara_choice[0] = 0;
    }
    game->audio.Stop_Music();
    game->audio.Free_Music();
    game->ChangeState(CPlayState::Instance());
  }

  // don't allow moving too fast...
  // the real game does allow key repeats here, but, slowly
  if (input_delay == 0) {
    input_delay = 3;

    // right
    if (game->input.button[CInput::game_buttons::RIGHT]) {
      if (char_col < 2) {
        char_col++;
        if (select_map[char_row][char_col] == -1) {
          char_col++;
        }
      } else {
        if (char_row == 0) {
          char_row++;
        }
        if (char_row == 2) {
          char_row--;
        }
      }
    }
    // left
    if (game->input.button[CInput::game_buttons::LEFT]) {
      if (char_col > 0) {
        char_col--;
        if (select_map[char_row][char_col] == -1) {
          char_col--;
        }
      } else {
        if (char_row == 0) {
          char_row++;
        }
        if (char_row == 2) {
          char_row--;
        }
      }
    }
    // down
    if (game->input.button[CInput::game_buttons::DOWN]) {
      if (char_row < 2) {
        if (select_map[char_row + 1][char_col] != -1) {
          char_row++;
        }
      }
    }
    // up
    if (game->input.button[CInput::game_buttons::UP]) {
      if (char_row > 0) {
        if (select_map[char_row - 1][char_col] != -1) {
          char_row--;
        }
      }
    }
  } else {
    // decrement counter
    input_delay--;
  }
}

void CCharaSelectState::Update(CGameEngine* game) {}

void CCharaSelectState::Draw(CGameEngine* game) {
  SDL_Color draw_color = {255, 255, 255, 255};

  SDL_SetRenderDrawColor(game->renderer, draw_color.r, draw_color.g,
                         draw_color.b, draw_color.a);
  SDL_RenderClear(game->renderer);

  back_image.render(0, 0, game->renderer, nullptr);

  int curr_id = select_map[char_row][char_col];
  //   curr_id = 4;
  curr_selection.render(circle_pos[curr_id].x - curr_selection.getWidth() / 2,
                        circle_pos[curr_id].y - curr_selection.getHeight() / 2,
                        game->renderer, nullptr);

  for (int i = 0; i < 7; i++) {
    chara_icons[i].render(circle_pos[i].x - chara_icons[i].getWidth() / 2,
                          circle_pos[i].y - chara_icons[i].getHeight() / 2,
                          game->renderer, nullptr);
  }

  SDL_RenderPresent(game->renderer);
}
