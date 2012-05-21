//
//  Scene2.h
//  Particles3
//
//  Created by Saqoosha on 12/05/13.
//  Copyright (c) 2012 Saqoosha. All rights reserved.
//

#pragma once

#include "ofMain.h"
#include "BaseScene.h"
#include "NoiseModifier.h"

#include "SPK.h"
#include "SPK_GL.h"


class StaticGroup;

class Scene2 : public BaseScene {
 public:
  Scene2() {};
  
  void setup(int texture_id);
  void update();
  void draw();
  void textureChange(int texture_id) {
    renderer_->setTexture(texture_id);
  }
  void audioIn(int channel, float volume);
  void action(int kind, bool start);

 protected:
  SPK::NoiseModifier *noise_modifier_;
  vector<StaticGroup*> groups_;
};
