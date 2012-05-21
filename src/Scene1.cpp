//
//  Scene1.cpp
//  Particles3
//
//  Created by Saqoosha on 12/05/09.
//  Copyright (c) 2012 Saqoosha. All rights reserved.
//

#include "Scene1.h"

using namespace SPK;
using namespace SPK::GL;


//--------------------------------------------------------------------------------
class FountainGroup : public Group {
  SPK_IMPLEMENT_REGISTERABLE(FountainGroup)
 public:
  FountainGroup() : Group(NULL, 1500) {
    model_ = Model::create(FLAG_ALPHA | FLAG_CUSTOM_0,
                           FLAG_ALPHA | FLAG_CUSTOM_0,
                           FLAG_CUSTOM_0);
    model_->setParam(PARAM_ALPHA, 10., 0.);
    model_->setParam(PARAM_CUSTOM_0, -10, 0, 10, 30);
    model_->setLifeTime(8.0f, 10.0f);
    setModel(model_);
    
    point_ = SPK::Point::create(Vector3D(0, 1, 0));
    
    emitter_ = SphericEmitter::create(Vector3D(0, 1, 0), 0, 0.2 * PI);
    emitter_->setZone(point_);
    emitter_->setFlow(1);
    emitter_->setForce(200, 280);
    addEmitter(emitter_);
    flow_.set_low(1);
    
    // Obstacle
    Plane *groundPlane = Plane::create();
    addModifier(Obstacle::create(groundPlane, INTERSECT_ZONE, 0.6, 1.0));
    
    // Modifier
    noise_modifier_ = NoiseModifier::create();
    addModifier(noise_modifier_);
    
    // Group
    setGravity(Vector3D(0.0f, -80.f, 0.0f));
    setFriction(0.8);
  }
  
  static FountainGroup *create() {
    FountainGroup *obj = new FountainGroup();
    registerObject(obj);
    return obj;
  }
  
  inline void update() {
    emitter_->setFlow(flow_.update());
  }

  inline void set_flow(float flow) { flow_.attack(flow); }
  inline void set_color(ofFloatColor color) {
    model_->setParam(PARAM_RED, color.r);
    model_->setParam(PARAM_GREEN, color.g);
    model_->setParam(PARAM_BLUE, color.b);
  }
  
  inline void set_position(Vector3D position) { point_->setPosition(position); }

 private:
  Model *model_;
  SPK::Point *point_;
  SphericEmitter *emitter_;
  NoiseModifier *noise_modifier_;
  DecayValue flow_;
};


//--------------------------------------------------------------------------------
Scene1::Scene1() : BaseScene() {
}


//--------------------------------------------------------------------------------
void Scene1::setup(int texture_id) {
  camera_.setFov(30);
  randomizeCamera();
  
  // Renderers
  renderer_ = GLPointRenderer::create();
  renderer_->setType(POINT_SPRITE);
  renderer_->setTexture(texture_id);
  renderer_->setBlending(BLENDING_ADD);
  renderer_->enableRenderingHint(DEPTH_TEST, false);
  
  system_ = System::create();
  for (int i = -2; i <= 2; i++) {
    FountainGroup *group = FountainGroup::create();
    group->setRenderer(renderer_);
    group->set_position(Vector3D(i * 150, 1, 0));
    groups_.push_back(group);
    system_->addGroup(group);
  }
  
  step_ = 0.f;
  
  bokeh_particle_shader_.load();
  bokeh_particle_shader_.set_fov_y(30);
}


//--------------------------------------------------------------------------------
void Scene1::update() {
  step_ += ofGetLastFrameTime() * 0.5f;
  for (FountainGroupArray::iterator it = groups_.begin(); it != groups_.end(); it++) {
    (*it)->set_color(ofFloatColor(0.6f + 0.4f * sin(step_),
                                  0.6f + 0.4f * sin(step_ + PI * 2.0f / 3.0f),
                                  0.6f + 0.4f * sin(step_ + PI * 4.0f / 3.0f)));
    (*it)->update();
  }

  system_->update(ofGetLastFrameTime());

  camera_position_ += (next_camera_position_ - camera_position_) * .002;
  camera_.setPosition(camera_position_);
  camera_target_ += (next_camera_target_ - camera_target_) * .002;
  camera_.lookAt(camera_target_);
}


//--------------------------------------------------------------------------------
void Scene1::draw() {
  camera_.begin(ofRectangle(0, 0, 1024, 768));
    bokeh_particle_shader_.begin();
      for (FountainGroupArray::iterator it = groups_.begin(); it != groups_.end(); it++) {
        bokeh_particle_shader_.setAttributePointer("rotation", 1, GL_FLOAT, GL_FALSE, (*it)->getParamStride(), (*it)->getParamAddress(PARAM_CUSTOM_0));
      }
      system_->render();
    bokeh_particle_shader_.end();
  camera_.end();
}


//--------------------------------------------------------------------------------
void Scene1::gotMessage(ofxOscEventArgs &event) {
  if (event.address(0) == "camera") {
    if (event.address(1) == "fov") {
      camera_.setFov(event.value());
    }
    bokeh_particle_shader_.setParameter(event.address(1), event.value());
  }
}


//--------------------------------------------------------------------------------
void Scene1::noteOn(int channel, int pitch, float velocity) {
  channel = ofClamp(channel, 0, groups_.size() - 1);
  groups_[channel]->set_flow(500 * velocity);
}


//--------------------------------------------------------------------------------
void Scene1::action(int kind, bool start) {
  switch (kind) {
    case 0: {
      if (start) {
        randomizeCamera();
      }
      break;
    }
    case 1: {
      if (start) {
        groups_[int(ofRandom(groups_.size()))]->set_flow(2000);
      }
      break;
    }
  }
}


//--------------------------------------------------------------------------------
void Scene1::randomizeCamera() {
  ofSeedRandom();
  camera_position_.x = ofRandom(-1500, 1500);
  camera_position_.y = ofRandom(0, 500);
  camera_position_.z = ofRandom(-1000, 1000);
  next_camera_position_.x = ofRandom(-1500, 1500);
  next_camera_position_.y = ofRandom(0, 500);
  next_camera_position_.z = ofRandom(-1000, 1000);
  camera_target_.x = ofRandom(-500, 500);
  camera_target_.y = ofRandom(0, 200);
  camera_target_.z = ofRandom(-200, 200);
  next_camera_target_.x = ofRandom(-500, 500);
  next_camera_target_.y = ofRandom(0, 200);
  next_camera_target_.z = ofRandom(-200, 200);
}
