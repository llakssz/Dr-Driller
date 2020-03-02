#include "LTexture.h"

#include "gameengine.h"
#include "lodepng.h"

LTexture::LTexture() {
  // Initialize
  mTexture = nullptr;
  mWidth = 0;
  mHeight = 0;
  // we have these as a paramter... so why twice?
  // mAlpha = 255;

  //  setRenderer(CGameEngine engine);
}

LTexture::~LTexture() {
  // Deallocate
  freeit();
}

void LTexture::setRenderer(CGameEngine* engine) {
  //  gRenderer = engine->renderer;
}

//
// bool LTexture::loadFromFile(std::string path, SDL_Renderer renderer)
//{
//  //Get rid of preexisting texture
//	freeit();
//
//  //The final texture
//  SDL_Texture* newTexture = nullptr;
//
//  //Load image at specified path
////  SDL_Surface* loadedSurface = load_PNG(path.c_str());
//  SDL_Surface* loadedSurface = IMG_Load(path.c_str());
//  if (loadedSurface == nullptr)
//  {
//    printf("Unable to load image %s! SDL_image Error: %s\n", path.c_str(),
//    IMG_GetError());
//	printf("i hope i see the line above");
//  }
//  else
//  {
//    //Color key image
//    SDL_SetColorKey(loadedSurface, SDL_TRUE, SDL_MapRGB(loadedSurface->format,
//    0, 0xFF, 0xFF));
//
//    //Create texture from surface pixels
//    newTexture = SDL_CreateTextureFromSurface(gRenderer, loadedSurface);
//    if (newTexture == nullptr)
//    {
//      printf("Unable to create texture from %s! SDL Error: %s\n",
//      path.c_str(), SDL_GetError());
//    }
//    else
//    {
//      //Get image dimensions
//      mWidth = loadedSurface->w;
//      mHeight = loadedSurface->h;
//    }
//
//    //Get rid of old loaded surface
//    SDL_FreeSurface(loadedSurface);
//  }
//
//  //Return success
//  mTexture = newTexture;
//  return mTexture != nullptr;
//}

bool LTexture::loadPNGFromFile(std::string path, SDL_Renderer* renderer,
                               Uint8 alpha) {
  // Get rid of preexisting texture
  freeit();
  mAlpha = alpha;

  // masks read about it later
  int logo_bpp = 0;
  Uint32 rmask = 0, gmask = 0, bmask = 0, amask = 0;

  // PREPARE COLOR MASKS FOR LOGO SURFACE:
  if (SDL_PixelFormatEnumToMasks(SDL_PIXELFORMAT_ABGR8888, &logo_bpp, &rmask,
                                 &gmask, &bmask, &amask) == SDL_FALSE) {
    // Failure ...
    printf(
        "Failed to determine color masks from SDL_PIXELFORMAT_ABGR8888 "
        "(\"%s\").\n",
        SDL_GetError());
  }

  // The final texture
  SDL_Texture* newTexture = nullptr;

  // to see if works or not
  bool lodepng_result = 0;

  unsigned char* logo_buffer = nullptr;

  SDL_Surface* logo_surface = nullptr;

  // LOAD LOGO PNG IMAGE INTO A BYTE BUFFER & ULTIMATELY CONVERT TO A TEXTURE
  // FOR RENDERING:
  // ... this always decodes to 32-bit ABGR raw image.
  lodepng_result =
      lodepng_decode32_file(&logo_buffer, &mWidth, &mHeight, path.c_str());
  if (lodepng_result != 0) {
    // Failure ...
    printf("Failed lodepng_decode32_file() (\"%s\") for \"%s.\"\n",
           lodepng_error_text(lodepng_result), path.c_str());
  } else {
    // Success (Continue Preparing Logo, we need a surface first to work with
    // it)... NOTE: SDL_CreateRGBSurfaceFrom() does not copy the buffer, but
    // uses the existing buffer that we're managing.
    if ((logo_surface = SDL_CreateRGBSurfaceFrom(
             (void*)logo_buffer, mWidth, mHeight,
             logo_bpp,    // depth or bits per pixel (4 channels X 8 bits per
                          // channel)
             mWidth * 4,  // pitch (width X 4 channels)
             rmask, gmask, bmask, amask  // color masks
             )) == 0) {
      // Failure ...
      free(logo_buffer);
      printf("Failed to create surface for logo image buffer (\"%s\").\n",
             SDL_GetError());
    } else {
      // Success (Convert the logo surface to a texture so it's usable) ...
      mTexture = SDL_CreateTextureFromSurface(renderer, logo_surface);

      // Clean-up the logo surface & buffer, it's no longer needed:
      SDL_FreeSurface(logo_surface);
      free(logo_buffer);  // SDL_FreeSurface() does not do this for us.
      logo_surface = NULL;
      logo_buffer = NULL;

      // Now it's a good time to check if texture creation failed:
      if (mTexture == NULL) {
        printf("Failed to create texture from logo surface (\"%s\").\n",
               SDL_GetError());
      }
    }
  }

  return mTexture != nullptr;
}

void LTexture::freeit() {
  // Free texture if it exists
  if (mTexture != nullptr) {
    SDL_DestroyTexture(mTexture);
    mTexture = nullptr;
    mWidth = 0;
    mHeight = 0;
    mAlpha = 0;
  }
}

void LTexture::setColor(Uint8 red, Uint8 green, Uint8 blue) {
  // Modulate texture rgb
  SDL_SetTextureColorMod(mTexture, red, green, blue);
}

void LTexture::setBlendMode(SDL_BlendMode blending) {
  // Set blending function
  SDL_SetTextureBlendMode(mTexture, blending);
}

void LTexture::setAlpha(int alpha) {
  mAlpha = alpha;
  if (mAlpha > 255) {
    mAlpha = 255;
  }
}
int LTexture::getAlpha() { return mAlpha; }

void LTexture::render(int x, int y, SDL_Renderer* renderer, SDL_Rect* clip) {
  // Set rendering space and render to screen
  SDL_Rect renderQuad = {x, y, (int)mWidth, (int)mHeight};

  // Set clip rendering dimensions
  if (clip != nullptr) {
    renderQuad.w = clip->w;
    renderQuad.h = clip->h;
  }

  SDL_SetTextureAlphaMod(mTexture, mAlpha);

  // Render to screen
  SDL_RenderCopy(renderer, mTexture, clip, &renderQuad);
}

int LTexture::getWidth() { return mWidth; }

int LTexture::getHeight() { return mHeight; }

SDL_Texture* LTexture::getTex() { return mTexture; }