#include "testApp.h"
#include "ofxDisplayManager.h"
#include "pystring.h"


#define PARAM_CHROMA_ABERR (0)


//--------------------------------------------------------------------------------
void testApp::setup() {
//  ofSetLogLevel(OF_LOG_VERBOSE);
  
  ofSetEscapeQuitsApp(false);

  ofBackground(0);
  ofSetFrameRate(60);
  ofSetVerticalSync(true);
  
  // set fullscreen on second display (if available).
  ofxDisplayList displays = ofxDisplayManager::get()->getDisplays();
  if (displays.size() > 1) {
    ofSetWindowPosition(displays[1]->x, displays[1]->y);
    ofSetFullscreen(true);
  }
  
  SPK::System::setClampStep(true, 0.1f);
  SPK::System::useAdaptiveStep(0.001f, 0.01f);
  
  background_.setup("/Users/hiko/Dropbox/Dev/openFrameworks/of_preRelease_v007_osx/apps/REPUBLIC9/#movie/46823154.3gp");
  background_.setPosition(ofRandom(1.f));
  
  bokeh_textures_.push_back(loadTexture("textures/bokeh1b.png"));
  bokeh_textures_.push_back(loadTexture("textures/bokeh2b.png"));
  bokeh_textures_.push_back(loadTexture("textures/bokeh3b.png"));
  bokeh_textures_.push_back(loadTexture("textures/bokeh4b.png"));
  bokeh_textures_.push_back(loadTexture("textures/bokeh5.png"));
  bokeh_textures_.push_back(loadTexture("textures/bokeh6.png"));
  bokeh_textures_.push_back(loadTexture("textures/bokeh7.png"));
  
  scenes_.push_back(new Scene1());
  scenes_.push_back(new Scene2());
  scenes_.push_back(new Scene3());
  scenes_.push_back(new Scene4());
  scenes_.push_back(new Scene5());
  for (SceneArray::iterator it = scenes_.begin(); it != scenes_.end(); it++) {
    (*it)->setup(bokeh_textures_[6]->getTextureReference().texData.textureID);
  }
  current_scene_index_ = 0;
  current_scene_ = scenes_[current_scene_index_];
  
  bloom_.setup(1024, 768);
  final_compo_.setup();
  
  for (int i = 0; i < 10; i++) {
    audio_map_[i] = 1;
    midi_map_[i] = 1;
  }
    
  // setup gui
  gui_ = new ofxOscUICanvas();
  gui_->setup("11.0.0.2", 9000, 8100);
//  gui_->setup("localhost", 8000, 8100);
//  gui_->setFont("GUI/AxisCondStd-Bold.otf");
  gui_->addWidgetDown(new ofxUILabel("PARTICLES", OFX_UI_FONT_LARGE));
  gui_->addSlider("Focal length", 20, 300, 50, "/camera/focallength/base");
  gui_->addSlider("F-number", 0.1, 20, 1.4, "/camera/fnumber/base");
  gui_->addSlider("Focus plane", 1, 5000, 2000, "/camera/focusplane/base");
  gui_->addSlider("Specular strength", 0, 100, 0, "/camera/specular/base");
  gui_->addSlider("Bloom threshold", 0.0, 1.0, 0.3, "/bloom/threshold/base");
  gui_->addSlider("Bloom strength", 0, 1, 0.2, "/bloom/strength/base");
  gui_->addSlider("Chromatic aberration", 0.01, 0.5, 0.1, "/compo/chromaticaberration/base");
  gui_->addSlider("Chromatic aberration variation", 0, 1, 0, "/compo/chromaticaberration/variation");
  gui_->addSlider("Vignette radius", 0, 1000, 512, "/compo/vignette/radius/base");
  gui_->addSlider("Vignette power", 0, 20, 3, "/compo/vignette/power/base");
  gui_->addSlider("Exposure", 0, 50, 2.0, "/compo/exposure/base");
  gui_->addSlider("Exposure variation", 0, 1, 0, "/compo/exposure/variation");
  gui_->addSlider("Noise", 0, 1, 0.1, "/compo/noise/base");
  gui_->addSlider("Noise variation", 0, 1, 0, "/compo/noise/variation");
  gui_->addSlider("Kaleidoscope", 0, 32, 0, "/compo/kaleidoscope");
  gui_->addSlider("Background alpha", 0, .5f, 0, "/background/alpha/base");
  gui_->addSlider("Background alpha variation", 0, .2f, 0, "/background/alpha/variation");
  gui_->addSlider("Overall alpha", 0, 255.f, 0, "/compo/overallalpha");
  gui_->addSlider("Audio input gain", 0, 50, 0, "/audio/gain");
  gui_->autoSizeToFitWidgets();
  
  save_mode_ = false;
  preset_manager_.setup(gui_);
  preset_manager_.loadSettings("presets.xml");
  
  ofAddListener(gui_->messageReceived, this, &testApp::gotMessage);
  gui_->loadSettings("settings.xml");
//  gui_->send("/compo/overallalpha", 0);
  
  spectrum_analyzer_.setup();

  gui_->setVisible(false);
  ofHideCursor();
  
#define km(c, addr) key_address_map_.insert(make_pair(c, addr))
  km('1', "/scene/1");
  km('2', "/scene/2");
  km('3', "/scene/3");
  km('4', "/scene/4");
  km('5', "/scene/5");
  km('\'', "/action/1");
  km(',', "/action/2");
  km('.', "/action/3");
  km('p', "/action/4");
  km('y', "/action/5");
//  km('f', "/action/6");
//  km('g', "/action/7");
//  km('c', "/action/8");
}


