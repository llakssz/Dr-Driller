#pragma once

#include <SDL.h>
#include <SDL_image.h>

#include <string>

#include "gameengine.h"

class LTexture {
 public:
  // Initializes variables
  LTexture();

  // Deallocates memory
  ~LTexture();

  void setRenderer(CGameEngine* engine);

  // Loads image at specified path
  bool loadFromFile(std::string path);

  // Loads image at specified path
  bool loadPNGFromFile(std::string path, SDL_Renderer* renderer,
                       Uint8 alpha = 255);

  // Deallocates texture
  void freeit();

  // Set color modulation
  void setColor(Uint8 red, Uint8 green, Uint8 blue);

  // Set blending
  void setBlendMode(SDL_BlendMode blending);

  // Set alpha modulation
  void setAlpha(int alpha);
  int getAlpha();

  // Renders texture at given point
  void render(int x, int y, SDL_Renderer* renderer, SDL_Rect* clip = nullptr);

  // Gets image dimensions
  int getWidth();
  int getHeight();

  SDL_Texture* getTex();

 private:
  // The actual hardware texture
  SDL_Texture* mTexture;

  // Image dimensions
  unsigned int mWidth;
  unsigned int mHeight;

  // 0 - 255
  int mAlpha;

  // at the moment every ltexture object will have it's own renderer member that
  // points to the global renderer - silly.

  // here I will set the pointer to the render to the main render in the
  // gameengine class
  //  SDL_Renderer* gRenderer = nullptr;
};
