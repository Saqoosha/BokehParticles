#pragma once

#include "ofMain.h"
#include "ofxSyphon.h"

#include "ControlValue.h"
#include "ofxOscUICanvas.h"
#include "PresetManager.h"
#include "ofxSpectrumAnalyzer.h"
#include "BlurredBackground.h"
#include "ofxBloom.h"
#include "ofxKaleidoscope.h"
#include "FinalComp.h"

#include "BaseScene.h"
#include "Scene1.h"
#include "Scene2.h"
#include "Scene3.h"
#include "Scene4.h"
#include "Scene5.h"


//--------------------------------------------------------------------------------
template<typename T>
T str2num(const std::string& text, int base = 10) {
  assert(base == 8 || base == 10 || base == 16);
  std::istringstream is(text);
  T value;
  switch (base) {
    case 8:
      is >> std::oct >> value;
      break;
    case 10:
      is >> value;
      break;
    case 16:
      is >> std::hex >> value;
      break;
  }
  return value;
}


//--------------------------------------------------------------------------------
class testApp : public ofBaseApp {
  typedef vector<BaseScene*> SceneArray;

 public:
  void setup();
  void update();
  void draw();
  void exit();
  
  void keyPressed(int key);
  void keyReleased(int key);
  
  ofxSRGBTexture *loadTexture(string path) {
    ofxSRGBTexture *tex = new ofxSRGBTexture();
    tex->loadImage(path, GL_TEXTURE_2D);
    return tex;
  }
  
  void gotMessage(ofxOscEventArgs &event);
  
  inline float getVolume(int index) {
    float value = 0;
    switch (index) {
      case 1: {
        return 0;
      }
      case 2: {
        return spectrum_analyzer_.smoothed_volume();
      }
      default: {
        return spectrum_analyzer_.smoothed_subband_volume(index - 1);
      }
    }
    return value;
  }
  
 private:
  SceneArray scenes_;
  BaseScene *current_scene_;
  int current_scene_index_;
  
  BlurredBackground background_;
  ofxBloom bloom_;
//  ofxKaleidoscope kaleidoscope_;
  FinalComp final_compo_;
  
  ofxOscUICanvas *gui_;
  ofxSpectrumAnalyzer spectrum_analyzer_;
  
  bool save_mode_;
  PresetManager preset_manager_;

  int audio_map_[10];
  int midi_map_[10];
  
  ControlValue bloom_strength_;
  ControlValue chromatic_aberration_;
  ControlValue vignette_power_;
  ControlValue exposure_;
  ControlValue noise_;
  ControlValue background_alpha_;
  float overall_alpha_;
  
  map<int, string> key_address_map_;
  
  vector<ofxSRGBTexture*> bokeh_textures_;
  
  ofxSyphonServer syphon_server_;
};
