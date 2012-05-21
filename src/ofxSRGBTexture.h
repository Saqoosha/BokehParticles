//
//  ofxSRGBTexture.h
//  Particles3
//
//  Created by Saqoosha on 12/05/12.
//  Copyright (c) 2012 Saqoosha. All rights reserved.
//

#pragma once

#include "ofMain.h"


class ofxSRGBTexture : public ofFbo {
 public:
  ofxSRGBTexture() : ofFbo() {};
  
  static void init();
  void loadImage(string path, GLenum texture_target = GL_TEXTURE_RECTANGLE_ARB);
  
  GLuint texture_id() {
    return getTextureReference().texData.textureID;
  };
  
 private:
  static bool inited_;
  static ofShader gamma_corrector_;
};
