
#include <SDL.h>
#include <SDL_mixer.h>
#include <switch.h>
#include <cstdio>

#include "State.h"
#include "gameengine.h"

void CGameEngine::Init(const char* title, int width, int height, int bpp,
                       bool fullscreen) {
  int flags = 0;

  socketInitializeDefault();
  nxlinkStdio();

  // initialize SDL
  // SDL_Init(SDL_INIT_EVERYTHING);
  if (SDL_Init(SDL_INIT_VIDEO | SDL_INIT_JOYSTICK | SDL_INIT_AUDIO) < 0) {
    SDL_Log("SDL_Init: %s\n", SDL_GetError());
    SDL_Quit();
  }

  if (fullscreen) {
    flags = SDL_WINDOW_FULLSCREEN;
  }

  // create the screen surface (now a window, in sdl 2)
  window = SDL_CreateWindow("sdl2_gles2", 0, 0, width, height, 0);
  if (!window) {
    SDL_Log("SDL_CreateWindow: %s\n", SDL_GetError());
    SDL_Quit();
  }

  // renderer = SDL_CreateRenderer(main_window, -1, SDL_RENDERER_PRESENTVSYNC);
  renderer = SDL_CreateRenderer(
      window, 0, SDL_RENDERER_ACCELERATED | SDL_RENDERER_PRESENTVSYNC);
  if (!renderer) {
    SDL_Log("SDL_CreateRenderer: %s\n", SDL_GetError());
    SDL_Quit();
  }

  Mix_Init(MIX_INIT_OGG);
  // Initialize SDL_mixer
  if (Mix_OpenAudio(44100, MIX_DEFAULT_FORMAT, 2, 2048) < 0) {
    SDL_Log("Mix_OpenAudio: %s\n", Mix_GetError());
  }

  m_fullscreen = fullscreen;
  m_running = true;

  printf("CGameEngine Init\n");

  romfsInit();
}

void CGameEngine::Cleanup() {
  // cleanup the all states
  while (!states.empty()) {
    states.back()->Cleanup();
    states.pop_back();
  }

  // switch back to windowed mode so other
  // programs won't get accidentally resized
  //	if ( m_fullscreen ) {
  //		screen = SDL_SetVideoMode(640, 480, 0, 0);
  //	}

  printf("CGameEngine Cleanup\n");

  SDL_DestroyRenderer(renderer);
  SDL_DestroyWindow(window);

  // shutdown SDL
  Mix_Quit();
  SDL_Quit();
  romfsExit();
  socketExit();
}

void CGameEngine::ChangeState(CGameState* state) {
  // cleanup the current state
  if (!states.empty()) {
    states.back()->Cleanup();
    states.pop_back();
  }

  // store and init the new state
  states.push_back(state);
  states.back()->Init(this);
}

void CGameEngine::PushState(CGameState* state) {
  // pause current state
  if (!states.empty()) {
    states.back()->Pause();
  }

  // store and init the new state
  states.push_back(state);
  states.back()->Init(this);
}

void CGameEngine::PopState() {
  // cleanup the current state
  if (!states.empty()) {
    states.back()->Cleanup();
    states.pop_back();
  }

  // resume previous state
  if (!states.empty()) {
    states.back()->Resume();
  }
}

void CGameEngine::HandleEvents() {
  input.CInputUpdate(0);

  // let the state handle events
  states.back()->HandleEvents(this);
}

void CGameEngine::Update() {
  // let the state update the game
  states.back()->Update(this);
}

void CGameEngine::Draw() {
  // let the state draw the screen
  states.back()->Draw(this);

  //	printf("drawing\n");
}
