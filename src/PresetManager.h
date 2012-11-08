//
//  PresetManager.h
//  OSCGUI
//
//  Created by Saqoosha on 12/05/16.
//  Copyright (c) 2012 Saqoosha. All rights reserved.
//

#pragma once

#include "ofxOscUICanvas.h"
#include "ofxTween.h"


class PresetManager {
  typedef map<string, float> DataSet;
  typedef map<string, DataSet> Preset;
  
 public:
  PresetManager() : ui_(NULL), tween_(NULL), tweening_data_(NULL) {}
  
  void setup(ofxOscUICanvas *ui) { ui_ = ui; }
  void update();
  
  void saveSettings(string path);
  void loadSettings(string path);
  
  void save(string name);
  void restore(string name);

 private:
  ofxOscUICanvas *ui_;
  Preset presets_;
  ofxTween *tween_;
  DataSet *tweening_data_;
  ofxEasingCubic cubic_;
  ofxEasingExpo expo_;
};
