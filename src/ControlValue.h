//
//  ControlValue.h
//  Particles4
//
//  Created by Saqoosha on 12/05/18.
//  Copyright (c) 2012 Saqoosha. All rights reserved.
//

#pragma once

class ControlValue {
 public:
  ControlValue(float base = 0.f, float variation = 0.f)
    : base_(base),
      variation_(variation) {
  }
  
  inline float value(float scale = 1.f) {
    float v = variation_ * scale;
    return base_ + ofRandom(-v, v);
  }
  inline float base() { return base_; }
  inline float variation() { return variation_; }
  inline void set_base(float base) { base_ = base; };
  inline void set_variation(float variation) { variation_ = variation; };
  
 private:
  float base_;
  float variation_;
};
