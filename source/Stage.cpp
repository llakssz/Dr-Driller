#include <fstream>
#include <iostream>
#include <iterator>
#include <sstream>

#include "CInput.h"
#include "Extra.h"
#include "Stage.h"
#include "json/json.h"

#define NO_BLOCK 100
#define AIR_BLOCK 20
#define HARD_BLOCK 5
#define CLEAR_BLOCK 4
#define WHITE_BLOCK 10
#define CRYSTAL_BLOCK 13

#define PENALTY_FRAMES 60

#define PLAYER_NUM 256

//#define BLOCK_SIZE	48
//#define TILE_SIZE 48
#define TILE_W 48
#define TILE_H 40

// #define    CHARACTER_NUM 256
LTexture displaypanelTexture;
LTexture circleTexture;

LTexture playerIconTexture;
LTexture air_percent_backTexture;
LTexture air_spikes_Texture;
LTexture air_spikes_shadow_Texture;
LTexture meter_mark_Texture;

LTexture leftribbonTexture;
LTexture backTexture;
std::vector<LTexture> location_name(4);
std::vector<LTexture> location_graphic(4);

std::vector<LTexture> blockTextures(25);
std::vector<LTexture> crystalBlockTextures(9);

std::vector<LTexture> popstarTextures(5);
std::vector<LTexture> meltstarTextures(5);

std::vector<LTexture> scrollBgTextures(7);
LTexture bg_gradient;

int frames = 0;

Stage::Stage() {
  gameCam.x = 0;
  gameCam.y = 0;
  gameCam.w = 1280;
  gameCam.h = 720;
  // height / tileheight = num blocks shown on screen vertically

  Player temp;
  // set player 1 and player 2
  temp.num = 1;
  players_vec.push_back(temp);

  // puchi
  temp.num = 2;
  temp.candbljump = true;
  players_vec.push_back(temp);

  leadplayer = &players_vec[0];

  STAGE_WIDTH = 23;
  // STAGE_HEIGHT = 11;

  // initalize these variables when we create the stage
  // i saw 22 being accessed in an array
  // GAME_STAGE_WIDTH = 9;
  GAME_STAGE_WIDTH = STAGE_WIDTH;
  // i saw unsetprefalls can access array at 110, illegal...
  GAME_STAGE_HEIGHT = 100 + 10;

  STAGE_START_Y = 5;

  // Block gamestage[22][100 + 10];
  // gamestage2.resize(GAME_STAGE_WIDTH,
  // std::vector<Block>(GAME_STAGE_HEIGHT+1));
  gamestage = new Block *[GAME_STAGE_WIDTH + 1];
  for (int i = 0; i < GAME_STAGE_WIDTH + 1; i++) {
    gamestage[i] = new Block[GAME_STAGE_HEIGHT];  // build rows
  }

  PREFALLTIME = 72;

  scoreplus = 0;
  scorerest = 0;

  my_clear = 0;
  my_clearcount = 0;
  my_fps = 60;
  // my_time = 0;

  lap_showing = 0;

  setting_climbwait = 9;

  destroyed = 0;

  lapcount = 0;

  setting_airdownspeed = 10;
  setting_fpsincreasespeed = 2;
  setting_airinterval = 4;
  setting_airdecreasewait = 50;
  setting_defaultFPS = 60;

  // for background color
  // vector of many pastel colors, in a nice rainbow gradient
  int steps = 5000;
  float frequency = (2 * 3.142) / steps;
  color_gradient = getColorGradient(frequency, frequency, frequency, 0, 2, 4,
                                    230, 25, steps);
  pop_sound = Mix_LoadWAV("romfs:/audio/se/bank_se_00009.ogg");
  melt_sound = Mix_LoadWAV("romfs:/audio/se/bank_se_00005.ogg");
  hard_sound = Mix_LoadWAV("romfs:/audio/se/bank_se_00006.ogg");
}

Stage::~Stage() {
  for (int i = 0; i < GAME_STAGE_WIDTH + 1; i++) delete[] gamestage[i];
  delete[] gamestage;
}

void Stage::set_stage(int number, int percentage, int blockstyle) {
  // number is how many normal block colors there are
  // percentage controls how many hard blocks there are
  // blockstyle controls how many hard blocks are wrapped around airs

  // test rand
  srand(time(NULL));
  //

  int x, y /*,i*/;
  Block *p;
  int range;

  /*通常のブロックを敷き詰める*/
  // spread with normal blocks
  for (y = 0; y < GAME_STAGE_HEIGHT; ++y) {
    for (x = 0; x < GAME_STAGE_WIDTH; ++x) {
      p = &(gamestage[x][y]);

      // ideally, simply pick N (number) normal block types at random
      // so, 0 and 3, or 1 and 2, 2 and 0 etc.

      // random double between 0 and 1 - then * by number
      p->type = ((double)rand() / RAND_MAX) * number;
      if (p->type >= 4) p->type = 0;
      p->destroycount = 0;
      p->state = Block::blockState::BLOCKSTATE_NONE;
      p->left = 0;
      p->lefttime = 0;
      p->shape = 0;
    }
  }
  /*硬いブロックをばら撒く*/
  // scatter some hard blocks

  for (y = 10; y < GAME_STAGE_HEIGHT - 6; ++y) {
    if (((double)rand() / RAND_MAX) * 100 > percentage) continue;
    range = ((double)rand() / RAND_MAX) * 4;

    x = ((double)rand() / RAND_MAX) * GAME_STAGE_WIDTH;

    while (range > 0) {
      range--;
      if (x + range >= GAME_STAGE_WIDTH) continue;
      p = &(gamestage[x + range][y]);
      p->type = HARD_BLOCK;
      p->state = Block::blockState::BLOCKSTATE_NONE;
      p->left = 0;
      p->destroycount = 0;
      p->player_dug = false;
    }
  }

  // add crystal blocks
  for (y = 10; y < GAME_STAGE_HEIGHT - 6; ++y) {
    if (((double)rand() / RAND_MAX) * 100 > percentage) continue;
    range = ((double)rand() / RAND_MAX) * 4;

    x = ((double)rand() / RAND_MAX) * GAME_STAGE_WIDTH;

    while (range > 0) {
      range--;
      if (x + range >= GAME_STAGE_WIDTH) continue;
      p = &(gamestage[x + range][y]);
      p->type = CRYSTAL_BLOCK;
      p->state = Block::blockState::BLOCKSTATE_NONE;
      p->left = 0;
      p->destroycount = 0;
      p->player_dug = false;
      p->crystal_break_time = rand() % 200 + 100;  // 100 to 300
      p->crystal_frame = 0;
    }
  }

  // add white blocks
  for (y = 10; y < GAME_STAGE_HEIGHT - 6; ++y) {
    if (((double)rand() / RAND_MAX) * 100 > percentage) continue;
    range = ((double)rand() / RAND_MAX) * 2;

    x = ((double)rand() / RAND_MAX) * GAME_STAGE_WIDTH;

    while (range > 0) {
      range--;
      if (x + range >= GAME_STAGE_WIDTH) continue;
      p = &(gamestage[x + range][y]);
      p->type = WHITE_BLOCK;
      p->state = Block::blockState::BLOCKSTATE_NONE;
      p->left = 0;
      p->destroycount = 0;
      p->player_dug = false;
    }
  }

  /*airの設置及び周りに硬いブロックを配置*/
  // put air near hard blocks
  // fix my_depth!
  for (y = 11; y < GAME_STAGE_HEIGHT - 10;
       y += (setting_airinterval /*+ my_depth*/)) {
    x = ((double)rand() / RAND_MAX) * (GAME_STAGE_WIDTH - 2) + 1;
    p = &(gamestage[x][y]);
    p->type = AIR_BLOCK;
    p->state = Block::blockState::BLOCKSTATE_NONE;
    p->destroycount = 0;
    //添え字チェックはしなくても良いようにAirを配置
    // check to see if air too easy, and make harder?

    if (blockstyle == 4) gamestage[x - 1][y].type = HARD_BLOCK;
    if (blockstyle == 4) gamestage[x + 1][y].type = HARD_BLOCK;
    if (blockstyle == 4 || blockstyle == 1 || blockstyle == 3)
      gamestage[x][y - 1].type = HARD_BLOCK;
    if (blockstyle == 4 || blockstyle == 2 || blockstyle == 3)
      gamestage[x][y + 1].type = HARD_BLOCK;
  }

  /*クリアブロックを配置*/
  // do 100M clear blocks, at 100m-starty (mr driller does this)
  for (y = 100; y < 100 + STAGE_START_Y; ++y) {
    for (x = 0; x < GAME_STAGE_WIDTH; ++x) {
      p = &(gamestage[x][y]);
      p->type = CLEAR_BLOCK;
      p->state = Block::blockState::BLOCKSTATE_NONE;
      p->left = 0;
    }
  }
  /*スタート余白を設置*/
  // put blank space on the top, as large as stage_start_y is
  // this is just empty space above where player starts
  for (y = 0; y < STAGE_START_Y; ++y) {
    for (x = 0; x < GAME_STAGE_WIDTH; ++x) {
      p = &(gamestage[x][y]);
      p->type = NO_BLOCK;
      p->state = Block::blockState::BLOCKSTATE_NONE;
      p->left = 0;
    }
  }
}

