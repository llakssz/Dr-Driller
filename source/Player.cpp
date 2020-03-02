#include <fstream>
#include <iostream>

#include "json/json.h"

#include "Player.h"

Player::Player() {
  Animation* currentAnimation = nullptr;

  dead = false;
  deadcount = 0;
  angelcount = 0;
  angely = 0;

  dig_graphic = 0;

  penaltybgnum = 0;
  penaltyframe = 0;
  fallingframe = 60;
  climbing = false;
  airdowncount = 0;
  got_air = 0;
  my_score = 0;

  my_air = 100;
  my_depth = 0;
  airplus = 0;

  dig = false;

  candbljump = false;
  isdlbjumping = false;

  cantripledig = false;
  climbing_wait = false;

  previous_direction = 0;

  framenumber = 0;
  loopanim = false;
  pauseanim = false;
  walking = false;
  anim_finished = false;

  dig_timer = 0;
}

Player::~Player() {}

void Player::draw(int x, int y, SDL_Rect clip, SDL_Renderer* renderer) {
  m_texture.render(x, y, renderer, &clip);
}

bool Player::initanim(std::string json, std::string png) {
  std::string segment;
  std::vector<std::string> seglist;

  std::ifstream ifs(json);
  Json::Reader reader;
  Json::Value obj;
  reader.parse(ifs, obj);
  const Json::Value& characters = obj["frames"];  // array of characters
  for (int i = 0; i < characters.size(); i++) {
    string mystring = characters[i]["filename"].asString();
    // remove .png from end of string
    // mystring.erase(mystring.size() - 4);
    stringstream ss;
    ss << mystring;

    int piece = 0;
    std::string animname;
    while (std::getline(ss, segment, '_')) {
      // seglist.push_back(segment);
      piece++;
      // 1st piece is character name
      if (piece == 2) {
        // animation name
        animname = segment;
      } else if (piece == 3) {
        // frame number
      }
    }

    SDL_Rect cliprect{characters[i]["frame"]["x"].asInt(),
                      characters[i]["frame"]["y"].asInt(),
                      characters[i]["frame"]["w"].asInt(),
                      characters[i]["frame"]["h"].asInt()};

    animations.resize(24);

    if (animname == "angel") {
      animations[(int)Player::anim_actions::angel].addFrame(cliprect);
    } else if (animname == "crushleft") {
      animations[(int)Player::anim_actions::crushleft].addFrame(cliprect);
    } else if (animname == "crushright") {
      animations[(int)Player::anim_actions::crushright].addFrame(cliprect);
    } else if (animname == "dodgebackleft") {
      animations[(int)Player::anim_actions::dodgebackleft].addFrame(cliprect);
    } else if (animname == "dodgebackright") {
      animations[(int)Player::anim_actions::dodgebackright].addFrame(cliprect);
    } else if (animname == "dodgefrontleft") {
      animations[(int)Player::anim_actions::dodgefrontleft].addFrame(cliprect);
    } else if (animname == "dodgefrontright") {
      animations[(int)Player::anim_actions::dodgefrontright].addFrame(cliprect);
    } else if (animname == "drilldown") {
      animations[(int)Player::anim_actions::drilldown].addFrame(cliprect);
    } else if (animname == "drillleft") {
      animations[(int)Player::anim_actions::drillleft].addFrame(cliprect);
    } else if (animname == "drillright") {
      animations[(int)Player::anim_actions::drillright].addFrame(cliprect);
    } else if (animname == "drillup") {
      animations[(int)Player::anim_actions::drillup].addFrame(cliprect);
    } else if (animname == "drillwalkingleft") {
      animations[(int)Player::anim_actions::drillwalkingleft].addFrame(
          cliprect);
    } else if (animname == "drillwalkingright") {
      animations[(int)Player::anim_actions::drillwalkingright].addFrame(
          cliprect);
    } else if (animname == "fall") {
      animations[(int)Player::anim_actions::fall].addFrame(cliprect);
    } else if (animname == "goal") {
      animations[(int)Player::anim_actions::goal].addFrame(cliprect);
    } else if (animname == "jumpleft") {
      animations[(int)Player::anim_actions::jumpleft].addFrame(cliprect);
    } else if (animname == "jumpright") {
      animations[(int)Player::anim_actions::jumpright].addFrame(cliprect);
    } else if (animname == "noairleft") {
      animations[(int)Player::anim_actions::noairleft].addFrame(cliprect);
    } else if (animname == "noairright") {
      animations[(int)Player::anim_actions::noairright].addFrame(cliprect);
    } else if (animname == "revive") {
      animations[(int)Player::anim_actions::revive].addFrame(cliprect);
    } else if (animname == "walkleft") {
      animations[(int)Player::anim_actions::walkleft].addFrame(cliprect);
    } else if (animname == "walkright") {
      animations[(int)Player::anim_actions::walkright].addFrame(cliprect);
    } else if (animname == "xyz") {
      animations[(int)Player::anim_actions::xyz].addFrame(cliprect);
    }
  }

  return true;
}

bool Player::angelrise(int y, SDL_Renderer* renderer) {
  // return when angel is done, and free to revive?
  SDL_Rect angel = animations[(int)Player::anim_actions::angel].getFrame(0);
  m_texture.setAlpha(100);
  draw(my_x, y - angelcount, angel, renderer);
  m_texture.setAlpha(255);
  angelcount++;
  if (angelcount == 200) {
    angelcount = 0;
    return true;
  }
  return false;
}

SDL_Rect Player::update_anim() {
  if (framenumber == 3 * currentAnimation->getSize()) {
    if (loopanim) {
      framenumber = 0;
    } else {
      pauseanim = true;
      framenumber = 0;
      anim_finished = true;
    }
  }

  SDL_Rect clip = currentAnimation->getFrame(framenumber / 3);

  if (!pauseanim) {
    framenumber++;
  }

  return clip;
}

void Player::change_anim(Animation& anim, bool loop) {
  prevAnimation = currentAnimation;  // ever needed?
  currentAnimation = &anim;

  if (prevAnimation == currentAnimation) {
    // return;
  }

  // how to handle a button being held (left0 for walk left etc)
  // and the animation not keep starting from the beginning
  // we can't just rely on the previous animation because if we let go
  // framenumber = 0;

  if (!loop) {
    framenumber = 0;
  }

  loopanim = loop;
  pauseanim = false;
  anim_finished = false;
}

void Player::reset_anim() { framenumber = 0; }
