#pragma once

#include <SDL.h>

#include <vector>

#include "Audio.h"
#include "CInput.h"
#include "Settings.h"

using namespace std;

class CGameState;

class CGameEngine {
 public:
  void Init(const char* title, int width = 640, int height = 480, int bpp = 0,
            bool fullscreen = false);
  void Cleanup();

  void ChangeState(CGameState* state);
  void PushState(CGameState* state);
  void PopState();

  void HandleEvents();
  void Update();
  void Draw();

  bool Running() { return m_running; }
  void Quit() { m_running = false; }

  //	SDL_Surface* screen;

  // should I not even initialize to nullptr in the header? do it in the init
  // function?
  SDL_Window* window = nullptr;
  SDL_Renderer* renderer = nullptr;

  // add event to the gameengine so that we can and use the input class easily
  // SDL_Event* event = nullptr;
  // actually I think I just want gameengine to have a CInput member... right,
  // and use that game->input->button[CONFIRM] game->input->button[BACK]

  // input class that handles input and can be accessed from all states
  CInput input;

  Settings settings;

  Audio audio;

 private:
  // the stack of states
  vector<CGameState*> states;

  bool m_running;
  bool m_fullscreen;
};
