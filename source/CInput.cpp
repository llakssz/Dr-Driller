#include "CInput.h"

CInput::CInput() {
  for (int i = 0; i < MAX_BUTTONS; i++) {
    button[i] = 0;
    down[i] = 0;
    prev_down[i] = 0;
    allow_repeat[i] = 0;
  }

  controller1 = SDL_JoystickOpen(0);

  allow_repeat[CInput::game_buttons::LEFT] = 1;
  allow_repeat[CInput::game_buttons::RIGHT] = 1;
  allow_repeat[CInput::game_buttons::UP] = 1;
  allow_repeat[CInput::game_buttons::DOWN] = 1;

  allow_repeat[CInput::game_buttons::P2_LEFT] = 1;
  allow_repeat[CInput::game_buttons::P2_RIGHT] = 1;
  allow_repeat[CInput::game_buttons::P2_UP] = 1;
  allow_repeat[CInput::game_buttons::P2_DOWN] = 1;
}

CInput::~CInput() {}

void CInput::CInputUpdate(int disabled) {
  // if a button was pressed, save so we can compare if still pressed
  for (int i = 0; i < MAX_BUTTONS; ++i) {
    prev_down[i] = down[i];
  }

  SDL_Event event;
  while (SDL_PollEvent(&event)) {
    down[CInput::game_buttons::LEFT] = SDL_JoystickGetButton(controller1, 16);
    down[CInput::game_buttons::UP] = SDL_JoystickGetButton(controller1, 17);
    down[CInput::game_buttons::RIGHT] = SDL_JoystickGetButton(controller1, 18);
    down[CInput::game_buttons::DOWN] = SDL_JoystickGetButton(controller1, 19);
    down[CInput::game_buttons::OK] = SDL_JoystickGetButton(controller1, 6);
    down[CInput::game_buttons::CANCEL] = SDL_JoystickGetButton(controller1, 8);
    down[CInput::game_buttons::START] = SDL_JoystickGetButton(controller1, 11);

    down[CInput::game_buttons::P2_LEFT] =
        SDL_JoystickGetButton(controller1, 20);
    down[CInput::game_buttons::P2_UP] = SDL_JoystickGetButton(controller1, 21);
    down[CInput::game_buttons::P2_RIGHT] =
        SDL_JoystickGetButton(controller1, 22);
    down[CInput::game_buttons::P2_DOWN] =
        SDL_JoystickGetButton(controller1, 23);
    down[CInput::game_buttons::P2_OK] = SDL_JoystickGetButton(controller1, 7);
    down[CInput::game_buttons::P2_CANCEL] =
        SDL_JoystickGetButton(controller1, 9);
    down[CInput::game_buttons::P2_START] =
        SDL_JoystickGetButton(controller1, 10);
  }

  // if the button is down, and it was previously down, and it's allowed to
  // repeat
  for (int i = 0; i < MAX_BUTTONS; ++i) {
    if ((down[i]) && (!prev_down[i])) {
      // if button is pressed and wasn't pressed last time, button is good
      button[i] = true;
    } else if ((down[i]) && (prev_down[i]) && (allow_repeat[i])) {
      // if button is pressed and it was pressed last time
      // but this button is allowed to repeat, also good
      button[i] = true;
    } else {
      // otherwise, ignore this button press
      button[i] = false;
    }
  }
}