void Stage::set_stage_startingpoint(void) {
  int i;
  int num_xy;
  Block *p;

  // const struct {
  //  int x;
  //  int y;
  //  int n;
  //}xy[] = {
  //  { 0,0,HARD_BLOCK },{ 1,0,HARD_BLOCK },{ 2,0,HARD_BLOCK },{ 3,0,HARD_BLOCK
  //  },{ 4,0,0 },{ 5,0,HARD_BLOCK },{ 6,0,HARD_BLOCK },{ 7,0,HARD_BLOCK },{
  //  8,0,HARD_BLOCK }, { 0,1,HARD_BLOCK },{ 1,1,HARD_BLOCK },{ 2,1,HARD_BLOCK
  //  },{ 3,1,HARD_BLOCK },{ 4,1,1 },{ 5,1,HARD_BLOCK },{ 6,1,HARD_BLOCK },{
  //  7,1,HARD_BLOCK },{ 8,1,HARD_BLOCK }, { 0,2,HARD_BLOCK },{ 1,2,HARD_BLOCK
  //  },{ 2,2,2 },{ 3,2,2 },{ 4,2,2 },{ 5,2,2 },{ 6,2,2 },{ 7,2,HARD_BLOCK },{
  //  8,2,HARD_BLOCK }, { 0,3,HARD_BLOCK },{ 1,3,HARD_BLOCK },{ 2,3,3 },{ 3,3,3
  //  },{ 4,3,3 },{ 5,3,3 },{ 6,3,3 },{ 7,3,HARD_BLOCK },{ 8,3,HARD_BLOCK }, {
  //  0,4,0 },{ 1,4,0 },{ 2,4,0 },{ 3,4,0 },{ 4,4,0 },{ 5,4,0 },{ 6,4,0 },{
  //  7,4,0 },{ 8,4,0 },
  //};
  // num_xy = sizeof(xy) / sizeof(xy[0]);

  struct xy {
    int x;
    int y;
    int n;
  };

  vector<xy> startsection;
  xy temp;
  // how long hard block line should be
  float halfwidth = (STAGE_WIDTH / 2);
  float interval = halfwidth / STAGE_START_Y;

  // make sure colors are alternating
  int lastcolor = -1;
  int currentcolor = -1;

  // 5 because that is the number of rows that the starting x blocks span
  for (int y = 0; y < STAGE_START_Y + 1; y++) {
    while (currentcolor == lastcolor) {
      // 4 is number of different color blocks
      currentcolor = rand() % 4;
    }

    for (int x = 0; x < STAGE_WIDTH; x++) {
      temp.x = x;
      temp.y = y;
      temp.n = -1;

      if (x < interval * (STAGE_START_Y - y)) {
        temp.n = HARD_BLOCK;
      }
      if (x > STAGE_WIDTH - interval * (STAGE_START_Y - y) - 1) {
        temp.n = HARD_BLOCK;
      }

      // if stage is even width, unset the top two middle blocks
      if (y == 0 && STAGE_WIDTH % 2 == 0) {
        if (x == (STAGE_WIDTH / 2) - 1) {
          temp.n = -1;
        }
        if (x == (STAGE_WIDTH / 2)) {
          temp.n = -1;
        }
      }

      if (temp.n == -1) {
        temp.n = currentcolor;
      }

      // set row under x blocks to same color as previous row
      // so when we drill previous row, all x blocks are free to break
      if (y == STAGE_START_Y) {
        temp.n = lastcolor;
      }

      startsection.push_back(temp);
    }
    lastcolor = currentcolor;
  }

  for (int i = 0; i < startsection.size(); i++) {
    p = &(gamestage[startsection[i].x][startsection[i].y + STAGE_START_Y]);
    p->type = startsection[i].n;

    // below not needed because it's in the blocks constructor
    // p->state = Block::blockState::BLOCKSTATE_NONE;
    // p->destroycount = 0;
  }

  // for (i = 0; i<num_xy; ++i) {
  //  p = &(gamestage[xy[i].x][xy[i].y + STAGE_START_Y]);
  //  p->type = xy[i].n;
  //  p->state = Block::blockState::BLOCKSTATE_NONE;
  //  p->destroycount = 0;
  //}
}

int Stage::BlitForBlock(int p, int num, int x, int y, SDL_Renderer *renderer,
                        int destroyed = 0) {
  static SDL_Rect official_rects[] = {
      {0, 0, TILE_W, TILE_H},
      {TILE_W, 0, TILE_W, TILE_H},
      {0, TILE_H, TILE_W, TILE_H},
      {TILE_W, TILE_H, TILE_W, TILE_H},

      {4 * TILE_W, 0, TILE_W, TILE_H},
      {5 * TILE_W, 0, TILE_W, TILE_H},
      {4 * TILE_W, TILE_H, TILE_W, TILE_H},
      {5 * TILE_W, TILE_H, TILE_W, TILE_H},

      {2 * TILE_W, 0, TILE_W, TILE_H},
      {3 * TILE_W, 0, TILE_W, TILE_H},
      {2 * TILE_W, TILE_H, TILE_W, TILE_H},
      {3 * TILE_W, TILE_H, TILE_W, TILE_H},

      {6 * TILE_W, 0, TILE_W, TILE_H},
      {7 * TILE_W, 0, TILE_W, TILE_H},
      {6 * TILE_W, TILE_H, TILE_W, TILE_H},
      {7 * TILE_W, TILE_H, TILE_W, TILE_H},
  };

  static SDL_Rect destroyedx_rects[] = {
      {3 * TILE_W, 0, TILE_W, TILE_H},
      {2 * TILE_W, 0, TILE_W, TILE_H},
      {TILE_W, 0, TILE_W, TILE_H},
      {0, 0, TILE_W, TILE_H},
  };

  // this is where we can offset the drawing of blocks
  // we can have a block start variable/stage x variable
  // and offset blocks a little right of the stripe banner
  // x += 12;

  if (p == CRYSTAL_BLOCK) {
    crystalBlockTextures[rand() % 9].render(x, y, renderer,
                                            &official_rects[num]);
  } else {
    blockTextures[p].render(x, y, renderer, &official_rects[num]);
    if (destroyed > 0) {
      blockTextures[6].render(x, y, renderer, &destroyedx_rects[destroyed - 1]);
    }
  }
  return 0;
}

// change to get actual block in here instead of just the block type
// crystal blocks stay/turn grey when about to break, until then they flash
// is grey one of the flashing colors? not sure
// also, in real game, crystals together flash the same color together...
int Stage::BlitForBlockpop(int p, int num, int x, int y, SDL_Renderer *renderer,
                           bool dug) {
  int popstar_size = 56;
  int meltstar_size = 64;

  static SDL_Rect popstar_rects[] = {
      {7 * popstar_size, 0, popstar_size, popstar_size},
      {6 * popstar_size, 0, popstar_size, popstar_size},
      {5 * popstar_size, 0, popstar_size, popstar_size},
      {4 * popstar_size, 0, popstar_size, popstar_size},
      {3 * popstar_size, 0, popstar_size, popstar_size},
      {2 * popstar_size, 0, popstar_size, popstar_size},
      {1 * popstar_size, 0, popstar_size, popstar_size},
      {0 * popstar_size, 0, popstar_size, popstar_size},
  };
  static SDL_Rect meltstar_rects[] = {
      {7 * meltstar_size, 0, meltstar_size, meltstar_size},
      {6 * meltstar_size, 0, meltstar_size, meltstar_size},
      {5 * meltstar_size, 0, meltstar_size, meltstar_size},
      {4 * meltstar_size, 0, meltstar_size, meltstar_size},
      {3 * meltstar_size, 0, meltstar_size, meltstar_size},
      {2 * meltstar_size, 0, meltstar_size, meltstar_size},
      {1 * meltstar_size, 0, meltstar_size, meltstar_size},
      {0 * meltstar_size, 0, meltstar_size, meltstar_size},
  };

  static SDL_Rect pop_rects[] = {
      {0 * TILE_W, 3 * TILE_H, TILE_W, TILE_H},
      {1 * TILE_W, 3 * TILE_H, TILE_W, TILE_H},
      {2 * TILE_W, 3 * TILE_H, TILE_W, TILE_H},
      {3 * TILE_W, 3 * TILE_H, TILE_W, TILE_H},
      {4 * TILE_W, 3 * TILE_H, TILE_W, TILE_H},
      {5 * TILE_W, 3 * TILE_H, TILE_W, TILE_H},
      {6 * TILE_W, 3 * TILE_H, TILE_W, TILE_H},
      {7 * TILE_W, 3 * TILE_H, TILE_W, TILE_H},
  };
  static SDL_Rect whiteblockpop_rects[] = {
      {0 * TILE_W, TILE_H, TILE_W, TILE_H},
      {1 * TILE_W, TILE_H, TILE_W, TILE_H},
      {2 * TILE_W, TILE_H, TILE_W, TILE_H},
      {3 * TILE_W, TILE_H, TILE_W, TILE_H},
      {4 * TILE_W, TILE_H, TILE_W, TILE_H},
      {5 * TILE_W, TILE_H, TILE_W, TILE_H},
      {6 * TILE_W, TILE_H, TILE_W, TILE_H},
      {7 * TILE_W, TILE_H, TILE_W, TILE_H},
  };

  static SDL_Rect melt_rects[] = {
      {0 * TILE_W, 2 * TILE_H, TILE_W, TILE_H},
      {1 * TILE_W, 2 * TILE_H, TILE_W, TILE_H},
      {2 * TILE_W, 2 * TILE_H, TILE_W, TILE_H},
      {3 * TILE_W, 2 * TILE_H, TILE_W, TILE_H},
      {4 * TILE_W, 2 * TILE_H, TILE_W, TILE_H},
      {5 * TILE_W, 2 * TILE_H, TILE_W, TILE_H},
      {6 * TILE_W, 2 * TILE_H, TILE_W, TILE_H},
      {7 * TILE_W, 2 * TILE_H, TILE_W, TILE_H},
  };

  if (dug) {
    if (p == WHITE_BLOCK) {
      blockTextures[p].render(x, y, renderer, &whiteblockpop_rects[num]);
    } else if (p == CRYSTAL_BLOCK) {
      crystalBlockTextures[rand() % 9].render(x, y, renderer, &pop_rects[num]);
    } else {
      blockTextures[p].render(x, y, renderer, &pop_rects[num]);
    }
    if (p < 4) {
      popstarTextures[p].render(x - (popstar_size - TILE_W) / 2,
                                y - (popstar_size - TILE_H) / 2, renderer,
                                &popstar_rects[num]);
    } else {
      popstarTextures[4].render(x - (popstar_size - TILE_W) / 2,
                                y - (popstar_size - TILE_H) / 2, renderer,
                                &popstar_rects[num]);
    }

  } else {
    if (p == CRYSTAL_BLOCK) {
      crystalBlockTextures[0].render(x, y, renderer, &melt_rects[num]);
    } else {
      blockTextures[p].render(x, y, renderer, &melt_rects[num]);
    }
    if (p < 4) {
      meltstarTextures[p].render(x - (meltstar_size - TILE_W) / 2,
                                 y - (meltstar_size - TILE_H) / 2, renderer,
                                 &meltstar_rects[num]);
    } else {
      meltstarTextures[4].render(x - (meltstar_size - TILE_W) / 2,
                                 y - (meltstar_size - TILE_H) / 2, renderer,
                                 &meltstar_rects[num]);
    }
  }

  return 0;
}

void Stage::set_shape(void) {
  int x, y;
  Block *p;

  for (y = 0; y < GAME_STAGE_HEIGHT; ++y) {
    for (x = 0; x < GAME_STAGE_WIDTH; ++x) {
      p = &(gamestage[x][y]);

      if (p->type == NO_BLOCK || p->type == AIR_BLOCK ||
          p->type == WHITE_BLOCK) {
        ++p;
        continue;
      }

      if (p->state == Block::blockState::BLOCKSTATE_FALLING ||
          p->state == Block::blockState::BLOCKSTATE_PREFALL ||
          p->state == Block::blockState::BLOCKSTATE_EXTINGUISHING) {
        ++p;
        continue;
      }
      p->shape = 0;

      if (x > 0 && gamestage[x - 1][y].type == p->type) /*p->shape+=4;*/ {
        ++p->shape;
        ++p->shape;
        ++p->shape;
        ++p->shape;
      }
      if (x < GAME_STAGE_WIDTH - 1 &&
          gamestage[x + 1][y].type == p->type) /*p->shape+=1;*/
        ++p->shape;
      if (y > 0 && gamestage[x][y - 1].type == p->type) /*p->shape+=2;*/ {
        ++p->shape;
        ++p->shape;
      }
      if (y < GAME_STAGE_HEIGHT - 1 &&
          gamestage[x][y + 1].type == p->type) /*p->shape+=8;*/ {
        ++p->shape;
        ++p->shape;
        ++p->shape;
        ++p->shape;
        ++p->shape;
        ++p->shape;
        ++p->shape;
        ++p->shape;
      }
    }
  }
}

