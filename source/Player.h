#pragma once

#define DIR_DOWN 0
#define DIR_UP 1
#define DIR_LEFT 2
#define DIR_RIGHT 3
#define DIR_NONE 4

#include <SDL.h>
#include <string>

#include "Animation.h"

class Player {
 public:
  enum class anim_actions {
    angel,
    crushleft,
    crushright,
    dodgebackleft,
    dodgebackright,
    dodgefrontleft,
    dodgefrontright,
    drilldown,
    drillleft,
    drillright,
    drillup,
    drillwalkingleft,
    drillwalkingright,
    fall,
    goal,
    jumpleft,
    jumpright,
    noairleft,
    noairright,
    revive,
    walkleft,
    walkright,
    xyz,
  };

  Player();
  ~Player();

  void draw(int x, int y, SDL_Rect clip, SDL_Renderer* renderer);

  bool initanim(std::string json, std::string png);

  // returns when angel is done, bring back to life
  bool angelrise(int y, SDL_Renderer* renderer);

  SDL_Rect update_anim();

  void change_anim(Animation& anim, bool loop = false);

  // set anim to 1st frame
  void reset_anim();

  // remember many of these ints are actually bools, fix later

  float WALKSPEED = 4;
  float WALKSPEED_Y = 4;

  // orginal values
  // int WALKSPEED = 4;
  // int WALKSPEED_Y = 6;

  int my_x;
  int my_y;
  float angely;

  int vx;
  int vy;

  int repeat_x;
  int repeat_y;

  bool climbing;
  bool candbljump;
  bool isdlbjumping;

  int got_air;

  bool dig;
  int dig_repeat;
  int dig_graphic;
  bool cantripledig;
  int dig_timer;

  int my_depth;
  int my_score;
  int my_air;

  bool dead;
  int angelcount;
  int deadcount;

  int direction;

  int airminus;
  int airminuscount;
  int airplus;

  int airdowncount;

  int airdownspeed;

  int movingframe;

  int penaltyframe;
  int penaltybgnum;

  int fallingframe;

  // if player 1, player 2, etc
  int num;

  // for what current frame we are on in the current animation
  int framenumber;
  bool loopanim;
  bool pauseanim;
  bool walking;
  bool anim_finished;

  bool climbing_wait;

  // when about to climb, the direction you are facing is stored here
  // if you change direction before you start climbing, stop the climb and walk
  // normally
  int previous_direction;

  Animation* currentAnimation;
  Animation* prevAnimation;

  vector<Animation> animations;
  // Animation angel;
  // Animation crushleft;
  // Animation crushright;
  // Animation dodgebackleft;
  // Animation dodgebackright;
  // Animation dodgefrontleft;
  // Animation dodgefrontright;
  // Animation drilldown;
  // Animation drillleft;
  // Animation drillright;
  // Animation drillup;
  // Animation drillwalkingleft;
  // Animation drillwalkingright;
  // Animation fall;
  // Animation goal;
  // Animation jumpleft;
  // Animation jumpright;
  // Animation noairleft;
  // Animation noairright;
  // Animation revive;
  // Animation walkleft;
  // Animation walkright;
  // Animation xyz;

  LTexture m_texture;
};
