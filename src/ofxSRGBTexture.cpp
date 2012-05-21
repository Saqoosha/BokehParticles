//
//  ofxSRGBTexture.cpp
//  Particles3
//
//  Created by Saqoosha on 12/05/13.
//  Copyright (c) 2012 Saqoosha. All rights reserved.
//

#include "ofxSRGBTexture.h"


bool ofxSRGBTexture::inited_ = false;
ofShader ofxSRGBTexture::gamma_corrector_;


void ofxSRGBTexture::init() {
  gamma_corrector_.load("", "shaders/gamma-correction.fs");
  inited_ = true;
}


void ofxSRGBTexture::loadImage(string path, GLenum texture_target) {
  if (!inited_) init();
  
  ofImage image;
  image.loadImage(path);
  
  ofFbo::Settings settings;
  settings.width = image.getWidth();
  settings.height = image.getHeight();
  settings.useDepth = false;
  settings.useStencil = false;
  settings.textureTarget = texture_target;
  settings.internalformat = GL_RGBA;
  allocate(settings);
  
  begin();
    ofClear(0);
    gamma_corrector_.begin();
      gamma_corrector_.setUniform1f("gamma", 2.2);
      image.draw(0, 0);
    gamma_corrector_.end();
  end();
  
  image.clear();
};
