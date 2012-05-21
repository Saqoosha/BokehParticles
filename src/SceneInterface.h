//
//  SceneInterface.h
//  Particles3
//
//  Created by Saqoosha on 12/05/13.
//  Copyright (c) 2012 Saqoosha. All rights reserved.
//

#pragma once

#include "ofxOscEventArgs.h"


class SceneInterface {
 public:
  virtual void setup() = 0;
  virtual void update() = 0;
  virtual void draw() = 0;
  virtual void gotMessage(ofxOscEventArgs &event) = 0;
  virtual void noteOn(int channel, int pitch, float velocity) = 0;
};
