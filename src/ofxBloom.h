//
//  ofxBloom.h
//  Particles3
//
//  Created by Saqoosha on 12/05/09.
//  Copyright (c) 2012 Saqoosha. All rights reserved.
//

#pragma once

#include "ofMain.h"


class ofxBloom {
 public:
  ofxBloom();
  
  void setup(int width, int height);
  void begin();
  void end();
	void draw(float x, float y) {
    bloomed_fbo_.draw(x, y);
  }
	void draw(float x, float y, float width, float height) {
    bloomed_fbo_.draw(x, y, width, height);
  }
  
  void set_threshold(float threshold) { bright_threshold_ = threshold; }
  void set_strength(float strength) { bloom_strength_ = strength;}
  
 protected:
  vector<ofFbo*> buffers_;
  ofFbo source_fbo_;
  ofFbo bloomed_fbo_;
  ofShader bright_part_shader_;
  float bright_threshold_;
  ofShader vertical_blur_shader_;
  ofShader horizontal_blur_shader_;
  ofShader composite_shader_;
  float bloom_strength_;
};
