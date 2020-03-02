
#include <stdio.h>
#include <memory>

#include <SDL.h>
#include "CInput.h"
#include "State_Splash.h"
#include "State_Title.h"
#include "gameengine.h"

CSplashState CSplashState::m_SplashState;

void CSplashState::Init(CGameEngine* game) {
  printf("CSplashState Init\n");
  splash_complete = false;

  // works but does not show 'of course, namco'
  bool more_splashes = true;
  while (more_splashes) {
    std::string num_string = std::to_string(splash_images.size());
    std::shared_ptr<LTexture> temp(new LTexture);
    if (temp->loadPNGFromFile("romfs:/splash/" + num_string + ".png",
                              game->renderer, 0)) {
      splash_images.push_back(temp);
    } else {
      more_splashes = false;
    }
  }
  // make first splash image fully visible from the start
  splash_images[0]->setAlpha(255);
}

void CSplashState::Cleanup() { printf("CSplashState Cleanup\n"); }

void CSplashState::Pause() { printf("CSplashState Pause\n"); }

void CSplashState::Resume() { printf("CSplashState Resume\n"); }

void CSplashState::HandleEvents(CGameEngine* game) {
  // if button press, set all splash images to opaque immediately
  if (game->input.button[CInput::game_buttons::START]) {
    for (std::shared_ptr<LTexture>& this_splash : splash_images) {
      this_splash->setAlpha(255);
    }
  }
}

void CSplashState::Update(CGameEngine* game) {
  if (splash_complete) {
    // wait a bit, then go to the title screen
    SDL_Delay(3 * 1000);
    game->ChangeState(CTitleState::Instance());
  }

  // go through all the splash images
  // make each one visible if not yet, in order, one action every Update call
  // every time we find a texture not fully opaque
  // splash_complete is set to false
  for (std::shared_ptr<LTexture>& this_splash : splash_images) {
    if (this_splash->getAlpha() < 255) {
      this_splash->setAlpha(this_splash->getAlpha() + 3);
      splash_complete = false;
      break;
    } else {
      splash_complete = true;
    }
  }
}

void CSplashState::Draw(CGameEngine* game) {
  SDL_Color draw_color = {255, 255, 255, 255};

  SDL_SetRenderDrawColor(game->renderer, draw_color.r, draw_color.g,
                         draw_color.b, draw_color.a);
  SDL_RenderClear(game->renderer);

  for (std::shared_ptr<LTexture>& this_splash : splash_images) {
    this_splash->render(0, 0, game->renderer, nullptr);
  }

  SDL_RenderPresent(game->renderer);
}
