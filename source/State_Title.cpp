
#include <stdio.h>

#include <SDL.h>
#include "CInput.h"
#include "State.h"

#include "State_CharaSelect.h"
#include "State_Title.h"
#include "gameengine.h"

CTitleState CTitleState::m_TitleState;

void CTitleState::Init(CGameEngine* game) {
  printf("CTitleState Init\n");
  back_image.loadPNGFromFile("romfs:/title.png", game->renderer);

  game->audio.Load_Music("romfs:/audio/bgm/bank_bgm_01_00001.ogg");
  game->audio.Play_Music();
}

void CTitleState::Cleanup() { printf("CTitleState Cleanup\n"); }

void CTitleState::Pause() { printf("CTitleState Pause\n"); }

void CTitleState::Resume() { printf("CTitleState Resume\n"); }

void CTitleState::HandleEvents(CGameEngine* game) {
  if (game->input.button[CInput::game_buttons::CANCEL]) {
    game->Quit();
  }
  if ((game->input.button[CInput::game_buttons::START]) ||
      (game->input.button[CInput::game_buttons::OK])) {
    game->ChangeState(CCharaSelectState::Instance());
  }
}

void CTitleState::Update(CGameEngine* game) {}

void CTitleState::Draw(CGameEngine* game) {
  SDL_Color draw_color = {255, 255, 255, 255};

  SDL_SetRenderDrawColor(game->renderer, draw_color.r, draw_color.g,
                         draw_color.b, draw_color.a);
  SDL_RenderClear(game->renderer);

  back_image.render(0, 0, game->renderer, nullptr);

  SDL_RenderPresent(game->renderer);
}
