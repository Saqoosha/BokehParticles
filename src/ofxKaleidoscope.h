//
//  ofxKaleidoscope.h
//  GaussBlur
//
//  Created by Saqoosha on 12/05/18.
//  Copyright (c) 2012 Saqoosha. All rights reserved.
//

#pragma once

#include "ofxFXObject.h"

#define STRINGIFY(A) #A


class ofxKaleidoscope : public ofxFXObject {
 public:
  ofxKaleidoscope() : num_segments_(1.f), source_angle_offset_(0.f), target_angle_offset_(0.f) {
    passes = 1;
    internalFormat = GL_RGBA16F;
    fragmentShader = STRINGIFY
    (
     uniform sampler2DRect tex0;
     uniform float segment_angle;
     uniform float source_angle_offset;
     uniform float target_angle_offset;
     
     void main() {
       vec2 center = vec2(512, 384);
       vec2 c = gl_TexCoord[0].xy - center;
       float radius = length(c);
       float angle = atan(c.y, c.x) + source_angle_offset;
       angle = mod(angle, segment_angle) + target_angle_offset;
       gl_FragColor = gl_Color * texture2DRect(tex0, vec2(cos(angle) * radius, sin(angle) * radius) + center);
     }
    );
  }
  
  void update() {
    pingPong.dst->begin();
    {
      ofClear(0);
      shader.begin();
      {
        shader.setUniformTexture("tex0", textures[0].getTextureReference(), 0);
        shader.setUniform1f("segment_angle", 2.f * PI / num_segments_);
        shader.setUniform1f("source_angle_offset", source_angle_offset_);
        shader.setUniform1f("target_angle_offset", target_angle_offset_);
        renderFrame();
      }
      shader.end();
    }
    pingPong.dst->end();
  }
  
  void set_num_segments(float segments) { num_segments_ = max(1.f, segments); }
  void set_source_angle_offset(float offset) { source_angle_offset_ = offset; }
  void set_target_angle_offset(float offset) { target_angle_offset_ = offset; }
  
  
 protected:
  float num_segments_;
  float source_angle_offset_;
  float target_angle_offset_;
};
