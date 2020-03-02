#include "Animation.h"

Animation::Animation() : m_texture(NULL) {}

Animation::~Animation() {}

void Animation::addFrame(SDL_Rect rect) { m_frames.push_back(rect); }

void Animation::setSpriteSheet(const LTexture& texture) {
  m_texture = &texture;
}

const LTexture* Animation::getSpriteSheet() const { return m_texture; }

std::size_t Animation::getSize() const { return m_frames.size(); }

SDL_Rect& Animation::getFrame(std::size_t n) { return m_frames[n]; }