void Stage::move(CInput input) {
  // erase_check_optimisation();
  // erase_check();

  for (Player &player : players_vec) {
    int mapx, mapy;
    int digx, digy;
    Block *p, *q;

    erase_check_optimisation();
    erase_check();

    if (player.dig_graphic) {
      player.dig_graphic++;
      if (player.dig_graphic >= 9) {
        player.dig_graphic = 0;
      }
    }

    if (player.climbing) {
      player.my_y += player.vy;
      if (player.my_y < 0) {
        player.my_y = 0;
      }
      if (player.my_y % TILE_H == 0) {
        // if a double jump, don't stop on the 1st block
        if (player.isdlbjumping) {
          player.isdlbjumping = false;
        } else {
          //			my_x+=vx*7;//for walkspeed=4
          //			my_x+=vx*5;//for walkspeed=6
          if (player.vx > 0)
            player.my_x += (7 * TILE_W) / 12;
          else
            player.my_x -= (7 * TILE_W) / 12;
          player.climbing = false;
          player.climbing_wait = false;
          player.repeat_x = 0;
        }
      } else
        player.vy = -player.WALKSPEED_Y;
      return;
    } else {
      player.vx = 0;
      player.vy = 0;
      player.dig = false;  // what's this line? if the player is not climbing,
                           // they are NOT digging??
      if (!player.dead && !my_clear && player.penaltyframe == 0) {
        // gameinput.CInputUpdate(0);
      } else {
        // gameinput.CInputUpdate(1);
      }
      if (player.num == 1 && !player.dead && !my_clear &&
          player.penaltyframe == 0) {
        if (input.button[CInput::game_buttons::UP]) {
          player.direction = DIR_UP;
        }
        if (input.button[CInput::game_buttons::DOWN]) {
          player.direction = DIR_DOWN;
        }
        if (input.button[CInput::game_buttons::RIGHT]) {
          player.vx = player.WALKSPEED;
          player.direction = DIR_RIGHT;
        }
        if (input.button[CInput::game_buttons::LEFT]) {
          player.vx = -player.WALKSPEED;
          player.direction = DIR_LEFT;
        }

        if (input.button[CInput::game_buttons::LEFT] ||
            input.button[CInput::game_buttons::RIGHT]) {
          player.walking = true;
        } else {
          player.walking = false;
        }

        if (input.button[CInput::game_buttons::CANCEL]) {
          leadplayer = &player;
          gameCam.Transition(leadplayer->my_y - (4 * TILE_H));
        }
      }

      if (player.num == 2 && !player.dead && !my_clear &&
          player.penaltyframe == 0) {
        if (input.button[CInput::game_buttons::P2_UP]) {
          player.direction = DIR_UP;
        }
        if (input.button[CInput::game_buttons::P2_DOWN]) {
          player.direction = DIR_DOWN;
        }
        if (input.button[CInput::game_buttons::P2_RIGHT]) {
          player.vx = player.WALKSPEED;
          player.direction = DIR_RIGHT;
        }
        if (input.button[CInput::game_buttons::P2_LEFT]) {
          player.vx = -player.WALKSPEED;
          player.direction = DIR_LEFT;
        }

        if (input.button[CInput::game_buttons::P2_LEFT] ||
            input.button[CInput::game_buttons::P2_RIGHT]) {
          player.walking = true;
        } else {
          player.walking = false;
        }

        if (input.button[CInput::game_buttons::P2_CANCEL]) {
          leadplayer = &player;
          gameCam.Transition(leadplayer->my_y - (4 * TILE_H));
        }
      }

      if (player.num == 1) {
        if ((input.button[CInput::game_buttons::OK]) &&
            (player.direction != DIR_NONE) && (player.my_y % TILE_H == 0)) {
          if (player.dig_repeat) {
            player.dig = false;
          } else {
            player.dig = true;
            player.dig_graphic = 1;
            player.dig_repeat = 1;
            player.dig_timer = 1;
          }
        } else {
          player.dig_repeat = 0;
        }
      }
      if (player.num == 2) {
        if ((input.button[CInput::game_buttons::P2_OK]) &&
            (player.direction != DIR_NONE) && (player.my_y % TILE_H == 0)) {
          if (player.dig_repeat) {
            player.dig = false;
          } else {
            player.dig = true;
            player.dig_graphic = 1;
            player.dig_repeat = 1;
            player.dig_timer = 1;
          }
        } else {
          player.dig_repeat = 0;
        }
      }
    }

    if (player.vx == 0) player.repeat_x = 0;

    if (player.penaltyframe > 0) {
      player.penaltyframe--;
    }

    mapx = player.my_x / TILE_W;
    mapy = player.my_y / TILE_H;

    // mapy would be negative at beginning, and used to access an array
    if (mapy < 0) {
      mapy = 0;
    }

    if (player.my_x % TILE_W >= TILE_W / 2) mapx++;
    if (player.my_y % TILE_H >= TILE_H / 2) mapy++;
    p = &gamestage[mapx][mapy];
    q = &gamestage[mapx][mapy + 1];

    if (player.my_y % TILE_H == 0) {
      if (q->type == NO_BLOCK || q->type == AIR_BLOCK ||
          q->state == Block::blockState::BLOCKSTATE_EXTINGUISHING) {
        player.vx = 0;
        player.vy = player.WALKSPEED_Y;
        player.dig = false;
        player.my_x = mapx * TILE_W;
        player.penaltyframe = 0;
        // player.dig_timer = 0; yes or no..?
      }
    } else {
      player.vx = 0;
      player.vy = player.WALKSPEED_Y;
      player.dig = false;
      player.penaltyframe = 0;
    }

    if (player.my_x % TILE_W >= TILE_W / 4 &&
        player.my_x % TILE_W < (TILE_W * 3) / 4) {
      if (player.direction == DIR_RIGHT || player.direction == DIR_LEFT) {
        // player.dig = false;
        // this to fix problem of not being able to drill on edges...
        // real game can
      }
    }

    if (player.vx != 0 && player.my_x % TILE_W == 0) {
      mapx = player.my_x / TILE_W;
      mapy = player.my_y / TILE_H;

      if (player.vx > 0) {
        mapx++;
      }
      if (player.vx < 0) {
        mapx--;
      }

      if (mapx < 0)
        player.vx = 0;
      else if (mapx >= STAGE_WIDTH)
        player.vx = 0;
      else if (gamestage[mapx][mapy].type == AIR_BLOCK) {
        ;
      } else if (gamestage[mapx][mapy].state ==
                     Block::blockState::BLOCKSTATE_EXTINGUISHING ||
                 gamestage[mapx][mapy].state ==
                     Block::blockState::BLOCKSTATE_EXTINGUISHED) {
        ;  // do nothing - fix maybe
      } else if (gamestage[mapx][mapy].type != NO_BLOCK) {
        // mapx is the block we are facing! (when pressed against)
        // player.my_x / TILE_W is where player is

        // if above player is empty and above where you are facing is empty/air
        if (gamestage[player.my_x / TILE_W][mapy - 1].type == NO_BLOCK &&
            gamestage[player.my_x / TILE_W][mapy - 1].state !=
                Block::blockState::BLOCKSTATE_EXTINGUISHING &&
            (gamestage[mapx][mapy - 1].type == NO_BLOCK ||
             gamestage[mapx][mapy - 1].type == AIR_BLOCK)) {
          player.repeat_x += player.vx;
          player.climbing_wait = true;
          if (player.repeat_x > player.WALKSPEED * setting_climbwait ||
              player.repeat_x < -player.WALKSPEED * setting_climbwait) {
            player.climbing = true;
            player.vy = -player.WALKSPEED_Y;
          } else
            player.vx = 0;
        }
        // or if can double jump and
        else if (player.candbljump) {
          // if above player is empty and 2 above player is empty, and 2 above
          // facing is empty
          if (gamestage[player.my_x / TILE_W][mapy - 1].type == NO_BLOCK &&
              gamestage[player.my_x / TILE_W][mapy - 1].state !=
                  Block::blockState::BLOCKSTATE_EXTINGUISHING &&
              gamestage[player.my_x / TILE_W][mapy - 2].type == NO_BLOCK &&
              gamestage[player.my_x / TILE_W][mapy - 2].state !=
                  Block::blockState::BLOCKSTATE_EXTINGUISHING &&
              (gamestage[mapx][mapy - 2].type == NO_BLOCK ||
               gamestage[mapx][mapy - 2].type == AIR_BLOCK)) {
            player.repeat_x += player.vx;
            player.climbing_wait = true;
            if (player.repeat_x > player.WALKSPEED * setting_climbwait ||
                player.repeat_x < -player.WALKSPEED * setting_climbwait) {
              player.isdlbjumping = true;
              player.climbing = true;
              player.vy = -player.WALKSPEED_Y;
            } else
              player.vx = 0;
          }
        } else {
          player.vx = 0;
          player.climbing_wait = false;
        }
      }
    }

    if (player.dig) {
      digx = player.my_x / TILE_W;
      if (player.my_x % TILE_W >= TILE_W / 2) digx++;
      digy = player.my_y / TILE_H;

      if (player.direction == DIR_DOWN) {
        digy++;
      } else if (player.direction == DIR_UP) {
        digy--;
      } else if (player.direction == DIR_RIGHT) {
        digx++;
      } else if (player.direction == DIR_LEFT) {
        digx--;
      }

      if (player.cantripledig) {
        int before_center_x = digx;
        int before_center_y = digy;
        int after_center_x = digx;
        int after_center_y = digy;

        if (player.direction == DIR_DOWN || player.direction == DIR_UP) {
          before_center_x--;
          after_center_x++;
        } else {
          before_center_y--;
          after_center_y++;
        }

        int digx_array[3] = {before_center_x, digx, after_center_x};
        int digy_array[3] = {before_center_y, digy, after_center_y};

        for (int i = 0; i < 3; i++) {
          if (digx_array[i] < 0 || digx_array[i] >= STAGE_WIDTH ||
              digy_array[i] < 0 || digy_array[i] >= GAME_STAGE_HEIGHT)
            ;
          else {
            if (gamestage[digx_array[i]][digy_array[i]].type != NO_BLOCK &&
                gamestage[digx_array[i]][digy_array[i]].state !=
                    Block::blockState::BLOCKSTATE_FALLING &&
                gamestage[digx_array[i]][digy_array[i]].state !=
                    Block::blockState::BLOCKSTATE_EXTINGUISHING &&
                gamestage[digx_array[i]][digy_array[i]].state !=
                    Block::blockState::BLOCKSTATE_EXTINGUISHED

            ) {
              if (gamestage[digx_array[i]][digy_array[i]].type == CLEAR_BLOCK) {
                stage_clear();
                //          PlaySample(sfx17);
              } else {
                self_destroy(digx_array[i], digy_array[i], player);
                destroyed = 1;
                player.repeat_x = 0;
              }
            }
          }
        }

      } else {
        // if normal dig
        destroyed = 0;
        if (digx < 0 || digx >= STAGE_WIDTH || digy < 0 ||
            digy >= GAME_STAGE_HEIGHT)
          ;
        else {
          if (gamestage[digx][digy].type != NO_BLOCK &&
              gamestage[digx][digy].state !=
                  Block::blockState::BLOCKSTATE_FALLING &&
              gamestage[digx][digy].state !=
                  Block::blockState::BLOCKSTATE_EXTINGUISHING &&
              gamestage[digx][digy].state !=
                  Block::blockState::BLOCKSTATE_EXTINGUISHED

          ) {
            if (gamestage[digx][digy].type == CLEAR_BLOCK) {
              stage_clear();
              //          PlaySample(sfx17);
            } else {
              self_destroy(digx, digy, player);
              destroyed = 1;
              player.repeat_x = 0;
            }
          }
        }
      }

      // close dig
    }

    if (player.my_y < 0) player.vy = 1;

    if (player.vx != 0)
      player.movingframe++;
    else
      player.movingframe = 0;
    if (!player.climbing) player.my_x += player.vx;
    if (player.vy > 0) {
      player.fallingframe++;
    } else {
      player.fallingframe = 0;
    }
    player.my_y += player.vy;

    if (player.my_x < 0) player.my_x = 0;
    if (player.my_y < 0) player.my_y = 0;
  }
  prefallcheck();
  blockprocess();
}

