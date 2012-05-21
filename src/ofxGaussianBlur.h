//
//  ofxGaussianBlur.h
//  GaussBlur
//
//  Created by Saqoosha on 12/05/18.
//  Copyright (c) 2012 Saqoosha. All rights reserved.
//

#pragma once

#include "ofxFXObject.h"


class ofxGaussianBlur : public ofxFXObject {	
 public:
  ofxGaussianBlur() {
    passes = 5;
    internalFormat = GL_RGB16F;
    scale = 1.f;
  }
  
  ofxGaussianBlur &setRadius(float _scale) {
    this->scale = _scale;
    return *this;
  }
  
  void update() {
    ofPushStyle();
    
    pingPong.src->begin();
    textures[0].draw(0,0);
    pingPong.src->end();
    
    for(int i = 0; i < passes; i++) {
      for(int j = 0; j < 2; j++) {    
        pingPong.dst->begin();
        blur_shader[j].begin();
        blur_shader[j].setUniformTexture("backbuffer", pingPong.src->getTextureReference(), 0 );
        blur_shader[j].setUniform1f("scale", scale);
        renderFrame();
        blur_shader[j].end();
        pingPong.dst->end();
        
        pingPong.swap();
      }
    }
    
    pingPong.swap();
    
    ofPopStyle();
  }
  
 protected:
  bool compileCode() {
    nTextures = 1;
    textures = new ofFbo[nTextures];
    
    for (int i = 0; i < nTextures; i++) {
      initFbo(textures[i], width, height, internalFormat);
    }
    
    string fragmentHorizontalBlurShader = STRINGIFY
    (
     uniform sampler2DRect backbuffer;
     uniform float scale = 1.0;
     
     const float offset[3] = float[]( 0.0, 1.3846153846, 3.2307692308 );
     const float weight[3] = float[]( 0.2270270270, 0.3162162162, 0.0702702703 );
     
     void main() {
       gl_FragColor = texture2DRect(backbuffer, gl_TexCoord[0].xy) * weight[0];
       for (int i = 1; i < 3; i++) {
         gl_FragColor += texture2DRect(backbuffer, gl_TexCoord[0].xy + vec2(offset[i] * scale, 0.0)) * weight[i];
         gl_FragColor += texture2DRect(backbuffer, gl_TexCoord[0].xy - vec2(offset[i] * scale, 0.0)) * weight[i];
       }
     }
    );
    blur_shader[0].unload();
    blur_shader[0].setupShaderFromSource(GL_FRAGMENT_SHADER, "#version 120\n" + fragmentHorizontalBlurShader);
    blur_shader[0].linkProgram();
    
    string fragmentVerticalBlurShader = STRINGIFY
    (
     uniform sampler2DRect backbuffer;
     uniform float scale = 1.0;
     
     const float offset[3] = float[]( 0.0, 1.3846153846, 3.2307692308 );
     const float weight[3] = float[]( 0.2270270270, 0.3162162162, 0.0702702703 );
     
     void main() {
       gl_FragColor = texture2DRect(backbuffer, gl_TexCoord[0].xy) * weight[0];
       for (int i = 1; i < 3; i++) {
         gl_FragColor += texture2DRect(backbuffer, gl_TexCoord[0].xy + vec2(0.0, offset[i] * scale)) * weight[i];
         gl_FragColor += texture2DRect(backbuffer, gl_TexCoord[0].xy - vec2(0.0, offset[i] * scale)) * weight[i];
       }
     }
    );
    blur_shader[1].unload();
    blur_shader[1].setupShaderFromSource(GL_FRAGMENT_SHADER, "#version 120\n" + fragmentVerticalBlurShader);
    blur_shader[1].linkProgram();
    
    return true;
  }
  
  ofShader blur_shader[2];
  float scale;
  int passes;
};
