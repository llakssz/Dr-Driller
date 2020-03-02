#pragma once

class Block {
 public:
  Block();
  ~Block();

  enum class blockState {
    BLOCKSTATE_NONE,
    BLOCKSTATE_FALLING,
    BLOCKSTATE_PREFALL,
    BLOCKSTATE_EXTINGUISHING,
    BLOCKSTATE_FALLFINISHED,
    BLOCKSTATE_PREEXTINGUISHING,
    BLOCKSTATE_EXTINGUISHED,
    BLOCKSTATE_FALLSTOP
  };

  //    enum class blockType {
  //      NO_BLOCK = 100, AIR_BLOCK = 20, HARD_BLOCK = 5, CLEAR_BLOCK = 4
  //    };
  //#define NO_BLOCK	100
  //#define AIR_BLOCK 20
  //#define HARD_BLOCK 5
  //#define CLEAR_BLOCK 4

  blockState state;
  //    blockType type;

  int type;
  int left;      // for falling(dy)
  int lefttime;  // for prefall wait time
  bool done;
  bool done_sub;

  bool unsetlock;

  int destroycount;  // for X block

  int shape;  // for soft blocks

  int extinguishingframe;

  bool player_dug;

  // holds random time (real game has 1-2 secs?) that crystal will exist before
  // self destruct
  int crystal_break_time;
  // which color the crystal should currently be showing
  // not used yet, we just pick a random texture at the moment...
  int crystal_frame;

  // has block been viewed (we can't rely on if in current view because
  // player can move up and down, but crystal should still destroy
  bool seen;
};
