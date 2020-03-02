#include "Block.h"

Block::Block() {
  state = Block::blockState::BLOCKSTATE_NONE;
  type = 100;
  left = 0;
  lefttime = 0;
  done = false;
  done_sub = false;
  unsetlock = false;

  destroycount = 0;

  shape = 0;

  extinguishingframe = 0;

  player_dug = false;

  crystal_break_time = 0;
  seen = false;
}

Block::~Block() {}
