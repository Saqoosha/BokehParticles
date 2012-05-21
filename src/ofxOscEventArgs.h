//
//  ofxOscEventArgs.h
//  Particles4
//
//  Created by Saqoosha on 12/05/18.
//  Copyright (c) 2012 Saqoosha. All rights reserved.
//

#pragma once

#include "pystring.h"

class ofxUIWidget;

class ofxOscEventArgs : public ofEventArgs {
 public:
  ofxOscEventArgs(string full_address, float value, ofxUIWidget *widget = NULL)
    : full_address_(full_address),
      value_(value),
      widget_(widget) {
    pystring::split(full_address_, address_, "/", 10);
    address_.erase(address_.begin());
  }
  
  inline ofxUIWidget *widget() { return widget_; }
  inline void set_widget(ofxUIWidget *widget) { widget_ = widget; }
  inline string &full_address() { return full_address_; }
  inline string &address(int index) { return address_[index]; }
  inline float value() { return value_; }
  inline void set_value(float value) { value_ = value; }
  
 protected:
  ofxUIWidget *widget_;
  vector<string> address_;
  string full_address_;
  float value_;
};