int Stage::erase_check_optimisation(void) {
  // erase_block(); + clear_blockflag_sub();
  Block *p;
  int x, y;
  int res = 0;
  for (y = 0; y < GAME_STAGE_HEIGHT; ++y)
    for (x = 0; x < GAME_STAGE_WIDTH; ++x) {
      p = &(gamestage[x][y]);
      // erase_block();
      if (p->state == Block::blockState::BLOCKSTATE_EXTINGUISHED) {
        res = 1;
        p->state = Block::blockState::BLOCKSTATE_NONE;
        p->type = NO_BLOCK;
      }
      // clear_blockflag_sub();
      p->done_sub = false;
      // JYCET ajout de clear_blockflag car il est appelé toujours avec ce void
      p->done = false;
      p->unsetlock = false;
    }
  return res;
}

void Stage::erase_check(void) {
  int x, y, answer, number;
  Block *p;

  // clear_blockflag_sub(); supprimé par erase_check_optimisation
  // clear_blockflag(); intégré dans clear_blockflag_sub

  for (y = GAME_STAGE_HEIGHT - 5; y >= 1; y--)
    for (x = 0; x < GAME_STAGE_WIDTH; x++) {
      p = &gamestage[x][y];
      if (p->done) continue;
      if (p->type == NO_BLOCK || p->type == AIR_BLOCK) continue;
      if (p->state ==
          Block::blockState::
              BLOCKSTATE_FALLING /*|| p->state == BLOCKSTATE_PREFALL*/)
        continue;

      answer = 0;
      number = 0;
      erase_check_recursive(x, y, p->type, &answer, &number);
      if (answer == 1 && number > 3) {
        seterase_recursive(x, y, p->type);
        // CWavsPlay(wavs,12);
        //        PlaySample(sfx13);
        Mix_PlayChannel(-1, pop_sound, 0);
      }
    }
}

void Stage::erase_check_recursive(int x, int y, int type, int *answer,
                                  int *number) {
  Block *p;

  p = &gamestage[x][y];
  if (p->done) return;
  if (p->type != type) return;
  p->done = true;
  if (p->state == Block::blockState::BLOCKSTATE_FALLING) return;
  (*number)++;
  //	if(p->state==BLOCKSTATE_EXTINGUISHED||p->state==BLOCKSTATE_EXTINGUISHING)(*answer)=1;
  if (p->just_landed) (*answer) = 1;
  p->just_landed = false;

  if (x > 0) erase_check_recursive(x - 1, y, type, answer, number);
  if (x < GAME_STAGE_WIDTH - 1)
    erase_check_recursive(x + 1, y, type, answer, number);
  if (y > 0) erase_check_recursive(x, y - 1, type, answer, number);
  if (y < GAME_STAGE_HEIGHT - 1)
    erase_check_recursive(x, y + 1, type, answer, number);
}

void Stage::seterase_recursive(int x, int y, int type) {
  Block *p, *q;

  p = &gamestage[x][y];
  q = &gamestage[x][y + 1];

  if (p->done_sub) return;
  if (p->type != type) return;
  p->done_sub = true;
  if (p->state == Block::blockState::BLOCKSTATE_FALLING) return;
  if (p->state == Block::blockState::BLOCKSTATE_EXTINGUISHING ||
      p->state == Block::blockState::BLOCKSTATE_EXTINGUISHED) {
    if (p->extinguishingframe < 8) return;
  }
  p->state = Block::blockState::BLOCKSTATE_EXTINGUISHING;
  p->extinguishingframe = 20;
  p->left = 0;
  p->lefttime = 0;
  p->player_dug = false;
  if (x > 0) seterase_recursive(x - 1, y, type);
  if (x < GAME_STAGE_WIDTH - 1) seterase_recursive(x + 1, y, type);
  if (y > 0) seterase_recursive(x, y - 1, type);
  if (y < GAME_STAGE_HEIGHT - 1) seterase_recursive(x, y + 1, type);
}

void Stage::prefallcheck(void) {
  int x, y, check, lefttime;
  Block *p, *q;

  clear_blockflag_sub();
  // clear_blockflag();

  for (y = GAME_STAGE_HEIGHT - 5; y >= 1; y--)
    for (x = 0; x < GAME_STAGE_WIDTH; x++) {
      p = &gamestage[x][y];
      if (p->type == NO_BLOCK) continue;
      if (p->state == Block::blockState::BLOCKSTATE_FALLING ||
          p->state == Block::blockState::BLOCKSTATE_EXTINGUISHING ||
          p->state == Block::blockState::BLOCKSTATE_EXTINGUISHED)
        continue;

      // if(p->state == BLOCKSTATE_FALLFINISHED)left=1;else left=PREFALLTIME;
      q = &gamestage[x][y + 1];
      if (p->unsetlock) continue;
      if (q->type == NO_BLOCK) {
        if (p->state == Block::blockState::BLOCKSTATE_NONE) {
          p->lefttime = 0;
          if (q->player_dug) p->lefttime = PREFALLTIME;
          p->state = Block::blockState::BLOCKSTATE_PREFALL;
        } else if (p->state == Block::blockState::BLOCKSTATE_FALLFINISHED) {
          p->lefttime = 0;
          p->state = Block::blockState::BLOCKSTATE_PREFALL;
        } else {
          p->state = Block::blockState::BLOCKSTATE_PREFALL;
        }
      } /*else if(q->type==p->type){

  p->state=q->state;
  p->state=q->state;


  }*/
      else if (q->state == Block::blockState::BLOCKSTATE_PREFALL) {
        // if(p->state==BLOCKSTATE_NONE){
        p->state = Block::blockState::BLOCKSTATE_PREFALL;
        p->lefttime = q->lefttime;
        //}
      } else if (q->state == Block::blockState::BLOCKSTATE_EXTINGUISHING ||
                 q->state == Block::blockState::BLOCKSTATE_FALLFINISHED ||
                 q->state == Block::blockState::BLOCKSTATE_EXTINGUISHED) {
        if (p->state != Block::blockState::BLOCKSTATE_FALLFINISHED) {
          p->lefttime = PREFALLTIME;
          p->state = Block::blockState::BLOCKSTATE_NONE;
          unsetprefall(x, y, p->type);

        } else {
          unsetprefallfinished(x, y, p->type);
          //				p->state=BLOCKSTATE_PREFALL;
        }
      } else {
        p->lefttime = PREFALLTIME;
        p->state = Block::blockState::BLOCKSTATE_NONE;
        unsetprefall(x, y, p->type);
      }
    }

  // clear_blockflag();
  clear_blockflag_sub();

  for (y = GAME_STAGE_HEIGHT - 5; y >= 1; y--)
    for (x = 0; x < GAME_STAGE_WIDTH; x++) {
      p = &gamestage[x][y];
      if (p->done) continue;
      if (p->type == NO_BLOCK) continue;
      if (p->state != Block::blockState::BLOCKSTATE_PREFALL) continue;
      lefttime = p->lefttime;
      check = 1;
      leftcheck(x, y, p->type, &lefttime, &check);
      setleft(x, y, p->type, lefttime);
    }
}

void Stage::leftcheck(int x, int y, int type, int *checkleft, int *check) {
  Block *p, *q;
  // int flag=0;
  int lefttime;
  p = &gamestage[x][y];
  q = &gamestage[x][y + 1];

  if (p->done) return;
  if (p->type != type) return;
  p->done = true;
  //	if(p->state!=BLOCKSTATE_PREFALL)return;
  if (p->state == Block::blockState::BLOCKSTATE_FALLING ||
      p->state == Block::blockState::BLOCKSTATE_EXTINGUISHING)
    return;

  if (p->lefttime > (*checkleft)) {
    *checkleft = p->lefttime;
    //		*check=0;
  }
  if (q->type != p->type) {
    if (q->type !=
        NO_BLOCK  //&&
                  //			q->state!=BLOCKSTATE_EXTINGUISHING
                  //&& 			q->state!=BLOCKSTATE_EXTINGUISHED
    ) {
      if (q->state == Block::blockState::BLOCKSTATE_PREFALL &&
          p->state == Block::blockState::BLOCKSTATE_PREFALL) {
        if (q->lefttime > (*checkleft)) *checkleft = q->lefttime;

      } else {
        /* if(q->state==BLOCKSTATE_NONE||q->state==BLOCKSTATE_FALLFINISHED
        ||q->state==BLOCKSTATE_FALLING){
        */
        lefttime = 0;
        if (q->player_dug) lefttime = PREFALLTIME;
        if (lefttime > (*checkleft)) *checkleft = lefttime;
        *check = 0;
      }
    }
  }

  if (p->type == AIR_BLOCK || p->type == WHITE_BLOCK)
    return;  // should white be here?

  if (x > 0) leftcheck(x - 1, y, type, checkleft, check);
  if (x < GAME_STAGE_WIDTH - 1) leftcheck(x + 1, y, type, checkleft, check);
  if (y > 0) leftcheck(x, y - 1, type, checkleft, check);
  if (y < GAME_STAGE_HEIGHT - 1) leftcheck(x, y + 1, type, checkleft, check);
}

void Stage::setleft(int x, int y, int type, int left) {
  Block *p, *q;

  p = &gamestage[x][y];
  q = &gamestage[x][y - 1];

  if (p->done_sub) return;
  if (p->type != type) return;
  p->done_sub = true;

  //	if(p->state==BLOCKSTATE_FALLING||p->state==BLOCKSTATE_EXTINGUISHING)return;
  if (p->state != Block::blockState::BLOCKSTATE_PREFALL) return;

  p->lefttime = left;
  /*	if(
  q->state!=BLOCKSTATE_EXTINGUISHING &&
  q->type!=NO_BLOCK &&
  q->state==BLOCKSTATE_PREFALL)q->lefttime=left;
  */
  if (p->type == AIR_BLOCK || p->type == WHITE_BLOCK)
    return;  // maybe not white here?

  if (x > 0) setleft(x - 1, y, type, left);
  if (x < GAME_STAGE_WIDTH - 1) setleft(x + 1, y, type, left);
  if (y > 0) setleft(x, y - 1, type, left);
  if (y < GAME_STAGE_HEIGHT - 1) setleft(x, y + 1, type, left);
}

