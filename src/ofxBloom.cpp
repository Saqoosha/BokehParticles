//
//  ofxBloom.cpp
//  Particles3
//
//  Created by Saqoosha on 12/05/09.
//  Copyright (c) 2012 Saqoosha. All rights reserved.
//

#include "ofxBloom.h"

#define NUM_ITERATION 4


//--------------------------------------------------------------------------------
ofxBloom::ofxBloom() : bright_threshold_(0.1f), bloom_strength_(0.3f) {
}


//--------------------------------------------------------------------------------
void ofxBloom::setup(int width, int height) {
  ofFbo::Settings settings;
  settings.width = width;
  settings.height = height;
  settings.useDepth = false;
  settings.useStencil = false;
  settings.internalformat = GL_RGB16F;
  
  source_fbo_.allocate(settings);
  bloomed_fbo_.allocate(settings);
  
  for (int i = 0; i < NUM_ITERATION; i++) {
    settings.width >>= 1;
    settings.height >>= 1;
    for (int j = 0; j < 2; j++) {
      ofFbo *buf = new ofFbo();
      buf->allocate(settings);
      buffers_.push_back(buf);
    }
  }
  
  bright_part_shader_.load("", "ofxBloom/bright-part.fs");
  vertical_blur_shader_.load("", "ofxBloom/gaussian-blur-vertical.fs");
  horizontal_blur_shader_.load("", "ofxBloom/gaussian-blur-horizontal.fs");
  composite_shader_.load("", "ofxBloom/bloom-composite.fs");
}


//--------------------------------------------------------------------------------
void ofxBloom::begin() {
  source_fbo_.begin();
  ofClear(0);
}


//--------------------------------------------------------------------------------
void ofxBloom::end() {
  source_fbo_.end();
  
  buffers_[0]->begin();
    ofClear(0);
    bright_part_shader_.begin();
      bright_part_shader_.setUniform1f("threshold", bright_threshold_);
      source_fbo_.draw(0, 0, buffers_[0]->getWidth(), buffers_[0]->getHeight());
    bright_part_shader_.end();
  buffers_[0]->end();
  
  for (int i = 0; i < buffers_.size(); i += 2) {
    ofFbo *b0 = buffers_[i];
    ofFbo *b1 = buffers_[i + 1];
    
    b1->begin();
      ofClear(0);
      vertical_blur_shader_.begin();
        b0->draw(0, 0);
      vertical_blur_shader_.end();
    b1->end();
    
    b0->begin();
      ofClear(0);
      horizontal_blur_shader_.begin();
        b1->draw(0, 0);
      horizontal_blur_shader_.end();
    b0->end();
    
    if (i < buffers_.size() - 2) {
      ofFbo *b2 = buffers_[i + 2];
      b2->begin();
        ofClear(0);
        b0->draw(0, 0, b2->getWidth(), b2->getHeight());
      b2->end();
    }
  }
  
  bloomed_fbo_.begin();
    ofClear(0);
    composite_shader_.begin();
      composite_shader_.setUniformTexture("tex0", source_fbo_.getTextureReference(), 0);
      composite_shader_.setUniformTexture("tex1", buffers_[0]->getTextureReference(), 1);
      composite_shader_.setUniformTexture("tex2", buffers_[2]->getTextureReference(), 2);
      composite_shader_.setUniformTexture("tex3", buffers_[4]->getTextureReference(), 3);
      composite_shader_.setUniformTexture("tex4", buffers_[6]->getTextureReference(), 4);
      composite_shader_.setUniform1f("strength", bloom_strength_);
      source_fbo_.draw(0, 0);
    composite_shader_.end();
  bloomed_fbo_.end();
}


