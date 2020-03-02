
#include <stdio.h>

#include <SDL.h>
#include "CInput.h"
#include "State_Game.h"
#include "State_Pause.h"
#include "gameengine.h"

CPauseState CPauseState::m_PauseState;

void CPauseState::Init(CGameEngine* game) {
  printf("CPauseState Init\n");

  pausebg.loadPNGFromFile("romfs:/pause/dl_main2x.png", game->renderer);
}

void CPauseState::Cleanup() { printf("CPauseState Cleanup\n"); }

void CPauseState::Pause() { printf("CPauseState Pause\n"); }

void CPauseState::Resume() { printf("CPauseState Resume\n"); }

void CPauseState::HandleEvents(CGameEngine* game) {
  if (game->input.button[CInput::game_buttons::START]) {
    game->PopState();
  }
}

void CPauseState::Update(CGameEngine* game) {}

void CPauseState::Draw(CGameEngine* game) {
  pausebg.render((1280 / 2) - (pausebg.getWidth() / 2),
                 (720 / 2) - (pausebg.getHeight() / 2), game->renderer,
                 nullptr);

  SDL_RenderPresent(game->renderer);
}