//--------------------------------------------------------------------------------
void testApp::update() {
  preset_manager_.update();
  
  bloom_.set_strength(bloom_strength_.value(getVolume(audio_map_[0])));
  final_compo_.set_chromatic_aberration_strength(chromatic_aberration_.value(getVolume(audio_map_[1])));
  final_compo_.set_vignette_power(vignette_power_.value(getVolume(audio_map_[2])));
  final_compo_.set_exposure(exposure_.value(getVolume(audio_map_[3])));
  final_compo_.set_noise_strength(noise_.value(getVolume(audio_map_[4])));
  current_scene_->audioIn(0, getVolume(audio_map_[5]));
  
//  for (int i = 0; i < 3; i++) {
//    current_scene_->audioIn(i, getVolume(audio_map_[i]));
//  }
  
  current_scene_->update();
  
  if (ofGetFrameNum() % 2 == 0) {
    for (int i = 1; i <= 15; i++) {
      stringstream address;
      address << "/level/audio/" << i;
      gui_->send(address.str(), getVolume(i + 1));
    }
  }
  gui_->update();
}


//--------------------------------------------------------------------------------
void testApp::draw() {
  ofEnablePointSprites();
  ofEnableAlphaBlending();
  
  bloom_.begin();
  {
    ofClear(0);
    
    ofSetColor(ofClamp(background_alpha_.value(), 0, 1) * 255.f);
    background_.draw(0, 768, 1024, -768);
    
    ofSetColor(255);
    current_scene_->draw();
  }
  bloom_.end();
  
  // final composite (chromatic aberration, vignette, exposure, gamma correction)
  final_compo_.begin();
  {
    ofPushMatrix();
    {
      ofScale(1, -1);
      ofSetColor(overall_alpha_);
      bloom_.draw(0, -768);
    }
    ofPopMatrix();
  }
  final_compo_.end();
  
  
  if (gui_->isEnabled()) {
    ofDrawBitmapString(ofToString(ofGetFrameRate(), 1), 10, 768 - 20);
    
    int w = 300;
    int x = 1024 - w - 10;
    int y = 10;
    
    ofSetColor(255);
    ofNoFill();
    spectrum_analyzer_.drawTimeDomain(x, y, w, 128);
    ofRect(x, y, w, 128);
    y += 128 + 10;
    ofSetColor(100);
    spectrum_analyzer_.drawBandVolumes(x, y, w, 128);
    ofSetColor(255);
    spectrum_analyzer_.drawFrequencyDomain(x, y, w, 128);
    ofRect(x, y, w, 128);
    y += 128 + 10;
    ofFill();
    spectrum_analyzer_.drawSpectrogram(x, y, w, 256);
    ofNoFill();
    ofRect(x, y, w, 256);
  }
  
  syphon_server_.publishScreen();
}


