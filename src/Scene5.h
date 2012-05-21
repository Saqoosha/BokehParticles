//
//  Scene5.h
//  Particles3
//
//  Created by Saqoosha on 12/05/13.
//  Copyright (c) 2012 Saqoosha. All rights reserved.
//

#pragma once

#include "ofxAssimpModelLoader.h"
#include "BaseScene.h"
#include "NoiseModifier.h"

class ModelVertexGroup;

class Scene5 : public BaseScene {
 public:
  Scene5() {};
  
  void setup(int texture_id);
  void update();
  void draw();
  void action(int kind, bool start);
  
  void newModel(int index);

 protected:
  vector<ofxAssimpModelLoader*> models_;
  SPK::NoiseModifier *noise_modifier_;
  typedef vector<ModelVertexGroup*> GroupArray;
  GroupArray groups_;
};