// block processing
void Stage::blockprocess(void) {
  // this method uses a walkspeed for some reason...
  // try to get rid of it :)

  // THIS! my deconstructor was getting called when my copy of player died...
  /*Player player;
  player = players_vec[0];*/

  int x, y /*,check*/;
  // int number;
  Block *p, *q;
  clear_blockflag();
  for (y = GAME_STAGE_HEIGHT - 1; y >= 0; y--)
    for (x = 0; x < GAME_STAGE_WIDTH; x++) {
      p = &gamestage[x][y];

      if (p->type == NO_BLOCK) continue;

      if (p->type == CRYSTAL_BLOCK && p->seen) {
        p->crystal_break_time--;
      }
      if (p->type == CRYSTAL_BLOCK && p->crystal_break_time == 0) {
        destroy(x, y);
      }

      switch (p->state) {
        case Block::blockState::BLOCKSTATE_PREFALL: {
          p->lefttime--;
          if (p->lefttime <= 0) {
            p->lefttime = 0;
            q = &gamestage[x][y + 1];
            //					if(q->state==BLOCKSTATE_EXTINGUISHING)hanabiset(x,y+1,q->extinguishingframe);

            *q = *p;

            p->type = NO_BLOCK;
            p->state = Block::blockState::BLOCKSTATE_NONE;

            q->state = Block::blockState::BLOCKSTATE_FALLING;
            q->left = TILE_H - players_vec[0].WALKSPEED_Y;
          }
        } break;

        case Block::blockState::BLOCKSTATE_FALLING: {
          p->left -= players_vec[0].WALKSPEED_Y;
          if (p->left < players_vec[0].WALKSPEED_Y) {  // finshing.

            p->left = 0;
            p->lefttime = 1;
            p->state = Block::blockState::BLOCKSTATE_FALLFINISHED;
          }

        } break;

        case Block::blockState::BLOCKSTATE_EXTINGUISHING: {
          p->extinguishingframe--;
          if (p->extinguishingframe == 0)
            p->state = Block::blockState::BLOCKSTATE_EXTINGUISHED;

        } break;

        case Block::blockState::BLOCKSTATE_FALLFINISHED: {
        } break;
      }

      // if crystal block is in view
      int startblock = (gameCam.y / TILE_H);
      int endblock = 1 + ((gameCam.y + gameCam.h) / TILE_H);
      for (int y = startblock; y < endblock; y++) {
        // make sure we stay in block array
        if (y < 0) {
          continue;
        }
        if (y >= GAME_STAGE_HEIGHT) {
          continue;
        }

        for (int x = 0; x < STAGE_WIDTH; x++) {
          // int ypos = (y*TILE_H) - gameCam.y;
          p = &(gamestage[x][y]);
          if (p->type == CRYSTAL_BLOCK) {
            p->seen = true;  // not doing this perfectly?
          }
        }
      }
    }
}

// unset lock
void Stage::clear_blockflag(void) {
  int x, y;
  Block *p;
  for (y = 0; y < GAME_STAGE_HEIGHT; ++y)
    for (x = 0; x < GAME_STAGE_WIDTH; ++x) {
      p = &(gamestage[x][y]);
      p->done = false;
      p->unsetlock = false;
    }
}

void Stage::clear_blockflag_sub(void) {
  int x, y;
  Block *p;
  for (y = 0; y < GAME_STAGE_HEIGHT; ++y)
    for (x = 0; x < GAME_STAGE_WIDTH; ++x) {
      p = &(gamestage[x][y]);
      p->done_sub = false;
      // JYCET ajout de clear_blockflag car il est appelé toujours avec ce void
      p->done = false;
      p->unsetlock = false;
    }
}

void Stage::search(int x, int y, int type) {
  Block *p;

  p = &(gamestage[x][y]);
  if (p->done) return;
  if (p->type != type) return;
  if (p->state == Block::blockState::BLOCKSTATE_FALLING) return;
  p->done = true;
  p->state = Block::blockState::BLOCKSTATE_EXTINGUISHING;
  p->player_dug = true;
  //	p->extinguishingframe = 15;
  p->extinguishingframe = 31;
  if (x > 0) search(x - 1, y, type);
  if (y > 0) search(x, y - 1, type);
  if (x < GAME_STAGE_WIDTH - 1) search(x + 1, y, type);
  if (y < GAME_STAGE_HEIGHT - 1) search(x, y + 1, type);
}

void Stage::self_destroy(int x, int y, Player &player) {
  Block *p;
  clear_blockflag();
  p = &(gamestage[x][y]);
  if (p->type == HARD_BLOCK) {
    // CWavsPlay(wavs,13);
    //    PlaySample(sfx18);
    Mix_PlayChannel(-1, hard_sound, 0);

    p->destroycount++;
    if (p->destroycount >= 5) {
      p->state = Block::blockState::BLOCKSTATE_EXTINGUISHING;
      p->extinguishingframe = 31;
      player.my_air -= 20;
      player.airminus = 1;
      player.airminuscount = 0;
      p->player_dug = true;
      scoreplus += 1;
      scorerest += 1;
    }
    return;
  } else if (p->type == WHITE_BLOCK) {
    p->state = Block::blockState::BLOCKSTATE_EXTINGUISHING;
    p->extinguishingframe = 31;
    p->player_dug = true;
    scoreplus++;  // how many points?
    scorerest++;
    return;
  }
  destroy(x, y);
  scoreplus++;
  scorerest++;
}

void Stage::destroy(int x, int y) {
  Block *p;
  clear_blockflag();
  p = &(gamestage[x][y]);
  if (p->type == AIR_BLOCK || p->type == WHITE_BLOCK) return;
  if (p->state == Block::blockState::BLOCKSTATE_EXTINGUISHING) return;
  search(x, y, p->type);
  // CWavsPlay(wavs,12);
  //  PlaySample(sfx12);
  Mix_PlayChannel(-1, melt_sound, 0);
}

void Stage::clearaftercrushed(Player &player) {
  int mapx = player.my_x / TILE_W;
  int mapy = player.my_y / TILE_H;
  if (player.my_x % TILE_W >= TILE_W / 2) mapx++;
  if (player.my_y % TILE_H >= TILE_H / 2) mapy++;

  Block *p;

  while (mapy > -1) {
    p = &(gamestage[mapx][mapy]);
    if (p->type != NO_BLOCK) {
      p->state = Block::blockState::BLOCKSTATE_EXTINGUISHING;
      p->extinguishingframe = 31;
    }

    if (mapx != 0) {
      p = &(gamestage[mapx - 1][mapy]);
      if (p->type != NO_BLOCK) {
        p->state = Block::blockState::BLOCKSTATE_EXTINGUISHING;
        p->extinguishingframe = 31;
      }
    }
    if (mapx != STAGE_WIDTH - 1) {
      p = &(gamestage[mapx + 1][mapy]);
      if (p->type != NO_BLOCK) {
        p->state = Block::blockState::BLOCKSTATE_EXTINGUISHING;
        p->extinguishingframe = 31;
      }
    }

    mapy--;
  }
}

int Stage::erase_block(void) {
  Block *p;
  int x, y;
  int res = 0;
  for (y = 0; y < GAME_STAGE_HEIGHT; ++y)
    for (x = 0; x < GAME_STAGE_WIDTH; ++x) {
      p = &(gamestage[x][y]);
      if (p->state == Block::blockState::BLOCKSTATE_EXTINGUISHED) {
        res = 1;
        p->state = Block::blockState::BLOCKSTATE_NONE;
        p->type = NO_BLOCK;
      }
    }
  return res;
}

// check and return whether these blocks can fall
// checksheat is 1 when calling search
void Stage::search_number(int x, int y, int type, int *number) {
  Block *p;

  p = &gamestage[x][y];
  if (p->done) return;
  if (p->type != type) return;
  p->done = true;
  if (p->state == Block::blockState::BLOCKSTATE_FALLING) return;
  (*number)++;

  if (x > 0) search_number(x - 1, y, type, number);
  if (x < GAME_STAGE_WIDTH - 1) search_number(x + 1, y, type, number);
  if (y > 0) search_number(x, y - 1, type, number);
  if (y < GAME_STAGE_HEIGHT - 1) search_number(x, y + 1, type, number);
}

void Stage::search_fall(int x, int y, int type, int *checksheat,
                        int *checkleft) {
  Block *p, *q;
  // int flag=0;
  p = &gamestage[x][y];

  if (p->done) return;
  if (p->type != type) return;
  p->done = true;
  if (p->state == Block::blockState::BLOCKSTATE_FALLING) return;
  q = &gamestage[x][y + 1];
  if (q->type != NO_BLOCK && q->type != type) {
    if (q->state != Block::blockState::BLOCKSTATE_PREFALL &&
        q->state != Block::blockState::BLOCKSTATE_FALLING)
      *checksheat &= 0;
  } else {
    // really nothing here? - fix look
  }

  if (p->state == Block::blockState::BLOCKSTATE_NONE) p->left = PREFALLTIME;
  if (q->state == Block::blockState::BLOCKSTATE_PREFALL) p->left = q->left;
  if (p->left > *checkleft) *checkleft = p->left;

  if (x > 0) search_fall(x - 1, y, type, checksheat, checkleft);
  if (x < GAME_STAGE_WIDTH - 1)
    search_fall(x + 1, y, type, checksheat, checkleft);
  if (y > 0) search_fall(x, y - 1, type, checksheat, checkleft);
  if (y < GAME_STAGE_HEIGHT - 1)
    search_fall(x, y + 1, type, checksheat, checkleft);
}

void Stage::setprefall(int x, int y, int type, int left) {
  Block *p, *q;

  p = &gamestage[x][y];
  q = &gamestage[x][y + 1];

  if (p->done_sub) return;
  if (p->type != type) return;
  p->done_sub = true;

  if (p->state == Block::blockState::BLOCKSTATE_FALLING) return;

  /*
  p->state=BLOCKSTATE_FALLING;
  p->left=48;
  */

  p->lefttime = left;
  /*
  p->lefttime=PREFALLTIME;
  if(p->state==BLOCKSTATE_FALLFINISHED)p->lefttime=1;
  if(q->state==BLOCKSTATE_PREFALL) p->lefttime=q->lefttime;
  */
  p->state = Block::blockState::BLOCKSTATE_PREFALL;

  if (x > 0) setprefall(x - 1, y, type, left);
  if (x < GAME_STAGE_WIDTH - 1) setprefall(x + 1, y, type, left);
  if (y > 0) setprefall(x, y - 1, type, left);
  if (y < GAME_STAGE_HEIGHT - 1) setprefall(x, y + 1, type, left);
}

void Stage::unsetprefall(int x, int y, int type) {
  Block *p, *q;

  p = &gamestage[x][y];
  q = &gamestage[x][y + 1];

  if (p->done) return;
  if (p->type != type) return;
  p->done = true;
  if (p->state == Block::blockState::BLOCKSTATE_FALLING) return;
  if (p->state == Block::blockState::BLOCKSTATE_EXTINGUISHED ||
      p->state == Block::blockState::BLOCKSTATE_EXTINGUISHING)
    return;
  if (p->state == Block::blockState::BLOCKSTATE_FALLFINISHED)
    p->just_landed = true;
  p->state = Block::blockState::BLOCKSTATE_NONE;
  p->lefttime = PREFALLTIME;
  p->unsetlock = true;
  if (p->type == AIR_BLOCK || p->type == WHITE_BLOCK)
    return;  // this stops the air/whites sticking to eachother

  if (x > 0) unsetprefall(x - 1, y, type);
  if (x < GAME_STAGE_WIDTH - 1) unsetprefall(x + 1, y, type);
  if (y > 0) unsetprefall(x, y - 1, type);
  if (y < GAME_STAGE_HEIGHT - 1) unsetprefall(x, y + 1, type);
}