//--------------------------------------------------------------------------------
void testApp::exit() {
  preset_manager_.saveSettings("presets.xml");
  gui_->saveSettings("settings.xml");
  delete gui_;
}


//--------------------------------------------------------------------------------
void testApp::keyPressed(int key) {
  switch (key) {
    case 'f': {
      ofToggleFullscreen();
      break;
    }
    case 'g': {
      gui_->toggleVisible();
//      if (gui_->isEnabled()) {
//        ofShowCursor();
//      } else {
//        ofHideCursor();
//      }
      break;
    }
    case ';': {
      current_scene_->textureChange(bokeh_textures_[0]->getTextureReference().texData.textureID);
      break;
    }
    case 'q': {
      current_scene_->textureChange(bokeh_textures_[1]->getTextureReference().texData.textureID);
      break;
    }
    case 'j': {
      current_scene_->textureChange(bokeh_textures_[2]->getTextureReference().texData.textureID);
      break;
    }
    case 'k': {
      current_scene_->textureChange(bokeh_textures_[3]->getTextureReference().texData.textureID);
      break;
    }
    case 'x': {
      current_scene_->textureChange(bokeh_textures_[4]->getTextureReference().texData.textureID);
      break;
    }
    case 'b': {
      current_scene_->textureChange(bokeh_textures_[5]->getTextureReference().texData.textureID);
      break;
    }
    case 'm': {
      current_scene_->textureChange(bokeh_textures_[6]->getTextureReference().texData.textureID);
      break;
    }
    default: {
      map<int,string>::iterator it = key_address_map_.find(key);
      if (it != key_address_map_.end()) {
        ofLog(OF_LOG_ERROR, "map found: %s", it->second.c_str());
        gui_->send(it->second, 1.f);
      } else {
        ofLog(OF_LOG_ERROR, "no key map!");
      }
    }
  }
}


//--------------------------------------------------------------------------------
void testApp::keyReleased(int key) {
  switch (key) {
    default: {
      map<int,string>::iterator it = key_address_map_.find(key);
      if (it != key_address_map_.end()) {
        gui_->send(it->second, 0.f);
      }
    }
  }
}


