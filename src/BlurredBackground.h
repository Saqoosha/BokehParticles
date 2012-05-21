//
//  BlurredBackground.h
//  GaussBlur
//
//  Created by Saqoosha on 12/05/18.
//  Copyright (c) 2012 Saqoosha. All rights reserved.
//

#pragma once

#include "ofMain.h"

#include "ofxSwapBuffer.h"
#include "ofxGammaCorrection.h"
#include "ofxGaussianBlur.h"


class BlurredBackground : public ofBaseDraws {
 public:
  BlurredBackground() : feedback_strength_(.99f) {}
  
  void setup(string movie_path) {
    int w = 64;
    int h = 48;
    feedback_.allocate(w, h, GL_RGB16F);
    gamma_in_.allocate(w, h, GL_RGB16F);
    gamma_out_.allocate(w, h, GL_RGB16F);
    gamma_out_.set_gamma(2.2);
    blur_.allocate(w, h, GL_RGB16F);
    
    video_.loadMovie(movie_path);
    video_.setSpeed(0.3);
//    video_.setLoopState(OF_LOOP_NORMAL);
    video_.setVolume(0);
    play();
    
    ofAddListener(ofEvents.update, this, &BlurredBackground::update);
  }
  
  void play() { video_.play(); }
  void stop() { video_.stop(); }
  void setPosition(float percent, bool immediate = false) {
//    ofLog(OF_LOG_ERROR, "setPosition: %f", percent);
    video_.setPosition(ofClamp(percent, 0.f, .99f));
    if (immediate) {
      video_.update();
      feedback_[0].begin();
      video_.draw(0, 0, 64, 48);
      feedback_[0].end();
      feedback_[1].begin();
      video_.draw(0, 0, 64, 48);
      feedback_[1].end();
    }
  }
  void randomPosition(bool immediate = false) { setPosition(ofRandom(1.f), immediate); }
  
  void update(ofEventArgs &event) {
    if (video_.getPosition() * video_.getDuration() > video_.getDuration() - 3.f) {
      video_.setPosition(0);
    }
    video_.update();
  }
  
  void draw(float x, float y) { draw(x, y, 64, 48); }
  void draw(float x, float y, float w, float h) {
    ofPushStyle();
    ofEnableAlphaBlending();
    
    feedback_.dst->begin();
    {
      ofSetColor(255);
      video_.draw(0, 0, feedback_.dst->getWidth(), feedback_.dst->getHeight());
      glColor4f(1, 1, 1, feedback_strength_);
      feedback_.src->draw(0, 0, feedback_.src->getWidth(), feedback_.src->getHeight());
    }
    feedback_.dst->end();
    feedback_.swap();
    
    gamma_in_.begin();
    feedback_.src->draw(0, 0);
    gamma_in_.end();
    gamma_in_.update();
    
    blur_.begin();
    gamma_in_.draw();
    blur_.end();
    blur_.update();
    
    gamma_out_.begin();
    blur_.draw();
    gamma_out_.end();
    gamma_out_.update();
    
    ofPopStyle();
    
    gamma_out_.draw(x, y, w, h);
  }
  
  float getWidth() { return 64.f; }
  float getHeight() { return 48.f; }
  
  float feedback_strength() { return feedback_strength_; }
  void set_feedback_strength(float feedback_strength) { feedback_strength_ = feedback_strength; }
  
// protected:
  ofxSwapBuffer feedback_;
  ofxGammaCorrection gamma_in_;
  ofxGammaCorrection gamma_out_;
  ofxGaussianBlur blur_;
  ofVideoPlayer video_;
  
  float feedback_strength_;
};
