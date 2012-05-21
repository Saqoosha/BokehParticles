//
//  BokehParticleShader.cpp
//  Particles4
//
//  Created by Saqoosha on 12/05/17.
//  Copyright (c) 2012 Saqoosha. All rights reserved.
//

#include "BokehParticleShader.h"


//------------------------------------------------------------------------------
BokehParticleShader::BokehParticleShader()
  : ofShader(),
    resolution_(ofGetWidth(), ofGetHeight()),
    fov_y_(30.f),
    focal_length_(50.f),
    f_number_(1.f),
    focus_plane_(1000.f),
    shininess_(128.f),
    specular_(20.f), 
    diffuse_(0.1f) {
}


//------------------------------------------------------------------------------
void BokehParticleShader::load() {
  setGeometryInputType(GL_POINTS);
  setGeometryOutputType(GL_TRIANGLE_STRIP);
  setGeometryOutputCount(4);
  ofShader::load("shaders/bokeh-particles.vs", "shaders/bokeh-particles.fs", "shaders/bokeh-particles.gs");
  printActiveUniforms();
//  printActiveAttributes();
}


//------------------------------------------------------------------------------
void BokehParticleShader::begin() {
  calcParameters();
  ofShader::begin();
  setUniform1f("shininess", shininess_);
  setUniform1f("specular_strength", specular_);
  setUniform1f("diffuse", diffuse_);
  setUniform2fv("resolution", resolution_.getPtr());
  setUniform1f("focal_length", focal_length_);
  setUniform1f("aparture", aparture_);
  setUniform1f("focus_plane", focus_plane_);
  setUniform1f("mm_per_pixel", mm_per_pixel_);
}


//------------------------------------------------------------------------------
void BokehParticleShader::setAttributePointer(const char *name, GLint size, GLenum type, GLboolean normalized, GLsizei stride, const GLvoid *pointer) {
  GLuint location = getAttributeLocation(name);
  glEnableVertexAttribArray(location);
  glVertexAttribPointer(location, size, type, normalized, stride, pointer);
}


//------------------------------------------------------------------------------
void BokehParticleShader::setParameter(string name, float value) {
  if (name == "fov") {
    fov_y_ = value;
  } else if (name == "focallength") {
    focal_length_ = value;
  } else if (name == "fnumber") {
    f_number_ = value;
  } else if (name == "focusplane") {
    focus_plane_ = value;
  } else if (name == "specular") {
    specular_ = value;
  }
}