//--------------------------------------------------------------------------------
void testApp::gotMessage(ofxOscEventArgs &event) {
//  ofLog(OF_LOG_ERROR, "%d: %s: %f", ofGetElapsedTimeMillis(), event.full_address().c_str(), event.value());
  
  if (event.address(0) == "midi") {
    if (event.address(1) == "note") {
      static int pitch = 0;
      static float velocity = 0.f;
      switch (str2num<int>(event.address(3))) {
        case 0: {
          pitch = event.value() * 127.f;
          break; 
        }
        case 1: {
          velocity = event.value();
          break;
        }
        case 2: { // trigger
          int channel = str2num<int>(event.address(2));
          if (event.value() > 0.f) {
            for (int i = 0; i < 5; i++) {
              if (midi_map_[i] == channel) {
                current_scene_->noteOn(i, pitch, velocity);
              }
            }
          }
          stringstream address;
          address << "/level/midi/" << channel;
          gui_->send(address.str(), velocity);
          break;
        }
      }
    }
  } else if (event.address(0) == "mapper") {
    if (event.value() > 0.f) {
      int out_channel = str2num<int>(event.address(2)) - 1;
      int in_channel = str2num<int>(event.address(3));
      if (event.address(1) == "midi") {
        midi_map_[out_channel] = in_channel;
      } else if (event.address(1) == "audio") {
        audio_map_[out_channel] = in_channel;
      }
    }

  } else if (event.address(0) == "camera") {
    current_scene_->gotMessage(event);
  
  } else if (event.address(0) == "bloom") {
    if (event.address(1) == "threshold") {
      if (event.address(2) == "base") {
        bloom_.set_threshold(event.value());
      }
    } else if (event.address(1) == "strength") {
      if (event.address(2) == "base") {
        bloom_strength_.set_base(event.value());
      } else if (event.address(2) == "variation") {
        bloom_strength_.set_variation(event.value());
      }
    }
    
  } else if (event.address(0) == "compo") {
    if (event.address(1) == "vignette") {
      if (event.address(2) == "radius") {
        if (event.address(3) == "base") {
          final_compo_.set_vignette_radius(event.value());
        }
      } else if (event.address(2) == "power") {
        if (event.address(3) == "base") {
          vignette_power_.set_base(event.value());
        } else if (event.address(3) == "variation") {
          vignette_power_.set_variation(event.value());
        }
      }
    } else if (event.address(1) == "chromaticaberration") {
      if (event.address(2) == "base") {
        chromatic_aberration_.set_base(event.value());
      } else if (event.address(2) == "variation") {
        chromatic_aberration_.set_variation(event.value());
      }

    } else if (event.address(1) == "exposure") {
      if (event.address(2) == "base") {
        exposure_.set_base(event.value());
      } else if (event.address(2) == "variation") {
        exposure_.set_variation(event.value());
      }

    } else if (event.address(1) == "noise") {
      if (event.address(2) == "base") {
        noise_.set_base(event.value());
      } else if (event.address(2) == "variation") {
        noise_.set_variation(event.value());
      }
    } else if (event.address(1) == "kaleidoscope") {
      final_compo_.set_kaleidoscope_segments(roundf(event.value()));
      
    } else if (event.address(1) == "overallalpha") {
      overall_alpha_ = event.value();
    }

  } else if (event.address(0) == "background") {
    if (event.address(1) == "alpha") {
      if (event.address(2) == "base") {
        background_alpha_.set_base(event.value());
      } else if (event.address(2) == "variation") {
        background_alpha_.set_variation(event.value());
      }
    }
    
  } else if (event.address(0) == "scene") {
    if (event.value() > 0.f) {
      int index = ofClamp(str2num<int>(event.address(1)) - 1, 0, scenes_.size() - 1);
      if (index != current_scene_index_) {
        stringstream save_key;
        save_key << "_scene" << current_scene_index_;
        preset_manager_.save(save_key.str());
        current_scene_index_ = index;
        current_scene_ = scenes_[current_scene_index_];
        stringstream restore_key;
        restore_key << "_scene" << current_scene_index_;
        preset_manager_.restore(restore_key.str());
      }
      background_.randomPosition(true);
    }
    
  } else if (event.address(0) == "preset") {
    if (event.address(1) == "save") {
      save_mode_ = event.value() > 0.f;
      
    } else if (event.address(1) == "select") {
      if (event.value() > 0.f) {
        if (save_mode_) {
          preset_manager_.save(event.full_address());
          gui_->send("/preset/save", 0);
          save_mode_ = false;
        } else {
          preset_manager_.restore(event.full_address());
        }
      }
    }
    
  } else if (event.address(0) == "audio") {
    if (event.address(1) == "gain") {
      spectrum_analyzer_.set_gain(event.value());
    }
    
  } else if (event.address(0) == "action") {
    int kind = str2num<int>(event.address(2)) - 1;
    current_scene_->action(kind, event.value() > 0.f);
  }
}



