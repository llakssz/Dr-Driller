#pragma once

#include <SDL.h>
#include <stdlib.h>
#include <string.h>
#include <cstdio>
#include <memory>
#include <vector>

#define MAX_BUTTONS 20

class CInput {
 public:
  CInput();
  ~CInput();

  // if use enum class, you can't really use it as an integer...
  // enum class game_buttons {
  enum game_buttons {
    UP = 0,
    DOWN = 1,
    LEFT = 2,
    RIGHT = 3,
    OK = 4,
    CANCEL = 5,
    START = 6,

    P2_UP = 7,
    P2_DOWN = 8,
    P2_LEFT = 9,
    P2_RIGHT = 10,
    P2_OK = 11,
    P2_CANCEL = 12,
    P2_START = 13

  };

  void CInputUpdate(int);

  // whether button is pressed
  bool button[MAX_BUTTONS];

 private:
  // whether button is down on this check
  bool down[MAX_BUTTONS];
  // whether button was down on last check
  bool prev_down[MAX_BUTTONS];
  // if button is allowed to be held down
  bool allow_repeat[MAX_BUTTONS];

  SDL_Joystick *controller1;
  // SDL_Joystick *controller2;
};