void Stage::unsetprefallfinished(int x, int y, int type) {
  Block *p, *q;

  p = &gamestage[x][y];
  q = &gamestage[x][y + 1];

  if (p->done) return;
  if (p->type != type) return;
  p->done = true;
  if (p->state == Block::blockState::BLOCKSTATE_FALLING) return;
  if (p->state == Block::blockState::BLOCKSTATE_EXTINGUISHED ||
      p->state == Block::blockState::BLOCKSTATE_EXTINGUISHING)
    return;
  p->state = Block::blockState::BLOCKSTATE_FALLFINISHED;
  p->lefttime = PREFALLTIME;
  p->unsetlock = true;
  if (p->type == AIR_BLOCK || p->type == WHITE_BLOCK) return;

  if (x > 0) unsetprefallfinished(x - 1, y, type);
  if (x < GAME_STAGE_WIDTH - 1) unsetprefallfinished(x + 1, y, type);
  if (y > 0) unsetprefallfinished(x, y - 1, type);
  if (y < GAME_STAGE_HEIGHT - 1) unsetprefallfinished(x, y + 1, type);
}

void Stage::stage_clear(void) {
  for (Player &player : players_vec) {
    int colors[] = {2, 3, 4, 4, 2, 4, 3, 3, 4, 4, 1};
    //	int colors[]={1,1,1,1,1,4,3,2,3,4,1};
    //	int percent[]={50,52,54,56,58,60,63,64,66,68,10};
    int percent[] = {50, 55, 60, 65, 30, 65, 70, 75, 80, 80, 10};
    int blockstyle[] = {1, 3, 3, 4, 1, 4, 4, 3, 4, 4, 1};

    int i, x, y;

    // for (i = 0, y = player.my_y / TILE_H - 4; i < 9; i++, y++)//should'-4' be
    // '-2'?
    for (i = 0, y = ((gameCam.h / TILE_H) - STAGE_START_Y - 1);
         i < (gameCam.h / TILE_H); i++, y++)  // should'-4' be '-2'?

      for (x = 0; x < GAME_STAGE_WIDTH; x++) {
        if (gamestage[x][y].type != NO_BLOCK) {
          //        hanabiset(x, i - 3, 15);
        }
      }

    player.my_depth++;
    if (player.my_depth >= 10) {
      my_clear = 1;
      my_clearcount = 0;
    }
    set_stage(colors[player.my_depth], percent[player.my_depth],
              blockstyle[player.my_depth]);
    player.climbing = false;
    player.climbing_wait = false;
    // not my_depth, the depth would be stored in the stage not the player,
    // maybe - fix this

    player.my_y = -2 * TILE_H;
    player.airdownspeed -= setting_airdownspeed;
    player.airdowncount = 0;
    scorerest += player.my_depth * 100;
    scoreplus += player.my_depth * 5;
    my_fps += setting_fpsincreasespeed;

    // my_time += gametime.clock / 10;

    player.fallingframe = 20;

    //  sprintf(lapstring, "%2d00m Passing %02d'%02d'%02d",
    //    my_depth,
    //    gametime.clock / 6000 / 10,
    //    (gametime.clock / 100 / 10) % 60,
    //    (gametime.clock / 10) % 100
    //  );
    lap_showing = 1;
    lapcount = 0;
    // CTimeReset(&gametime, setting_defaultFPS);
  }
}

void Stage::mainloop(CInput input, SDL_Renderer *renderer) {
  frames++;

  int y = 0;

  set_shape();

  /*for (Player& player : players_vec)
  {*/
  players_vec[0].airminuscount++;
  if (players_vec[0].airminuscount > 80) players_vec[0].airminus = 0;

  for (Player &player : players_vec) {
    if (player.dead) {
      player.deadcount++;
      if (player.deadcount > 250) {
        // gameover();
        // break;
      }
    }
  }

  // deplacement a
  move(input);
  // calcul air et score
  other_move();

  // test collision susumi
  for (Player &player : players_vec) {
    if (!player.dead && !my_clear) {
      atarihantei(player, renderer);
    }
  }

  // y = players_vec[0].my_y / TILE_H - STAGE_START_Y + 1;

  // always full air...
  players_vec[0].my_air = 100;
  players_vec[1].my_air = 100;

  // just uncomment below line to get a smooth lerp on camera
  // but, game feels much better without
  // gameCam.Transition(leadplayer->my_y - ((gameCam.h / TILE_H) - STAGE_START_Y
  // - 1)*TILE_H - TILE_H / 2);
  gameCam.Update(leadplayer->my_y -
                 ((gameCam.h / TILE_H) - STAGE_START_Y - 1) * TILE_H -
                 TILE_H / 2);
}

void Stage::draw(SDL_Renderer *renderer) {
  draw_back(renderer);
  draw_blocks(renderer);
  draw_me(renderer);
  draw_other();
  // drawhanabi();
  draw_screen(renderer);
  draw_air(renderer);
}

void Stage::draw_back(SDL_Renderer *renderer) {
  // go through the gradient as the player moves down
  // and loop back to start of gradient if needed
  int current_color = leadplayer->my_y % color_gradient.size();
  SDL_SetRenderDrawColor(renderer, color_gradient[current_color].r,
                         color_gradient[current_color].g,
                         color_gradient[current_color].b, 255);
  SDL_Rect temp_rect = {0, 0, 1280, 720};
  SDL_RenderFillRect(renderer, &temp_rect);

  // rename STAGE_WIDTH to NUM_BLOCKS_ACROSS
  // this is from left most block pixel to right most block pixel
  int main_stage_width = STAGE_WIDTH * TILE_W;
  int half_way = main_stage_width / 2;
  // we want to draw the background to the half way mark
  // by default, the texture is only 256 wide

  bg_gradient.render(0, 0, renderer, nullptr);

  int bg_x_pos = 200;
  int bg_width = scrollBgTextures[0].getWidth();

  int x1 = bg_x_pos;
  int x2 = bg_x_pos + bg_width - scrollBgTextures[1].getWidth();
  int x3 = bg_x_pos + bg_width - scrollBgTextures[2].getWidth();
  int x4 = bg_x_pos + bg_width - scrollBgTextures[3].getWidth();

  int start = gameCam.y - leadplayer->my_y;
  int end = gameCam.h + leadplayer->my_y;

  for (int y = start; y < end; y += 256) {
    // scrollBgTextures[0].render(x1, y - (1.0 * leadplayer->my_y), renderer,
    // nullptr); scrollBgTextures[1].render(x2, y - (0.75 * leadplayer->my_y),
    // renderer, nullptr); scrollBgTextures[2].render(x3, y - (0.50 *
    // leadplayer->my_y), renderer, nullptr); scrollBgTextures[3].render(x4, y -
    // (0.25 * leadplayer->my_y), renderer, nullptr);
    int y1 = std::floor((1.000 * leadplayer->my_y));
    int y2 = std::floor((0.875 * leadplayer->my_y));
    int y3 = std::floor((0.750 * leadplayer->my_y));
    int y4 = std::floor((0.625 * leadplayer->my_y));
    int y5 = std::floor((0.500 * leadplayer->my_y));
    int y6 = std::floor((0.375 * leadplayer->my_y));
    int y7 = std::floor((0.250 * leadplayer->my_y));

    scrollBgTextures[0].render(0, y - y1, renderer, nullptr);
    scrollBgTextures[1].render(0, y - y2, renderer, nullptr);
    scrollBgTextures[2].render(0, y - y3, renderer, nullptr);
    scrollBgTextures[3].render(0, y - y4, renderer, nullptr);
    scrollBgTextures[4].render(0, y - y5, renderer, nullptr);
    scrollBgTextures[5].render(0, y - y6, renderer, nullptr);
    scrollBgTextures[6].render(0, y - y7, renderer, nullptr);

    SDL_Rect temp_rect1 = {552, y - y1, scrollBgTextures[0].getWidth(),
                           scrollBgTextures[0].getHeight()};
    SDL_Rect temp_rect2 = {552, y - y2, scrollBgTextures[0].getWidth(),
                           scrollBgTextures[0].getHeight()};
    SDL_Rect temp_rect3 = {552, y - y3, scrollBgTextures[0].getWidth(),
                           scrollBgTextures[0].getHeight()};
    SDL_Rect temp_rect4 = {552, y - y4, scrollBgTextures[0].getWidth(),
                           scrollBgTextures[0].getHeight()};
    SDL_Rect temp_rect5 = {552, y - y5, scrollBgTextures[0].getWidth(),
                           scrollBgTextures[0].getHeight()};
    SDL_Rect temp_rect6 = {552, y - y6, scrollBgTextures[0].getWidth(),
                           scrollBgTextures[0].getHeight()};
    SDL_Rect temp_rect7 = {552, y - y7, scrollBgTextures[0].getWidth(),
                           scrollBgTextures[0].getHeight()};

    SDL_RenderCopyEx(renderer, scrollBgTextures[0].getTex(), nullptr,
                     &temp_rect1, 0, nullptr, SDL_FLIP_HORIZONTAL);
    SDL_RenderCopyEx(renderer, scrollBgTextures[1].getTex(), nullptr,
                     &temp_rect2, 0, nullptr, SDL_FLIP_HORIZONTAL);
    SDL_RenderCopyEx(renderer, scrollBgTextures[2].getTex(), nullptr,
                     &temp_rect3, 0, nullptr, SDL_FLIP_HORIZONTAL);
    SDL_RenderCopyEx(renderer, scrollBgTextures[3].getTex(), nullptr,
                     &temp_rect4, 0, nullptr, SDL_FLIP_HORIZONTAL);
    SDL_RenderCopyEx(renderer, scrollBgTextures[4].getTex(), nullptr,
                     &temp_rect5, 0, nullptr, SDL_FLIP_HORIZONTAL);
    SDL_RenderCopyEx(renderer, scrollBgTextures[5].getTex(), nullptr,
                     &temp_rect6, 0, nullptr, SDL_FLIP_HORIZONTAL);
    SDL_RenderCopyEx(renderer, scrollBgTextures[6].getTex(), nullptr,
                     &temp_rect7, 0, nullptr, SDL_FLIP_HORIZONTAL);
  }
}

