#pragma once

class Camera {
 public:
  Camera();
  ~Camera();

  float x;
  float y;
  int w;
  int h;

  float y_lerp_from;
  float y_lerp_to;

  bool needlerp;
  float lerpval;

  // sets lerp destination
  void Transition(int y);

  // forces change immediately
  void HardChange(int y);

  // updates with lerp if needed
  void Update(int y);
};
