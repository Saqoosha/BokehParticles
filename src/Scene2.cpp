//
//  Scene2.cpp
//  Particles3
//
//  Created by Saqoosha on 12/05/13.
//  Copyright (c) 2012 Saqoosha. All rights reserved.
//

#include "Scene2.h"

using namespace SPK;
using namespace SPK::GL;

#define DEFAULT_FOVY 30.f


//--------------------------------------------------------------------------------
class StaticGroup : public Group {
  SPK_IMPLEMENT_REGISTERABLE(StaticGroup)
 public:
  StaticGroup() : Group(NULL, 1000) {
    model_ = Model::create(FLAG_RED | FLAG_GREEN | FLAG_BLUE | FLAG_ALPHA | FLAG_CUSTOM_0,
                           FLAG_RED | FLAG_GREEN | FLAG_BLUE | FLAG_ALPHA | FLAG_CUSTOM_0,
                           FLAG_RED | FLAG_GREEN | FLAG_BLUE | FLAG_CUSTOM_0);
    model_->setParam(PARAM_ALPHA, 5.f, 0.f);
    model_->setParam(PARAM_CUSTOM_0, -10, 0, 10, 30);
    model_->setLifeTime(5.0f, 10.0f);
    setModel(model_);
    
    sphere_ = Sphere::create();
    emitter_ = StaticEmitter::create();
    emitter_->setZone(sphere_);
    emitter_->setFlow(30);
  	addEmitter(emitter_);
    
    setGravity(Vector3D(0.0f, 0.f, 0.0f));
    setFriction(0.2f);

    changePosition();
}
  
  static StaticGroup *create() {
    StaticGroup *obj = new StaticGroup();
    registerObject(obj);
    return obj;
  }
  
  void changePosition() {
    float w = 150.f;
    sphere_->setPosition(Vector3D(ofRandom(-w, w),
                                  ofRandom(-w, w),
                                  ofRandom(-w, w)));
    set_color(ofFloatColor(ofRandom(1), ofRandom(1), ofRandom(1)));
  }

  void set_color(ofFloatColor color) {
    model_->setParam(PARAM_RED, 0, color.r, 1, 1);
    model_->setParam(PARAM_GREEN, 0, color.g, 1, 1);
    model_->setParam(PARAM_BLUE, 0, color.b, 1, 1);
  }

 private:
  Model *model_;
  StaticEmitter *emitter_;
  Sphere *sphere_;
};


//--------------------------------------------------------------------------------
void Scene2::setup(int texture_id) {
  // Camera
  camera_.setFov(DEFAULT_FOVY);
  camera_.lookAt(ofVec3f(0, 0, 0));
  
  // System
  system_ = System::create();

  // Modifier
  noise_modifier_ = NoiseModifier::create();
  noise_modifier_->set_power(3);
  noise_modifier_->set_scroll_speed(0.3f);

  // Renderers
  renderer_ = GLPointRenderer::create();
  renderer_->setType(POINT_SPRITE);
  renderer_->setTexture(texture_id);
  renderer_->setBlending(BLENDING_ADD);
  renderer_->enableRenderingHint(DEPTH_TEST, false);
  
  for (int i = 0; i < 20; i++) {
    StaticGroup *group = StaticGroup::create();
    group->setRenderer(renderer_);
    group->addModifier(noise_modifier_);
    groups_.push_back(group);
    system_->addGroup(group);
  }

  // Shader
  bokeh_particle_shader_.load();
  bokeh_particle_shader_.set_fov_y(DEFAULT_FOVY);
}


//--------------------------------------------------------------------------------
void Scene2::update() {
  static float offset = 0.f;
  camera_.orbit(ofGetElapsedTimef() * 15.f, 0, ofNoise(offset) * 400.f + 300.f);
  offset += .002f;
  system_->update(ofGetLastFrameTime());
}


//--------------------------------------------------------------------------------
void Scene2::draw() {
  camera_.begin(ofRectangle(0, 0, 1024, 768));
  {
    bokeh_particle_shader_.begin();
    {
      for (vector<StaticGroup*>::iterator it = groups_.begin(); it != groups_.end(); it++) {
        bokeh_particle_shader_.setAttributePointer("rotation", 1, GL_FLOAT, GL_FALSE, (*it)->getParamStride(), (*it)->getParamAddress(PARAM_CUSTOM_0));
      }
      system_->render();
    }
    bokeh_particle_shader_.end();
  }
  camera_.end();
}


//--------------------------------------------------------------------------------
void Scene2::audioIn(int channel, float volume) {
  switch (channel) {
    case 0: {
      bokeh_particle_shader_.set_specular(volume);
      break;
    }
  }
}


//--------------------------------------------------------------------------------
void Scene2::action(int kind, bool start) {
  switch (kind) {
    case 0: {
      if (start) {
        for (vector<StaticGroup*>::iterator it = groups_.begin(); it != groups_.end(); it++) {
          (*it)->changePosition();
        }
      }
      break;
    }
  }
}


//--------------------------------------------------------------------------------
//void Scene2::gotMessage(ofxOscEventArgs &event) {
//  if (event.address(0) == "camera") {
//    if (event.address(1) == "fov") {
//      camera_.setFov(event.value());
//    }
//    bokeh_particle_shader_.setParameter(event.address(1), event.value());
//  }
//}
