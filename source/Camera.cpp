#include "Camera.h"

float lerp(float a, float b, float f) { return (a * (1.0 - f)) + (b * f); }

Camera::Camera() {
  x = 0;
  y = 0;
  w = 0;
  h = 0;

  y_lerp_from = 0;
  y_lerp_to = 0;

  needlerp = false;
}

Camera::~Camera() {}

void Camera::Transition(int newy) {
  needlerp = true;
  lerpval = 0.0;

  y_lerp_from = y;
  y_lerp_to = newy;
}

void Camera::HardChange(int newy) {
  needlerp = false;
  y = newy;
}

void Camera::Update(int updatey) {
  // if position passed to function has already been passed,
  if (updatey == y_lerp_to) {
    if (needlerp) {
      lerpval += 0.1;
      y = lerp(y_lerp_from, y_lerp_to, lerpval);
      if (lerpval >= 1.0) {
        needlerp = false;
        lerpval = 0.0;
      }
    }
  } else {
    y = updatey;
  }
}
