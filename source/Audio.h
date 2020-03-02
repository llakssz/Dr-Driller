#pragma once

#include <SDL_mixer.h>
#include <string>

class Audio {
 public:
  Audio();
  ~Audio();

  void Load_Music(std::string path);
  void Play_Music(int channel = -1);
  void Stop_Music();
  void Free_Music();

  Mix_Music* current_bgm;
};
