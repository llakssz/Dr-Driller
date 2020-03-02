#pragma once

#include <vector>

#include "Audio.h"
#include "Block.h"
#include "CInput.h"
#include "Camera.h"
#include "Player.h"
#include "Settings.h"

struct view {
  float x;
  float y;
  int w;
  int h;
};

class Stage {
 public:
  Stage();
  ~Stage();

  void set_stage(int number, int percentage, int blockstyle);

  void set_stage_startingpoint(void);

  int BlitForBlock(int p, int num, int x, int y, SDL_Renderer *renderer,
                   int destroyed);

  int BlitForBlockpop(int p, int num, int x, int y, SDL_Renderer *renderer,
                      bool dug);

  void set_shape(void);

  void move(CInput input);

  int erase_check_optimisation(void);

  void erase_check(void);

  void erase_check_recursive(int x, int y, int type, int *answer, int *number);

  void seterase_recursive(int x, int y, int type);

  void prefallcheck(void);

  void leftcheck(int x, int y, int type, int *checkleft, int *check);

  void setleft(int x, int y, int type, int left);

  void blockprocess(void);
  void clear_blockflag();

  void clear_blockflag_sub(void);

  void search(int x, int y, int type);

  void self_destroy(int x, int y, Player &player);

  void destroy(int x, int y);

  void clearaftercrushed(Player &player);

  int erase_block(void);

  void search_number(int x, int y, int type, int *number);

  void search_fall(int x, int y, int type, int *checksheat, int *checkleft);

  void setprefall(int x, int y, int type, int left);

  void unsetprefall(int x, int y, int type);

  void unsetprefallfinished(int x, int y, int type);

  void stage_clear(void);

  void mainloop(CInput input, SDL_Renderer *renderer);

  void draw(SDL_Renderer *renderer);

  void draw_back(SDL_Renderer *renderer);

  void draw_blocks(SDL_Renderer *renderer);
  void draw_me(SDL_Renderer *renderer);

  void draw_other(void);

  void draw_screen(SDL_Renderer *renderer);
  void draw_air(SDL_Renderer *renderer);
  void other_move();
  int atarihantei(Player &player, SDL_Renderer *renderer);

  void game_ready(SDL_Renderer *renderer);

  CInput gameinput;

  std::vector<Player> players_vec;
  Player *leadplayer;

  int STAGE_WIDTH;
  int STAGE_HEIGHT;

  int GAME_STAGE_WIDTH;
  int GAME_STAGE_HEIGHT;

  int STAGE_START_Y;

  // Block gamestage[22][100 + 10];
  // std::vector< std::vector<Block> > gamestage2;
  Block **gamestage;

  Camera gameCam;

  int PREFALLTIME;

  int scoreplus;
  int scorerest;

  int my_clear;
  int my_clearcount;
  int my_fps;
  int my_time;

  int lap_showing;

  // int penaltybgnum;
  // int penaltyframe;
  // int fallingframe;

  int setting_climbwait;

  int destroyed;

  int lapcount;

  int setting_airdownspeed;
  int setting_fpsincreasespeed;
  int setting_airinterval;
  int setting_airdecreasewait;
  int setting_defaultFPS;

  int setting_joyconfirm = 0;
  int setting_joycancel = 1;
  int setting_joyaxismax = 10000;
  int setting_joyenabled = 1;
  int setting_joysticknumber = 0;

  std::vector<SDL_Color> color_gradient;

  // a pointer to the engine's game settings
  Settings *settings = nullptr;

  Mix_Chunk *pop_sound = nullptr;
  Mix_Chunk *melt_sound = nullptr;
  Mix_Chunk *hard_sound = nullptr;
};
