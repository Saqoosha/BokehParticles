//
//  BaseScene.h
//  Particles4
//
//  Created by Saqoosha on 12/05/18.
//  Copyright (c) 2012 Saqoosha. All rights reserved.
//

#pragma once

#include "ofMain.h"
#include "ofxFPSCam.h"
#include "ofxOscEventArgs.h"
#include "ofxSRGBTexture.h"
#include "BokehParticleShader.h"
#include "SPK.h"
#include "SPK_GL.h"


class BaseScene {
 public:
  BaseScene() : system_(NULL), renderer_(NULL) {}
  virtual void setup(int texture_id) {}
  virtual void update() {}
  virtual void draw() {}
  virtual void gotMessage(ofxOscEventArgs &event) {
    if (event.address(0) == "camera") {
      if (event.address(1) == "fov") {
        camera_.setFov(event.value());
      }
      bokeh_particle_shader_.setParameter(event.address(1), event.value());
    }
  }
  virtual void textureChange(int texture_id) {
    if (renderer_ != NULL) {
      renderer_->setTexture(texture_id);
    }
  }
  virtual void audioIn(int channel, float volume) {}
  virtual void noteOn(int channel, int pitch, float velocity) {}
  virtual void action(int kind, bool start) {}
  
  BokehParticleShader &bokeh_particle_shader() { return bokeh_particle_shader_; }

 protected:
  ofxFPSCam camera_;
  ofVec3f camera_position_;
  ofVec3f next_camera_position_;
  ofVec3f camera_target_;
  ofVec3f next_camera_target_;
  
  BokehParticleShader bokeh_particle_shader_;
  
  SPK::System *system_;
  SPK::GL::GLPointRenderer *renderer_;
};