void Stage::draw_blocks(SDL_Renderer *renderer) {
  static int vibration[] = {
      -1, -2, -3, -4, -3, -2, -1, 0, 1, 2, 3, 4, 3, 2, 1, 0,
      -1, -2, -3, -4, -3, -2, -1, 0, 1, 2, 3, 4, 3, 2, 1, 0,
      -1, -2, -3, -4, -3, -2, -1, 0, 1, 2, 3, 4, 3, 2, 1, 0,
      -1, -2, -3, -4, -3, -2, -1, 0, 1, 2, 3, 4, 3, 2, 1, 0,
      -1, -2, -3, -4, -3, -2, -1, 0, 1, 2, 3, 4, 3, 2, 1, 0,
  };

  Block *p;

  int startblock = (gameCam.y / TILE_H);
  int endblock = 1 + ((gameCam.y + gameCam.h) / TILE_H);
  for (int y = startblock; y < endblock; y++) {
    // make sure we stay in block array
    if (y < 0) {
      continue;
    }
    if (y >= GAME_STAGE_HEIGHT) {
      continue;
    }

    for (int x = 0; x < STAGE_WIDTH; x++) {
      int ypos = (y * TILE_H) - gameCam.y;

      p = &(gamestage[x][y]);

      if (p->type == NO_BLOCK) {
        // test
        // blockTextures[19].render(x*TILE_W, ypos, renderer);
        continue;
      }

      switch (p->state) {
        case Block::blockState::BLOCKSTATE_FALLING:
          BlitForBlock(p->type, p->shape, x * TILE_W, ypos - p->left, renderer,
                       p->destroycount);
          break;

        case Block::blockState::BLOCKSTATE_PREFALL:
          if (p->lefttime < 66) {
            BlitForBlock(p->type, p->shape, x * TILE_W + vibration[p->lefttime],
                         ypos, renderer, p->destroycount);
          } else {
            BlitForBlock(p->type, p->shape, x * TILE_W, ypos, renderer,
                         p->destroycount);
          }
          break;

        case Block::blockState::BLOCKSTATE_EXTINGUISHING:
          BlitForBlockpop(p->type, p->extinguishingframe / 4, x * TILE_W, ypos,
                          renderer, p->player_dug);
          break;

        case Block::blockState::BLOCKSTATE_EXTINGUISHED:
          // what would be drawn here? test sometime
          // CBmpsBlit(cbmps_character, screen_back, 109, pre_calcul_x[x], gy);
          break;

        default:
          BlitForBlock(p->type, p->shape, x * TILE_W, ypos, renderer,
                       p->destroycount);
          // test
          // testTextures[0].render(x*TILE_W, ypos, renderer);
          break;
      }
    }
  }
}

void Stage::draw_me(SDL_Renderer *renderer) {
  for (Player &player : players_vec) {
    //	CBmpsBlit(cbmps_character,screen_back,0,my_x*48,3*48);
    int i = 0;
    int j = 0;

    switch (player.direction) {
      case DIR_RIGHT:
        i = 30;
        break;
      case DIR_LEFT:
        i = 40;
        break;
      case DIR_DOWN:
        i = 10;
        break;
      case DIR_UP:
        i = 20;
        break;
    }

    if (player.dig_graphic == 0 && player.movingframe > 0) {
      j = player.movingframe / 2;
      j %= 4;
      i += j;
      i += 5;
    }

    i += player.dig_graphic / 3;  // frameeach
    if (player.penaltyframe > 0) {
      i = player.penaltybgnum;
    }

    if (player.fallingframe > 20) {
      i = 110 + (player.fallingframe % 12) / 6;
    }

    if (player.dead) {
      i = 100;
      if (player.deadcount > 30) i = 101;

      // clear blocks once
      if (player.deadcount == 100) {
        clearaftercrushed(player);
      }
      if (player.deadcount > 100) {
        // angelrise has  an internal timer of 200, maybe get rid of it?
        if (player.angelrise(player.my_y - gameCam.y, renderer)) {
          player.dead = false;
          player.my_air = 100;
        }
      }
    }

    // anim stuf

    // player.change_anim(player.animations[(int)Player::anim_actions::drillup]);

    // have this in the draw function?? maybe not :)
    if (player.dig_timer == 1) {
      if (player.anim_finished) {
        player.dig_timer = 0;
      }
    }

    // if not digging/after digging animation is done
    if (player.dig_timer == 0) {
      switch (player.direction) {
        case DIR_LEFT:
          if (player.climbing_wait && !player.climbing) {
            player.change_anim(
                player.animations[(int)Player::anim_actions::jumpleft]);
            player.reset_anim();
            player.pauseanim = true;
          } else if (player.climbing) {
            player.change_anim(
                player.animations[(int)Player::anim_actions::jumpleft]);
          } else if (player.walking) {
            player.change_anim(
                player.animations[(int)Player::anim_actions::walkleft], true);
          } else {
            player.change_anim(
                player.animations[(int)Player::anim_actions::walkleft]);
            player.reset_anim();
            player.pauseanim = true;
          }
          break;
        case DIR_RIGHT:
          if (player.climbing_wait && !player.climbing) {
            player.change_anim(
                player.animations[(int)Player::anim_actions::jumpright]);
            player.reset_anim();
            player.pauseanim = true;
          } else if (player.climbing) {
            player.change_anim(
                player.animations[(int)Player::anim_actions::jumpright]);
          } else if (player.walking) {
            player.change_anim(
                player.animations[(int)Player::anim_actions::walkright], true);
          } else {
            player.change_anim(
                player.animations[(int)Player::anim_actions::walkright]);
            player.reset_anim();
            player.pauseanim = true;
          }
          break;
        case DIR_UP:
          player.change_anim(
              player.animations[(int)Player::anim_actions::drillup]);
          player.reset_anim();  // not needed i think. but safe, remove later
          player.pauseanim = true;
          break;
        case DIR_DOWN:
          player.change_anim(
              player.animations[(int)Player::anim_actions::drilldown]);
          player.reset_anim();  // not needed i think. but safe, remove later
          player.pauseanim = true;
          break;
      }
    }

    // called once, when we press the dig button
    if (player.dig) {
      switch (player.direction) {
        case DIR_LEFT:
          if (player.walking) {
            player.change_anim(
                player.animations[(int)Player::anim_actions::drillwalkingleft]);
          } else {
            player.change_anim(
                player.animations[(int)Player::anim_actions::drillleft]);
          }
          break;
        case DIR_RIGHT:
          if (player.walking) {
            player.change_anim(
                player
                    .animations[(int)Player::anim_actions::drillwalkingright]);
          } else {
            player.change_anim(
                player.animations[(int)Player::anim_actions::drillright]);
          }
          break;
        case DIR_UP:
          player.change_anim(
              player.animations[(int)Player::anim_actions::drillup]);
          break;
        case DIR_DOWN:
          player.change_anim(
              player.animations[(int)Player::anim_actions::drilldown]);
          break;
      }
    }

    // if (!player.walking) {
    //	player.change_anim(player.animations[(int)Player::anim_actions::walkright],
    // true);
    //}

    // anim stuff done

    // SDL_Rect clip = player.currentAnimation->getFrame(susumuspriteloop);
    SDL_Rect clip = player.update_anim();

    // probably due to drawing in a wrong coordinate form? or make all sprites
    // 48px high? but, sprites where height was 48 were not aligned to blocks,
    // so:
    if (clip.h == 48) {
      player.draw(player.my_x, (player.my_y - gameCam.y) - 8, clip, renderer);
    } else {
      player.draw(player.my_x, player.my_y - gameCam.y, clip, renderer);
    }
  }
}

void Stage::draw_other(void) {
  for (Player &player : players_vec) {
    int gy;
    static int frame = 0;
    frame++;
    if (player.dead) {
      //    if (player.deadcount>20)	CBmpsBlit(cbmps_character, screen_back,
      //    70, 31, 70);//coords?
    }
    if (my_clear == 1) {
      //    if (my_clearcount>20)	CBmpsBlit(cbmps_character, screen_back,
      //    72, 31, 70);//coords?
    }
    if (lap_showing) {
      //    if (lapcount % 40<25)CffontBlitxy(font, lapstring, screen_back, 0,
      //    0);
    }
    if (player.my_y > 24 * 90) {
      // draw
      gy = 102 * 24 - player.my_y;
      //    CBmpsBlit(cbmps_character, screen_back, 120 + my_depth, 33, gy + 20
      //    + 24); //should '20' be '10'?
    }
    if (player.my_air < 25) {
      if (player.my_air == 20) {
        //      PlaySample(sfx15);
      }
      if ((frame % 12 / 6) == 0) {
        //      CBmpsBlit(cbmps_character, screen_back, 111, 33, 25); //changed
      }
    }
    if (player.airminus) {
      //    CBmpsBlit(cbmps_character, screen_back, 110, 58, 160); //changed
    }
  }
}

void Stage::draw_screen(SDL_Renderer *renderer) {
  location_graphic[0].render(1280 - location_graphic[0].getWidth(), 315,
                             renderer, nullptr);
  SDL_Rect renderQuad = {1280 - 216, 150, circleTexture.getWidth(),
                         circleTexture.getHeight()};
  SDL_Point center = {circleTexture.getWidth() / 2,
                      circleTexture.getHeight() / 2};
  SDL_RenderCopyEx(renderer, circleTexture.getTex(), nullptr, &renderQuad,
                   leadplayer->my_y / 4, &center, SDL_FLIP_NONE);

  // draw a rect with the same color as the location header name (in this case,
  // color is harcoded to Japan) because we want to hide empty space
  SDL_SetRenderDrawColor(renderer, 74, 82, 33, 255);
  SDL_Rect fill_up_space_rect = {1280 - 197, 0, 1280, 280};
  SDL_RenderFillRect(renderer, &fill_up_space_rect);

  displaypanelTexture.render(1280 - 224, 720 - 280, renderer, nullptr);
  playerIconTexture.render(1280 - 224 + 72, 720 - 280 + 225, renderer, nullptr);
  meter_mark_Texture.render(1280 - 224 + 142, 720 - 280 + 68, renderer,
                            nullptr);
  air_spikes_shadow_Texture.render(1280 - 224 + 69, 720 - 280 + 112, renderer,
                                   nullptr);
  air_spikes_Texture.render(1280 - 224 + 69, 720 - 280 + 112, renderer,
                            nullptr);
  air_percent_backTexture.render(1280 - 224 + 94, 720 - 280 + 132, renderer,
                                 nullptr);

  location_name[0].render(1280 - location_name[0].getWidth(), 274, renderer,
                          nullptr);

  // there must be a neater way to do this?
  int start = gameCam.y - leadplayer->my_y;
  int end = gameCam.h + leadplayer->my_y;
  for (int y = start; y < end; y += 22) {
    if (y - gameCam.y < -22) {
      continue;
    }
    if (y - gameCam.y > gameCam.h) {
      continue;
    }
    leftribbonTexture.render(0, y - gameCam.y, renderer, nullptr);
  }
}

void Stage::draw_air(SDL_Renderer *renderer) {
  // this is the air meter, not the air pick up item

  // SDL_Rect dest = { 243,160,50,16 };
  // SDL_Rect src = { 0,0,50,16 };

  // if (player.my_air <= 0)return;

  // src.w = player.my_air / 2;
  // dest.w = player.my_air / 2;

  //  SDL_BlitSurface(cbmps_character->bmp[103], &src, screen_back, &dest);
}

