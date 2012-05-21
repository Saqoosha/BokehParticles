//
//  BokehParticleShader.h
//  Particles3
//
//  Created by Saqoosha on 12/05/13.
//  Copyright (c) 2012 Saqoosha. All rights reserved.
//

#pragma once

#include "ofMain.h"


class BokehParticleShader : public ofShader {
 public:
  BokehParticleShader();
  
  void load();
  void begin();
  void setAttributePointer(const char *name, GLint size, GLenum type, GLboolean normalized, GLsizei stride, const GLvoid *pointer);
  
  void setParameter(string name, float value);

  inline void set_shininess(float shininess) { shininess_ = shininess; }
  inline float specular() { return specular_; }
  inline void set_specular(float specular) { specular_ = specular; }
  inline void set_diffuse(float diffuse) { diffuse_ = diffuse; }
  inline void set_resolution(ofVec2f resolution) { resolution_ = resolution; }
  inline float fov_y() { return fov_y_; }
  inline void set_fov_y(float fov_y) { fov_y_ = fov_y; }
  inline const float focal_length() { return focal_length_; }
  inline void set_focal_length(float focal_length) { focal_length_ = focal_length; }
  inline float f_number() { return f_number_; }
  inline void set_f_number(float f_number) { f_number_ = f_number; }
  inline float focus_plane() { return focus_plane_; }
  inline void set_focus_plane(float focus_plane) { focus_plane_ = focus_plane; }
  
 protected:
  inline void calcParameters() {
    aparture_ = focal_length_ / f_number_;
    float image_plane = focus_plane_ * focal_length_ / (focus_plane_ - focal_length_);
    mm_per_pixel_ = fabs(tanf(fov_y_ / 2) * image_plane * 2 / resolution_.y);
  }
  
 protected:
  ofVec2f resolution_;
  float fov_y_;
  float focal_length_;
  float f_number_;
  float aparture_;
  float focus_plane_;
  float mm_per_pixel_;
  float shininess_;
  float specular_;
  float diffuse_;
};
