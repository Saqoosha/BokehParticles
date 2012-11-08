//
//  ofxOscUICanvas.h
//  OSCGUI
//
//  Created by Saqoosha on 12/05/16.
//  Copyright (c) 2012 Saqoosha. All rights reserved.
//

#pragma once

#include "ofxUI.h"
#include "ofxOsc.h"
#include "ofxBonjour.h"
#include "pystring.h"

#include "ofxOscEventArgs.h"



class ofxOscUICanvas : public ofxUICanvas {
 public:
  typedef void (*CallbackFunction)(string name, string address, float value);
  
 private:
  class AddressInfo {
   public:
    AddressInfo(ofxUIWidget *widget, string address, CallbackFunction callback = NULL)
      : widget_(widget),
        address_(address),
        callback_(callback) {}
    ofxUIWidget *widget() { return widget_; }
    string name() { return widget_->getName(); }
    string address() { return address_; }
    CallbackFunction callback() { return callback_; }
   private:
    ofxUIWidget *widget_;
    string address_;
    CallbackFunction callback_;
  };
  
  typedef map<string, AddressInfo *> AddressMap;
  
 public:
  ofxOscUICanvas() : ofxUICanvas() {}
  
  void setup(string send_host, int send_port, int receive_port);

  ofxUIButton *addButton(string name, string address);
  ofxUIToggle *addToggle(string name, string address);
  ofxUISlider *addSlider(string name, float min, float max, float value, string address) {
    addSlider(name, 300, 10, min, max, value, address);
  }
  ofxUISlider *addSlider(string name, float width, float height, float min, float max, float value, string address);

  void setAddress(ofxUIWidget *widget, string address);
  ofxUIWidget *getWidget(string address) {
    AddressInfo *info = getAddressInfo(address);
    return info ? info->widget() : NULL;
  }
  string getAddress(ofxUIWidget *widget) {
    AddressInfo *info = getAddressInfo(widget->getName());
    return info ? info->address() : "";
  }
  float getValue(string address, bool scaled = true);

  void guiEvent(ofxUIEventArgs &event);
//  void update(ofEventArgs &event);
  void update();
  void send(string address, float value);
  
  ofEvent<ofxOscEventArgs> messageReceived;
  
 private:
  inline AddressInfo *getAddressInfo(string address) {
    AddressMap::iterator it = address_map_.find(address);
    return it == address_map_.end() ? NULL : it->second;
  }
  
  string send_host_;
  int send_port_;
  int receive_port_;
  ofxOscSender osc_sender_;
  ofxOscReceiver osc_receiver_;
  ofxBonjour bonjour_;
  AddressMap address_map_;
};
