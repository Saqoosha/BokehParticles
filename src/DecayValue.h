//
//  DecayValue.h
//  Particles4
//
//  Created by Saqoosha on 12/05/18.
//  Copyright (c) 2012 Saqoosha. All rights reserved.
//

#pragma once

class DecayValue {
 public:
  DecayValue() : current_(0), low_(0), drag_(.4f) {}

  inline void attack(float value) {
    current_ = value;
  }

  inline float update() {
    current_ += (low_ - current_) * drag_;
    return current_;
  }

  inline float current() { return current_; }
  inline void set_low(float low) { low_ = low; }
  inline void set_drag(float drag) { drag_ = ofClamp(drag, 1e-5, 1); }
  
 protected:
  float current_;
  float low_;
  float drag_;
};
