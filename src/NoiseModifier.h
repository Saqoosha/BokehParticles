//
//  NoiseModifier.h
//  Particles3
//
//  Created by Saqoosha on 12/05/06.
//  Copyright (c) 2012 Saqoosha. All rights reserved.
//

#pragma once

#include "SPK.h"
#include "MSAPerlin.h"

namespace SPK {

class NoiseModifier : public Modifier {
  SPK_IMPLEMENT_REGISTERABLE(NoiseModifier)
  
 public:
  NoiseModifier()
    : Modifier(ALWAYS | INSIDE_ZONE | OUTSIDE_ZONE, ALWAYS, false, false),
      size_(100),
      scroll_speed_(1),
      power_(20) {
    perlin_ = new MSA::Perlin();
  };
  
  static inline NoiseModifier* create() {
    NoiseModifier* obj = new NoiseModifier();
    registerObject(obj);
    return obj;
  };
  
  float get_size() { return size_; };
  void set_size(float size) { size_ = size; };
  float get_scroll_speed() { return scroll_speed_; };
  void set_scroll_speed(float scroll_speed) { scroll_speed_ = scroll_speed; };
  float get_power() { return power_; };
  void set_power(float power) { power_ = power; };
  
 protected:
  MSA::Perlin *perlin_;
  
 private:
  void modify(Particle& particle,float deltaTime) const {
    Vector3D pos = particle.position() / size_;
    float p = ofGetElapsedTimef() / 20. * scroll_speed_;;
    float fx = perlin_->get(p, pos.y, pos.z);
    float fy = perlin_->get(pos.x, p, pos.z);
    float fz = perlin_->get(pos.x, pos.y, p);
    particle.velocity() += Vector3D(fx, fy, fz) * power_;
  };
  
  float size_;
  float scroll_speed_;
  float power_;
};

}
