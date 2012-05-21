//
//  FinalComp.h
//  Particles3
//
//  Created by Saqoosha on 12/05/09.
//  Copyright (c) 2012 Saqoosha. All rights reserved.
//

#pragma once

#include "ofMain.h"
#include "ControlValue.h"


class FinalComp : public ofShader  {
 public:
  FinalComp()
    : chromatic_aberration_center_(512, 384),
      chromatic_aberration_strength_(0.1),
      vignette_center_(512, 384),
      vignette_radius_(512),
      vignette_power_(0.5),
      exposure_(2.0),
      noise_strength_(0.1),
      kaleidoscope_segments_(1.f) {
  }
  
  void setup() {
    load("", "shaders/composite.fs");
    chromatic_aberration_center_.x = vignette_center_.x = ofGetWidth() * .5;
    chromatic_aberration_center_.y = vignette_center_.y = ofGetHeight() * .5;
  }
  
  void begin() {
    ofShader::begin();
    setUniform2fv("aberration_center", chromatic_aberration_center_.getPtr());
    setUniform1f("aberration_strength", chromatic_aberration_strength_);
    setUniform2fv("vignette_center", vignette_center_.getPtr());
    setUniform1f("vignette_radius", vignette_radius_);
    setUniform1f("vignette_power", vignette_power_);
    setUniform1f("exposure", exposure_);
    setUniform1f("gamma_correction", 1.0 / 2.2);
    setUniform1f("time", ofGetElapsedTimef());
    setUniform1f("noise_strength", noise_strength_);
    setUniform1f("segment_angle", 2.f * PI / kaleidoscope_segments_);
    if (kaleidoscope_segments_ > 1.f) {
      setUniform1f("source_angle_offset", ofGetElapsedTimef() * .2);
      setUniform1f("target_angle_offset", ofGetElapsedTimef() * -.1);
    } else {
      setUniform1f("source_angle_offset", 0);
      setUniform1f("target_angle_offset", 0);
    }
  }
  
  inline void set_chromatic_aberration_center(float x, float y) {
    chromatic_aberration_center_.x = x;
    chromatic_aberration_center_.y = y;
  }
  inline void set_chromatic_aberration_center(ofVec2f center) {
    chromatic_aberration_center_ = center;
  }
  
  inline void set_chromatic_aberration_strength(float strength) {
    chromatic_aberration_strength_ = strength;
  }
  
  inline void set_vignette_radius(float radius) { vignette_radius_ = radius; }
  inline void set_vignette_power(float power) { vignette_power_ = power; }
  inline void set_exposure(float exposure) { exposure_ = exposure; }
  inline void set_noise_strength(float noise_strength) { noise_strength_ = noise_strength; }
  inline void set_kaleidoscope_segments(float segments) { kaleidoscope_segments_ = max(1.f, segments); }
  
 private:
  ofVec2f chromatic_aberration_center_;
  float chromatic_aberration_strength_;
  ofVec2f vignette_center_;
  float vignette_radius_;
  float vignette_power_;
  float exposure_;
  float noise_strength_;
  float kaleidoscope_segments_;
};
