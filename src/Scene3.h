//
//  Scene3.h
//  Particles3
//
//  Created by Saqoosha on 12/05/13.
//  Copyright (c) 2012 Saqoosha. All rights reserved.
//

#pragma once

#include "BaseScene.h"
#include "SPK.h"
#include "SPK_GL.h"


class Scene3 : public BaseScene {
 public:
  Scene3() {}
  
  void setup(int texture_id);
  void update();
  void draw();

 protected:
};
