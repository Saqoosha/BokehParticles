//
//  Scene4.cpp
//  Particles3
//
//  Created by Saqoosha on 12/05/13.
//  Copyright (c) 2012 Saqoosha. All rights reserved.
//

#include "Scene4.h"

using namespace SPK;
using namespace SPK::GL;

#define DEFAULT_FOVY 30.f


//--------------------------------------------------------------------------------
#pragma mark -
class CircleEmitter : public StaticEmitter {
  SPK_IMPLEMENT_REGISTERABLE(CircleEmitter)
  
public:
  CircleEmitter() : StaticEmitter(), step_(0.f), radius_(100.f) {
    point_ = SPK::Point::create();
    setZone(point_);
    update();
  }
  
  static CircleEmitter *create() {
    CircleEmitter *obj = new CircleEmitter();
    registerObject(obj);
    return obj;
  }
  
  void update() {
    step_ += .05f;
    float x = cos(step_) * radius_;
    float y = sin(step_) * radius_;
    float z = 0;
    point_->setPosition(Vector3D(x, y, z));
  }
  
  void set_radius(float radius) {
    radius_ = radius;
  }
  
private:
  SPK::Point *point_;
  float radius_;
  float step_;
};


//--------------------------------------------------------------------------------
void Scene4::setup(int texture_id) {
  // Camera
  camera_.setFov(DEFAULT_FOVY);
  camera_.setPosition(0, 50, 500);
  camera_.lookAt(ofVec3f(0, 0, 0));
  
  // Renderers
  renderer_ = GLPointRenderer::create();
  renderer_->setType(POINT_SPRITE);
  renderer_->setTexture(texture_id);
  renderer_->setBlending(BLENDING_ADD);
  renderer_->enableRenderingHint(DEPTH_TEST, false);
  
  // Model
  model_ = Model::create(FLAG_ALPHA | FLAG_CUSTOM_0, FLAG_ALPHA | FLAG_CUSTOM_0, FLAG_CUSTOM_0);
  model_->setParam(PARAM_ALPHA, 5.f, 0.f);
  model_->setParam(PARAM_CUSTOM_0, -10.f, 0.f, 10.f, 30.f);
  model_->setLifeTime(8.f, 10.f);
  
  // Emitter
  emitter_ = CircleEmitter::create();
  emitter_->setFlow(200);
  
  noise_modifier_ = NoiseModifier::create();
  noise_modifier_->set_power(.5f);
  noise_modifier_->set_scroll_speed(0.1f);
  
  // Group
  group_ = Group::create(model_, 3000);
  group_->addEmitter(emitter_);
  group_->addModifier(noise_modifier_);
  group_->setRenderer(renderer_);
  group_->setGravity(Vector3D(0.0f, -20.f, 0.0f));
  group_->setFriction(0.2);
  
  // System
  system_ = System::create();
  system_->addGroup(group_);
  
  // Shader
  bokeh_particle_shader_.load();
  bokeh_particle_shader_.set_fov_y(DEFAULT_FOVY);
}


//--------------------------------------------------------------------------------
void Scene4::update() {
  static float offset = 0.f;
  camera_.orbit(ofGetElapsedTimef() * -8.f, 0, ofNoise(offset) * 400.f + 300.f);
  offset += .002f;

  ofFloatColor color;
  color.setHsb(fmod((ofSignedNoise(offset * 2.f) * 200.f + 100.f + 256.f), 255.f), 128.f, 255.f);
  model_->setParam(PARAM_RED, color.r / 255.f);
  model_->setParam(PARAM_GREEN, color.g / 255.f);
  model_->setParam(PARAM_BLUE, color.b / 255.f);

  emitter_->update();
  
  system_->update(ofGetLastFrameTime());
}


//--------------------------------------------------------------------------------
void Scene4::draw() {
  camera_.begin(ofRectangle(0, 0, 1024, 768));
    bokeh_particle_shader_.begin();
      bokeh_particle_shader_.setAttributePointer("rotation", 1, GL_FLOAT, GL_FALSE, group_->getParamStride(), group_->getParamAddress(PARAM_CUSTOM_0));
      system_->render();
    bokeh_particle_shader_.end();
  camera_.end();
}