void Stage::other_move(void) {
  for (Player &player : players_vec) {
    if (player.my_y >= (STAGE_START_Y - 1) * TILE_H) player.airdowncount++;
    if (player.airdowncount > player.airdownspeed) {
      player.airdowncount = 0;
      //    player.my_air--; //removed for testing! fix this
    }
    if (player.airplus > 0) {
      player.airplus--;
      player.my_air++;
    }
    if (player.my_air <= 0) player.my_air = 0;
    if (player.my_air >= 100) player.my_air = 100;

    if (scoreplus > 0) {
      if (scorerest > scoreplus) {
        player.my_score += scoreplus;
        scorerest -= scoreplus;
      } else {
        player.my_score += scorerest;
        scorerest = 0;
        scoreplus = 0;
      }
    }
  }
}

int Stage::atarihantei(Player &player, SDL_Renderer *renderer) {
  int mapx, mapy;
  Block *p;
  int deltax, deltay;
  deltax = player.my_x % TILE_W;
  deltay = player.my_y % TILE_H;
  mapx = player.my_x / TILE_W;
  mapy = player.my_y / TILE_H;
  if (player.my_x % TILE_W >= TILE_W / 2) mapx++;
  if (player.my_y % TILE_H >= TILE_H / 2) mapy++;
  p = &gamestage[mapx][mapy];
  // i don't know if below air value should be W or H.... -- i think left is for
  // vertical fall, so I should use H here...
  if (p->type == AIR_BLOCK &&
      p->left < TILE_W / 2) {  // surely 12 because half of the block width?
    p->type = NO_BLOCK;
    p->state = Block::blockState::BLOCKSTATE_NONE;
    p->player_dug = true;
    player.airplus += 20;
    player.got_air++;
    scorerest += player.got_air * 10;
    scoreplus += player.got_air / 2 + 1;
    // CWavsPlay(wavs,11);
    //    PlaySample(sfx11);
  } else if (p->type != NO_BLOCK &&
             p->state != Block::blockState::BLOCKSTATE_EXTINGUISHING &&
             p->state != Block::blockState::BLOCKSTATE_EXTINGUISHED) {
    if (p->left < TILE_W / 2) {
      player.dead = true;
      player.deadcount = 0;
      player.vx = 0;
      //        PlaySample(sfx14);
      cout << "CRUSHED";
      // clearaftercrushed(player);
      // player.angelrise(renderer);

      return 1;
    }
  } else if (mapx > 0 && player.my_x % TILE_W == TILE_W / 2 &&
             gamestage[mapx - 1][mapy].type != NO_BLOCK &&
             gamestage[mapx - 1][mapy].type != AIR_BLOCK &&
             gamestage[mapx - 1][mapy].state !=
                 Block::blockState::BLOCKSTATE_EXTINGUISHING &&
             gamestage[mapx - 1][mapy].state !=
                 Block::blockState::BLOCKSTATE_EXTINGUISHED) {
    if (p->left < TILE_W / 2) {
      //          PlaySample(sfx14);
      player.dead = true;
      player.deadcount = 0;
      player.vx = 0;
      cout << "CRUSHED";
      // clearaftercrushed(player);
      // player.angelrise(renderer);

      return 1;
    }
  }
  if (player.my_air == 0) {
    //    PlaySample(sfx14);
    player.dead = true;
    player.deadcount = 0;
    player.vx = 0;
    return 2;
  }
  //補正
  if (deltay == 0 && deltax <= (5 * TILE_W) / 12 && mapx < STAGE_WIDTH &&
      gamestage[mapx + 1][mapy].type != NO_BLOCK &&
      gamestage[mapx + 1][mapy].type != AIR_BLOCK &&
      gamestage[mapx + 1][mapy].state !=
          Block::blockState::BLOCKSTATE_EXTINGUISHING &&
      gamestage[mapx + 1][mapy].state !=
          Block::blockState::BLOCKSTATE_EXTINGUISHED

  ) {
    player.my_x = mapx * TILE_W;
    if (deltax > TILE_W / 4) {
      //      PlaySample(sfx16);
      player.penaltyframe = PENALTY_FRAMES;
      if (player.direction == DIR_LEFT)
        player.penaltybgnum = 43;  // left escape
      else
        player.penaltybgnum = 34;
    }
  }
  if (deltay == 0 && deltax >= (7 * TILE_W) / 12 && mapx > 0 &&
      gamestage[mapx - 1][mapy].type != NO_BLOCK &&
      gamestage[mapx - 1][mapy].type != AIR_BLOCK &&
      gamestage[mapx - 1][mapy].state !=
          Block::blockState::BLOCKSTATE_EXTINGUISHING &&
      gamestage[mapx - 1][mapy].state !=
          Block::blockState::BLOCKSTATE_EXTINGUISHED

  ) {
    player.my_x = (mapx)*TILE_W;
    if (deltax < (TILE_W * 3) / 4) {
      //      PlaySample(sfx16);
      player.penaltyframe = PENALTY_FRAMES;
      if (player.direction == DIR_LEFT)
        player.penaltybgnum = 44;  // left escape
      else
        player.penaltybgnum = 33;
    }
  }

  return 0;
}

void Stage::game_ready(SDL_Renderer *renderer) {
  // gameinput.CInputInit(setting_joysticknumber, setting_joyenabled);
  // gameinput.CInputDefaultSetting();
  // gameinput.CInputHoldButtons();
  // gameinput.CInputUnholdArrows();
  // gameinput.CInputSetMinAxis(setting_joyaxismax);

  set_stage(4, 50, 1);

  players_vec[0].m_texture.loadPNGFromFile(
      "romfs:/chara/" + std::to_string(settings->chara_choice[0]) + ".png",
      renderer);
  players_vec[0].initanim(
      "romfs:/chara/" + std::to_string(settings->chara_choice[0]) + ".json",
      "");

  // p2 is always puchi for now...
  players_vec[1].m_texture.loadPNGFromFile("romfs:/chara/2.png", renderer);
  players_vec[1].initanim("romfs:/chara/2.json", "");

  // for puchi and usagi
  if ((settings->chara_choice[0] == 2) || (settings->chara_choice[0] == 6)) {
    players_vec[0].candbljump = true;
  }
  // for usagi
  if (settings->chara_choice[0] == 6) {
    players_vec[0].cantripledig = true;
  }

  for (Player &player : players_vec) {
    player.my_x = 4 * TILE_W;
    //	my_y=(STAGE_START_Y-1)*48;
    //	my_y=0;
    player.my_y = -2 * TILE_H;
    player.direction = DIR_DOWN;
    player.airdownspeed = setting_airdecreasewait;
    //	my_depth=9;
  }

  players_vec[0].my_x = ((STAGE_WIDTH / 2) - 1) * TILE_W;
  players_vec[1].my_x = ((STAGE_WIDTH / 2) + 1) * TILE_W;

  set_stage_startingpoint();

  //  clearhanabi();
  scoreplus = 0;
  scorerest = 0;
  my_clear = 0;
  my_clearcount = 0;
  my_fps = setting_defaultFPS;
  // my_time = 0;

  lap_showing = 0;

  // CTimeChangeFPS(&gametime,my_fps);
  // CTimeReset(&gametime, my_fps);

  //  LoadModule(mod_GAME);
  //  reset_frame_skip();

  players_vec[0].currentAnimation =
      &players_vec[0].animations[(int)Player::anim_actions::drillup];
  players_vec[1].currentAnimation =
      &players_vec[1].animations[(int)Player::anim_actions::drillup];

  int block_choice = rand() % 15;
  blockTextures[0].loadPNGFromFile(
      "romfs:/blocks/" + std::to_string(block_choice) + "/0.png", renderer);
  blockTextures[1].loadPNGFromFile(
      "romfs:/blocks/" + std::to_string(block_choice) + "/1.png", renderer);
  blockTextures[2].loadPNGFromFile(
      "romfs:/blocks/" + std::to_string(block_choice) + "/2.png", renderer);
  blockTextures[3].loadPNGFromFile(
      "romfs:/blocks/" + std::to_string(block_choice) + "/3.png", renderer);

  blockTextures[4].loadPNGFromFile("romfs:/blocks/clear.png", renderer);

  blockTextures[5].loadPNGFromFile("romfs:/blocks/batsu.png", renderer);
  blockTextures[6].loadPNGFromFile("romfs:/blocks/batsu_break.png", renderer);

  blockTextures[10].loadPNGFromFile("romfs:/blocks/white.png", renderer);

  for (int i = 0; i < crystalBlockTextures.size(); i++) {
    crystalBlockTextures[i].loadPNGFromFile(
        "romfs:/blocks/crystal/" + std::to_string(i) + ".png", renderer);
  }

  blockTextures[20].loadPNGFromFile("romfs:/air.png", renderer);

  popstarTextures[0].loadPNGFromFile("romfs:/blocks/pop_0.png", renderer);
  popstarTextures[1].loadPNGFromFile("romfs:/blocks/pop_1.png", renderer);
  popstarTextures[2].loadPNGFromFile("romfs:/blocks/pop_2.png", renderer);
  popstarTextures[3].loadPNGFromFile("romfs:/blocks/pop_3.png", renderer);
  popstarTextures[4].loadPNGFromFile("romfs:/blocks/pop_w.png", renderer);

  meltstarTextures[0].loadPNGFromFile("romfs:/blocks/melt_0.png", renderer);
  meltstarTextures[1].loadPNGFromFile("romfs:/blocks/melt_1.png", renderer);
  meltstarTextures[2].loadPNGFromFile("romfs:/blocks/melt_2.png", renderer);
  meltstarTextures[3].loadPNGFromFile("romfs:/blocks/melt_3.png", renderer);
  meltstarTextures[4].loadPNGFromFile("romfs:/blocks/melt_w.png", renderer);

  displaypanelTexture.loadPNGFromFile("romfs:/hud/displaypanel.png", renderer);
  leftribbonTexture.loadPNGFromFile("romfs:/hud/leftribbon.png", renderer);
  circleTexture.loadPNGFromFile("romfs:/hud/fullcircleribbon.png", renderer);
  location_name[0].loadPNGFromFile("romfs:/hud/name_japan.png", renderer);
  location_graphic[0].loadPNGFromFile("romfs:/hud/graphic_japan.png", renderer);

  playerIconTexture.loadPNGFromFile(
      "romfs:/hud/" + std::to_string(settings->chara_choice[0]) + ".png",
      renderer);

  air_percent_backTexture.loadPNGFromFile("romfs:/hud/percent.png", renderer);
  air_spikes_Texture.loadPNGFromFile("romfs:/hud/spikes.png", renderer);
  air_spikes_shadow_Texture.loadPNGFromFile("romfs:/hud/spikes_shadow.png",
                                            renderer);
  meter_mark_Texture.loadPNGFromFile("romfs:/hud/meter_inf.png", renderer);

  bg_gradient.loadPNGFromFile("romfs:/bg/gradient.png", renderer);
  scrollBgTextures[0].loadPNGFromFile("romfs:/bg/1.png", renderer);
  scrollBgTextures[1].loadPNGFromFile("romfs:/bg/2.png", renderer);
  scrollBgTextures[2].loadPNGFromFile("romfs:/bg/3.png", renderer);
  scrollBgTextures[3].loadPNGFromFile("romfs:/bg/4.png", renderer);
  scrollBgTextures[4].loadPNGFromFile("romfs:/bg/5.png", renderer);
  scrollBgTextures[5].loadPNGFromFile("romfs:/bg/6.png", renderer);
  scrollBgTextures[6].loadPNGFromFile("romfs:/bg/7.png", renderer);
}