//
//  Scene1.h
//  Particles3
//
//  Created by Saqoosha on 12/05/09.
//  Copyright (c) 2012 Saqoosha. All rights reserved.
//

#pragma once

#include "ofMain.h"
#include "BaseScene.h"
#include "DecayValue.h"

#include "SPK.h"
#include "SPK_GL.h"
#include "NoiseModifier.h"


class FountainGroup;

//--------------------------------------------------------------------------------
class Scene1 : public BaseScene {
 public:
  Scene1();
  
  void setup(int texture_id);
  void update();
  void draw();
  void gotMessage(ofxOscEventArgs &event);
  void textureChange(int texture_id) {
    renderer_->setTexture(texture_id);
  }
  void noteOn(int channel, int pitch, float velocity);
  void action(int kind, bool start);
  void randomizeCamera();
  
 protected:
  typedef vector<FountainGroup*> FountainGroupArray;
  FountainGroupArray groups_;

  int flow_control_channel_;
  float step_;
};
