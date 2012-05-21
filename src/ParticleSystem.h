//
//  ParticleSystem.h
//  Particles3
//
//  Created by Saqoosha on 12/05/06.
//  Copyright (c) 2012 Saqoosha. All rights reserved.
//

#pragma once

#include "ofMain.h"
#include "SPK.h"
#include "SPK_GL.h"
#include "NoiseModifier.h"


class ParticleSystem {
 public:
  ParticleSystem();
  
  void setup(GLuint texture_id);
  void update();
  void draw();
  
  void set_flow(float flow) {
    emitter_->setFlow(flow);
  };
  
  void enableAttribute(GLint location, SPK::ModelParam param);
  
 private:
  SPK::Model *model_;
  SPK::Emitter *emitter_;
  NoiseModifier *noise_modifier_;
  SPK::Group *group_;
  SPK::System *system_;
  
  float step_;
};

