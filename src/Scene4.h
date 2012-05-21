//
//  Scene4.h
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

class CircleEmitter;

class Scene4 : public BaseScene {
 public:
  Scene4() {};
  
  void setup(int texture_id);
  void update();
  void draw();

 protected:
  SPK::Model *model_;
  CircleEmitter *emitter_;
  SPK::NoiseModifier *noise_modifier_;
  SPK::Group *group_;
};
