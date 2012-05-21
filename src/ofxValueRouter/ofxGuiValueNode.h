//
//  ofxGuiValueNode.h
//  Particles3
//
//  Created by Saqoosha on 12/05/12.
//  Copyright (c) 2012 Saqoosha. All rights reserved.
//

#pragma once


typedef map<string, string> StringMap;
typedef pair<string, string> StringPair;

class ofxGuiValueNode : public ofxValueSource, public ofxValueTarget {
public:
  ofxGuiValueNode() : ofxValueSource() {};
  
  void setup(ofxValueRouter &router, ofxUICanvas *gui) {
    ofxValueSource::setup(router);
    gui_ = gui;
    ofAddListener(gui_->newGUIEvent, this, &ofxGuiValueNode::guiEvent);
  };
  
  void addRoute(string widget_name, string source_address) {
    address_map_.insert(StringPair(widget_name, source_address));
    float low = 0;
    float high = 1;
    ofxUIWidget *widget = gui_->getWidget(widget_name);
    switch (widget->getKind()) {
      case OFX_UI_WIDGET_SLIDER_H:
      case OFX_UI_WIDGET_SLIDER_V: {
        ofxUISlider *slider = reinterpret_cast<ofxUISlider *>(widget);
        low = slider->getMin();
        high = slider->getMax();
        break;
      }
    }
    router_->addRoute(source_address, this, source_address, low, high);
    reverse_address_map_.insert(StringPair(source_address, widget_name));
  };
  
  void receive(string address, float value) {
    StringMap::iterator it = reverse_address_map_.find(address);
    if (it != reverse_address_map_.end()) {
      ofxUIWidget *widget = gui_->getWidget(it->second);
      switch (widget->getKind()) {
        case OFX_UI_WIDGET_SLIDER_H:
        case OFX_UI_WIDGET_SLIDER_V:
          reinterpret_cast<ofxUISlider *>(widget)->setValue(value);
          break;
      }
    }
  };
  
  void guiEvent(ofxUIEventArgs &e) {
    StringMap::iterator it = address_map_.find(e.widget->getName());
    if (it != address_map_.end()) {
      float value;
      switch (e.widget->getKind()) {
        case OFX_UI_WIDGET_SLIDER_H:
        case OFX_UI_WIDGET_SLIDER_V:
          value = reinterpret_cast<ofxUISlider *>(e.widget)->getValue();
          send(it->second, value);
          break;
      }
    } else {
      ofLog(OF_LOG_VERBOSE, "ofxGuiValueNode: no address map. %s", e.widget->getName().c_str());
    }
  };
  
private:
  ofxUICanvas *gui_;
  StringMap address_map_;
  StringMap reverse_address_map_;
};


