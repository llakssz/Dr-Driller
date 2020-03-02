#pragma once

#include <SDL.h>
#include <cmath>
#include <vector>

// thanks krazydad https://krazydad.com/tutorials/makecolors.php
std::vector<SDL_Color> getColorGradient(float frequency1, float frequency2,
                                        float frequency3, int phase1,
                                        int phase2, int phase3,
                                        int center = 128, int width = 127,
                                        int len = 50) {
  std::vector<SDL_Color> gradient_vector;
  for (int i = 0; i < len; i++) {
    Uint8 r = sin(frequency1 * i + phase1) * width + center;
    Uint8 g = sin(frequency2 * i + phase2) * width + center;
    Uint8 b = sin(frequency3 * i + phase3) * width + center;
    SDL_Color color = {r, g, b, 255};
    gradient_vector.push_back(color);
  }
  return gradient_vector;
}
