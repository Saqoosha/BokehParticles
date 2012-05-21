//
//  ofxGammaCorrection.h
//  GaussBlur
//
//  Created by Saqoosha on 12/05/18.
//  Copyright (c) 2012 Saqoosha. All rights reserved.
//

#pragma once

#include "ofxFXObject.h"

#define STRINGIFY(A) #A


class ofxGammaCorrection : public ofxFXObject {
 public:
  ofxGammaCorrection() {
    passes = 1;
    internalFormat = GL_RGBA16F;
    fragmentShader = STRINGIFY
    (
     uniform sampler2DRect tex0;
     uniform float gamma;
     
     void main() {
       vec4 color = texture2DRect(tex0, gl_TexCoord[0].xy);
       gl_FragColor = gl_Color * vec4(pow(color.xyz, vec3(gamma)), color.a);
     }
    );
    cout << fragmentShader << endl;
    gamma_ = 1.0 / 2.2;
  }
  
  void update() {
    pingPong.dst->begin();
      ofClear(0);
      shader.begin();
        shader.setUniformTexture("tex0", textures[0].getTextureReference(), 0);
        shader.setUniform1f("gamma", gamma_);
        renderFrame();
      shader.end();
    pingPong.dst->end();
  }
  
  void set_gamma(float gamma) { gamma_ = gamma; }
  
 protected:
  float gamma_;
};
