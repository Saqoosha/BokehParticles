//
//  Scene5.cpp
//  Particles3
//
//  Created by Saqoosha on 12/05/13.
//  Copyright (c) 2012 Saqoosha. All rights reserved.
//

#include "Scene5.h"

using namespace SPK;
using namespace SPK::GL;

#define DEFAULT_FOVY 30.f


//--------------------------------------------------------------------------------
#pragma mark -
class ModelVertexZone : public SPK::Zone {
  SPK_IMPLEMENT_REGISTERABLE(ModelVertexZone)
  
 public:
  ModelVertexZone(ofxAssimpModelLoader *model)
    : Zone(),
      model_(model),
      current_mesh_index_(0),
      current_mesh_(model->getMesh(0)),
      current_vertex_(0) {

    float scale = ofRandom(20.f, 40.f);
    transform_.scale(scale, scale, scale);
    transform_.rotate(ofRandom(360.), 1., 0., 0.);
    transform_.rotate(ofRandom(360.), 0., 1., 0.);
    transform_.rotate(ofRandom(360.), 0., 0., 1.);
    float w = 100.f;
    transform_.translate(ofRandom(-w, w), ofRandom(-w, w) - 50.f, ofRandom(-w, w));
  }

  static ModelVertexZone *create(ofxAssimpModelLoader *model) {
    ModelVertexZone *obj = new ModelVertexZone(model);
    registerObject(obj);
    return obj;
  }
  
  // Interface
  virtual void generatePosition(Particle &particle, bool full) const {
    ofVec3f v = current_mesh_.getVerticesPointer()[current_vertex_] * transform_;
    particle.position().set(v.x, v.y, v.z);
    
    if (++current_vertex_ == current_mesh_.getNumVertices()) {
      current_vertex_ = 0;
    }
  }
  
  virtual bool contains(const Vector3D& v) const { return false; }
  virtual bool intersects(const Vector3D& v0,const Vector3D& v1,Vector3D* intersection,Vector3D* normal) const { return false; }
  virtual void moveAtBorder(Vector3D& v,bool inside) const {};
  
  virtual Vector3D computeNormal(const Vector3D& point) const {
		Vector3D normal(point - getTransformedPosition());
		normalizeOrRandomize(normal);
		return normal;
	}

 private:
  ofxAssimpModelLoader *model_;
  ofMesh current_mesh_;
  int current_mesh_index_;
  mutable int current_vertex_;
  ofMatrix4x4 transform_;
};


//--------------------------------------------------------------------------------
#pragma mark -
class ModelVertexGroup : public Group {
  SPK_IMPLEMENT_REGISTERABLE(ModelVertexGroup)

 public:
  ModelVertexGroup(ofxAssimpModelLoader *model)
    : Group(NULL, 2000) {

    model_ = Model::create(FLAG_ALPHA | FLAG_CUSTOM_0,
                           FLAG_ALPHA | FLAG_CUSTOM_0,
                           FLAG_CUSTOM_0);
    model_->setParam(PARAM_ALPHA, 10., 0.);
    model_->setParam(PARAM_CUSTOM_0, -10, 0, 10, 30);
    model_->setLifeTime(8.0f, 10.0f);
    setModel(model_);
    
    // Zone
    model_zone_ = ModelVertexZone::create(model);
    
    // Emitter
    emitter_ = StaticEmitter::create();
    emitter_->setZone(model_zone_);
    emitter_->setTank(model->getMesh(0).getNumVertices());
    emitter_->setFlow(-1);
    addEmitter(emitter_);
    
    setGravity(Vector3D(0.f, 5.f, 0.f));
    setFriction(0.2f);
  }
  
  static ModelVertexGroup *create(ofxAssimpModelLoader *model) {
    ModelVertexGroup *obj = new ModelVertexGroup(model);
    registerObject(obj);
    return obj;
  }
  
  inline void set_color(ofFloatColor color) {
    model_->setParam(PARAM_RED, color.r);
    model_->setParam(PARAM_GREEN, color.g);
    model_->setParam(PARAM_BLUE, color.b);
  }
  
private:
  Model *model_;
  ModelVertexZone *model_zone_;
  StaticEmitter *emitter_;
  NoiseModifier *noise_modifier_;
};


//--------------------------------------------------------------------------------
#pragma mark -
ofxAssimpModelLoader *load(string path) {
  ofxAssimpModelLoader *model = new ofxAssimpModelLoader();
  model->loadModel(path);
  return model;
}
  
void Scene5::setup(int texture_id) {
  // Camera
  camera_.setFov(DEFAULT_FOVY);
  camera_.setPosition(0, 50, 500);
  camera_.lookAt(ofVec3f(0, 0, 0));
  
  models_.push_back(load("cube2.dae"));
  models_.push_back(load("sphere.dae"));

  // Renderers
  renderer_ = GLPointRenderer::create();
  renderer_->setType(POINT_SPRITE);
  renderer_->setTexture(texture_id);
  renderer_->setBlending(BLENDING_ADD);
  renderer_->enableRenderingHint(DEPTH_TEST, false);
  renderer_->setShared(true);
  
  // Modifier
  noise_modifier_ = NoiseModifier::create();
  noise_modifier_->set_power(.5f);
  noise_modifier_->set_scroll_speed(0.1f);
  noise_modifier_->setShared(true);
  
  // Shader
  bokeh_particle_shader_.load();
  bokeh_particle_shader_.set_fov_y(DEFAULT_FOVY);
}


//--------------------------------------------------------------------------------
void Scene5::update() {
  static float offset = 0.f;
  camera_.orbit(ofGetElapsedTimef() * -8.f, 0, ofNoise(offset) * 400.f + 300.f);
  offset += .002f;

  float delta_time = ofGetLastFrameTime();
  for (GroupArray::iterator it = groups_.begin(); it != groups_.end();) {
    if ((*it)->update(delta_time)) {
      it++;
    } else {
//      SPK_Destroy(*it);
      delete *it;
      it = groups_.erase(it);
    }
  }
}


//--------------------------------------------------------------------------------
void Scene5::draw() {
  camera_.begin(ofRectangle(0, 0, 1024, 768));
  {
    bokeh_particle_shader_.begin();
    {
      for (GroupArray::iterator it = groups_.begin(); it != groups_.end(); it++) {
        bokeh_particle_shader_.setAttributePointer("rotation", 1, GL_FLOAT, GL_FALSE, (*it)->getParamStride(), (*it)->getParamAddress(PARAM_CUSTOM_0));
        (*it)->render();
      }
    }
    bokeh_particle_shader_.end();
  }
  camera_.end();
}


//--------------------------------------------------------------------------------
void Scene5::action(int kind, bool start) {
  if (!start) return;
  
  switch (kind) {
    case 0:
    case 1: {
      newModel(kind);
      break;
    }
  }
}


//--------------------------------------------------------------------------------
void Scene5::newModel(int index) {
  ModelVertexGroup *group = ModelVertexGroup::create(models_[index]);
  group->addModifier(noise_modifier_);
  group->setRenderer(renderer_);

  static float offset = 0.f;
  offset += .02f;
  group->set_color(ofFloatColor(ofRandom(1.f), ofRandom(1.f), ofRandom(1.f)));
  
  groups_.push_back(group);
}
