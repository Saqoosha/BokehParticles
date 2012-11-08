//
//  PresetManager.cpp
//  OSCGUI
//
//  Created by Saqoosha on 12/05/16.
//  Copyright (c) 2012 Saqoosha. All rights reserved.
//

#include "PresetManager.h"


//------------------------------------------------------------------------------
void PresetManager::saveSettings(string path) {
  ofxXmlSettings *settings = new ofxXmlSettings();
  Preset::iterator preset = presets_.begin();
  while (preset != presets_.end()) {
    int index = settings->addTag("preset");
    settings->addAttribute("preset", "name", preset->first, index);
    settings->pushTag("preset", index);
    {
      DataSet::iterator data = preset->second.begin();
      while (data != preset->second.end()) {
        int index = settings->addTag("data");
        settings->addAttribute("data", "address", data->first, index);
        settings->addAttribute("data", "value", data->second, index);
        data++;
      }
    }
    settings->popTag();
    preset++;
  }
  settings->saveFile(path);
  delete settings;
}


//------------------------------------------------------------------------------
void PresetManager::loadSettings(string path) {
  ofxXmlSettings *settings = new ofxXmlSettings();
  if (settings->loadFile(path)) {
    int count = settings->getNumTags("preset");
    for (int i = 0; i < count; i++) {
      string name = settings->getAttribute("preset", "name", "", i);
      DataSet data;
      settings->pushTag("preset", i);
      {
        int count = settings->getNumTags("data");
        for (int i = 0; i < count; i++) {
          string address = settings->getAttribute("data", "address", "", i);
          float value = settings->getAttribute("data", "value", 0.0, i);
          if (!address.empty()) {
            data.insert(pair<string, float>(address, value));
          }
        }
      }
      settings->popTag();
      if (!name.empty() && data.size() > 0) {
        presets_[name] = data;
      }
    }
  }
  delete settings;
}


//------------------------------------------------------------------------------
void PresetManager::save(string name) {
//  ofLog(OF_LOG_ERROR, "\nsave: %s", name.c_str());
  DataSet data;
  vector<ofxUIWidget *> widgets = ui_->getWidgets();
  for (int i = 0; i < widgets.size(); i++) {
    switch (widgets[i]->getKind()) {
      case OFX_UI_WIDGET_SLIDER_H:
      case OFX_UI_WIDGET_SLIDER_V: {
//        ofLog(OF_LOG_ERROR, "%s: %f", ui_->getAddress(widgets[i]).c_str(), reinterpret_cast<ofxUISlider *>(widgets[i])->getScaledValue());
        data.insert(make_pair(ui_->getAddress(widgets[i]), reinterpret_cast<ofxUISlider *>(widgets[i])->getScaledValue()));
        break;
      }
    }
  }
  presets_[name] = data;
}


//------------------------------------------------------------------------------
void PresetManager::restore(string name) {
//  ofLog(OF_LOG_ERROR, "\nrestore: %s", name.c_str());
  Preset::iterator it = presets_.find(name);
  if (it == presets_.end()) return;
  
  if (tween_ != NULL) {
    delete tween_;
  }
  tween_ = new ofxTween();
  tweening_data_ = &(it->second);
  
  DataSet::iterator data = tweening_data_->begin();
  int pos = 0;
  while (data != tweening_data_->end()) {
    float value = ui_->getValue(data->first);
    if (value != value) {
      ofLog(OF_LOG_ERROR, "failed to get value: %s", data->first.c_str());
      value = 0.f;
    }
//    ofLog(OF_LOG_ERROR, "%s: %f", data->first.c_str(), value);
//    ofLog(OF_LOG_ERROR, "%s: %s, %f, %f, %f", name.c_str(), data->first.c_str(), value, data->second, value - data->second);
    if (pos == 0) {
      tween_->setParameters(expo_, ofxTween::easeOut, value, data->second, 250, 0);
    } else {
      tween_->addValue(value, data->second);
    }
    pos++;
    data++;
  }
}


//------------------------------------------------------------------------------
void PresetManager::update() {
  if (tween_ == NULL) return;
  
  tween_->update();
  
  DataSet::iterator it = tweening_data_->begin();
  int pos = 0;
  while (it != tweening_data_->end()) {
    ui_->send(it->first, tween_->getTarget(pos));
    pos++;
    it++;
  }
  
  if (tween_->isCompleted()) {
    delete tween_;
    tween_ = NULL;
  }
}
