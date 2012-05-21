//
//  Scene3.cpp
//  Particles3
//
//  Created by Saqoosha on 12/05/13.
//  Copyright (c) 2012 Saqoosha. All rights reserved.
//

#include "Scene3.h"
#include "NoiseModifier.h"
#include "SPK.h"
#include "SPK_GL.h"

using namespace SPK;
using namespace SPK::GL;

#define DEFAULT_FOVY 30.f


//--------------------------------------------------------------------------------
#pragma mark -
class LissajousEmitter : public StraightEmitter {
  SPK_IMPLEMENT_REGISTERABLE(LissajousEmitter)
  
public:
  LissajousEmitter() : StraightEmitter(Vector3D(0, 0, 0)), step_(0) {
    point_ = SPK::Point::create();
    setZone(point_);
  }
  
  static LissajousEmitter *create() {
    LissajousEmitter *obj = new LissajousEmitter();
    registerObject(obj);
    return obj;
  }
  
  void update(float delta_time) {
    step_ += delta_time;
    float x = sin(step_ * cycle_.x) * radius_.x;
    float y = sin(step_ * cycle_.y) * radius_.y;
    float z = sin(step_ * cycle_.z) * radius_.z;
    Vector3D next(x, y, z);
    setDirection(point_->getPosition() - next);
    point_->setPosition(next);
  }
  
  void set_cycle(float x, float y, float z) {
    cycle_ = ofVec3f(x, y, z);
  }
  
  void set_radius(float x, float y, float z) {
    radius_ = ofVec3f(x, y, z);
  }
  
private:
  SPK::Point *point_;
  ofVec3f cycle_;
  ofVec3f radius_;
  float step_;
};


//--------------------------------------------------------------------------------
#pragma mark -
class LissajousGroup : public Group {
  SPK_IMPLEMENT_REGISTERABLE(LissajousGroup)
public:
  LissajousGroup() : Group(NULL, 3000) {
    model_ = Model::create(FLAG_ALPHA | FLAG_CUSTOM_0,
                           FLAG_RED | FLAG_GREEN | FLAG_BLUE | FLAG_ALPHA | FLAG_CUSTOM_0,
                           FLAG_RED | FLAG_GREEN | FLAG_BLUE | FLAG_CUSTOM_0);
    model_->setParam(PARAM_ALPHA, 2., 0.);
    model_->setParam(PARAM_CUSTOM_0, -10, 0, 10, 30);
    model_->setLifeTime(6.0f, 10.0f);
    setModel(model_);
    
    emitter_ = LissajousEmitter::create();
    emitter_->setFlow(60);
    emitter_->setForce(50, 100);
    emitter_->set_cycle(0.7, 1.0, 1.3);
    emitter_->set_radius(100, 100, 200);
  	addEmitter(emitter_);
    
    noise_modifier_ = NoiseModifier::create();
    noise_modifier_->set_power(.2);
    noise_modifier_->set_scroll_speed(0.3);
    addModifier(noise_modifier_);
    
    setGravity(Vector3D(0.0f, 0.f, 0.0f));
    setFriction(0.2);
  }
  
  static LissajousGroup *create() {
    LissajousGroup *obj = new LissajousGroup();
    registerObject(obj);
    return obj;
  }
  
  void update(float delta_time) {
    emitter_->update(delta_time);
  }
  
  void set_parameters(ofVec3f cycle, ofVec3f radius) {
    emitter_->set_cycle(cycle.x, cycle.y, cycle.z);
    emitter_->set_radius(radius.x, radius.y, radius.z);
  }
  
  void set_color(ofFloatColor color) {
    model_->setParam(PARAM_RED, 1, 1, 1, color.r);
    model_->setParam(PARAM_GREEN, 1, 1, 1, color.g);
    model_->setParam(PARAM_BLUE, 1, 1, 1, color.b);
  }
  
private:
  Model *model_;
  LissajousEmitter *emitter_;
  NoiseModifier *noise_modifier_;
};


//--------------------------------------------------------------------------------
#pragma mark -
void Scene3::setup(int texture_id) {
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
  
  LissajousGroup *group1 = LissajousGroup::create();
  group1->setRenderer(renderer_);
  group1->set_color(ofFloatColor(1, 0, 0));
  LissajousGroup *group2 = LissajousGroup::create();
  group2->setRenderer(renderer_);
  group2->set_parameters(ofVec3f(1.2, 0.8, 1.5), ofVec3f(80, 120, 200));
  group2->set_color(ofFloatColor(0, 1, 0));
  LissajousGroup *group3 = LissajousGroup::create();
  group3->setRenderer(renderer_);
  group3->set_parameters(ofVec3f(0.3, 0.5, 0.8), ofVec3f(280, 220, 300));
  group3->set_color(ofFloatColor(0, 0, 1));
  
  // System
  system_ = System::create();
  system_->addGroup(group1);
  system_->addGroup(group2);
  system_->addGroup(group3);
  
  // Shader
  bokeh_particle_shader_.load();
  bokeh_particle_shader_.set_fov_y(DEFAULT_FOVY);
}


//--------------------------------------------------------------------------------
void Scene3::update() {
  float delta = ofGetLastFrameTime();
  const vector<Group *> groups = system_->getGroups();
  for (int i = 0; i < groups.size(); i++) {
    reinterpret_cast<LissajousGroup *>(groups[i])->update(delta);
  }
  system_->update(delta);
  camera_.orbit(ofGetElapsedTimef() * 5,
                ofSignedNoise(ofGetElapsedTimef() * .1, 100) * 20,
                200 + ofNoise(ofGetElapsedTimef() * .12) * 800);
}


//--------------------------------------------------------------------------------
void Scene3::draw() {
  camera_.begin(ofRectangle(0, 0, 1024, 768));
    bokeh_particle_shader_.begin();
      vector<Group *> groups = system_->getGroups();
      for (vector<Group *>::iterator it = groups.begin(); it != groups.end(); it++) {
        bokeh_particle_shader_.setAttributePointer("rotation", 1, GL_FLOAT, GL_FALSE, (*it)->getParamStride(), (*it)->getParamAddress(PARAM_CUSTOM_0));
      }
      system_->render();
    bokeh_particle_shader_.end();
  camera_.end();
}


//--------------------------------------------------------------------------------
//void Scene3::gotMessage(ofxOscEventArgs &event) {
//  if (event.address(0) == "camera") {
//    if (event.address(1) == "fov") {
//      camera_.setFov(event.value());
//    }
//    bokeh_particle_shader_.setParameter(event.address(1), event.value());
//  }
//}
