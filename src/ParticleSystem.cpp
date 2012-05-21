//
//  ParticleSystem.cpp
//  Particles3
//
//  Created by Saqoosha on 12/05/06.
//  Copyright (c) 2012 Saqoosha. All rights reserved.
//

#include "ParticleSystem.h"


using namespace SPK;
using namespace SPK::GL;


ParticleSystem::ParticleSystem() {
}


void ParticleSystem::setup(GLuint texture_id) {
	// Renderers
  GLPointRenderer *renderer = GLPointRenderer::create();
  renderer->setType(POINT_SPRITE);
  renderer->setTexture(texture_id);
	renderer->setBlending(BLENDING_ADD);
	renderer->enableRenderingHint(DEPTH_TEST, false);
  
	// Model
	model_ = Model::create(FLAG_ALPHA | FLAG_CUSTOM_0, FLAG_ALPHA | FLAG_CUSTOM_0, FLAG_CUSTOM_0);
	model_->setParam(PARAM_ALPHA, 10., 0.);
  model_->setParam(PARAM_CUSTOM_0, -10, 0, 10, 30);
	model_->setLifeTime(8.0f, 10.0f);
  
	// Emitter
	emitter_ = SphericEmitter::create(Vector3D(0, 1, 0), 0, 0.2 * PI);
	emitter_->setZone(SPK::Point::create(Vector3D(0, 1, 0)));
	emitter_->setFlow(250);
	emitter_->setForce(100, 180);
  
	// Obstacle
	Plane *groundPlane = Plane::create();
	Obstacle *obstacle = Obstacle::create(groundPlane, INTERSECT_ZONE, 0.6, 1.0);
  
  noise_modifier_ = NoiseModifier::create();
  
	// Group
	group_ = Group::create(model_, 3000);
	group_->addEmitter(emitter_);
	group_->addModifier(obstacle);
  group_->addModifier(noise_modifier_);
	group_->setRenderer(renderer);
	group_->setGravity(Vector3D(0.0f, -80.f, 0.0f));
  group_->setFriction(0.8);
	
	system_ = System::create();
	system_->addGroup(group_);
  
  step_ = 0.0;
}


void ParticleSystem::update() {
  step_ += ofGetLastFrameTime() * 0.5f;
  model_->setParam(PARAM_RED, 0.6f + 0.4f * sin(step_));
  model_->setParam(PARAM_GREEN, 0.6f + 0.4f * sin(step_ + PI * 2.0f / 3.0f));
  model_->setParam(PARAM_BLUE, 0.6f + 0.4f * sin(step_ + PI * 4.0f / 3.0f));
  
  // Updates particle system
  system_->update(ofGetLastFrameTime());
}


void ParticleSystem::draw() {
  system_->render();
}


void ParticleSystem::enableAttribute(GLint location, SPK::ModelParam param) {
  glEnableVertexAttribArray(location);
  glVertexAttribPointer(location, 1, GL_FLOAT, GL_FALSE, group_->getParamStride(), group_->getParamAddress(param));
}


