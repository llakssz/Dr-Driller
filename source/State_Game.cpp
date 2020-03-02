
//#include <stdio.h>
#include <cstdio>
#include <random>

#include <SDL.h>
#include <SDL_image.h>

#include "Audio.h"
#include "LTexture.h"
#include "State.h"
#include "State_Game.h"
#include "State_Pause.h"
#include "gameengine.h"

// #include "Extra.h"

//#include "lodepng.h"

// Current time start time
Uint32 startTime = 0;

CPlayState CPlayState::m_PlayState;

// Current animation frame
int frame = 0;

// for fps stuff
int fpsframes = 0;

void CPlayState::Init(CGameEngine* game) {
  startTime = SDL_GetTicks();

  game->audio.Load_Music("romfs:/audio/bgm/bank_bgm_02_00001.ogg");
  game->audio.Play_Music();

  drillgame.settings = &game->settings;
  drillgame.game_ready(game->renderer);

  printf("CPlayState Init\n");
}

void CPlayState::Cleanup() {
  SDL_FreeSurface(bg);

  printf("CPlayState Cleanup\n");
}

void CPlayState::Pause() {
  printf("CPlayState Pause\n");
  Mix_PauseMusic();
}

void CPlayState::Resume() {
  printf("CPlayState Resume\n");
  Mix_ResumeMusic();
}

void CPlayState::HandleEvents(CGameEngine* game) {
  if (game->input.button[CInput::game_buttons::START]) {
    game->PushState(CPauseState::Instance());
  }
}

void CPlayState::Update(CGameEngine* game) {
  fpsframes++;
  drillgame.mainloop(game->input, game->renderer);
}

void CPlayState::Draw(CGameEngine* game) {
  SDL_SetRenderDrawColor(game->renderer, 255, 255, 255, 255);
  SDL_RenderClear(game->renderer);

  drillgame.draw(game->renderer);

  SDL_RenderPresent(game->renderer);
}
