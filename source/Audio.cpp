#include "Audio.h"

Audio::Audio() {}

Audio::~Audio() {}

void Audio::Load_Music(std::string path) {
  current_bgm = Mix_LoadMUS(path.c_str());
}
void Audio::Play_Music(int channel) { Mix_PlayMusic(current_bgm, channel); }
void Audio::Stop_Music() { Mix_HaltMusic(); }
void Audio::Free_Music() {
  Mix_FreeMusic(current_bgm);
  current_bgm = nullptr;
}
