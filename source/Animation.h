#pragma once

#include <SDL.h>
#include <SDL_image.h>
#include <vector>

#include "LTexture.h"

class Animation {
 public:
  Animation();
  ~Animation();

  void addFrame(SDL_Rect rect);
  void setSpriteSheet(const LTexture& texture);
  const LTexture* getSpriteSheet() const;
  std::size_t getSize() const;
  SDL_Rect& getFrame(std::size_t n);

 private:
  std::vector<SDL_Rect> m_frames;
  const LTexture* m_texture;
};
