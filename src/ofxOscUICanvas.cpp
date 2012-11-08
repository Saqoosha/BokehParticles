//
//  ofxOscUICanvas.cpp
//  OSCGUI
//
//  Created by Saqoosha on 12/05/16.
//  Copyright (c) 2012 Saqoosha. All rights reserved.
//

#include "ofxOscUICanvas.h"


//------------------------------------------------------------------------------
void ofxOscUICanvas::setup(string send_host, int send_port, int receive_port) {
  ofAddListener(newGUIEvent, this, &ofxOscUICanvas::guiEvent);
  send_host_ = send_host;
  send_port_ = send_port;
  receive_port_ = receive_port;
  osc_sender_.setup(send_host_, send_port_);
  osc_receiver_.setup(receive_port_);
  bonjour_.publish("_osc._udp", receive_port, "ofxOscUICanvas");
//  ofAddListener(ofEvents.update, this, &ofxOscUICanvas::update);
}


//------------------------------------------------------------------------------
ofxUIButton *ofxOscUICanvas::addButton(string name, string address) {
  ofxUIButton *button = new ofxUIButton(20, 20, false, name);
  addWidgetDown(button);
  setAddress(button, address);
  return button;
}


//------------------------------------------------------------------------------
ofxUIToggle *ofxOscUICanvas::addToggle(string name, string address) {
  ofxUIToggle *toggle = new ofxUIToggle(20, 20, false, name);
  addWidgetDown(toggle);
  setAddress(toggle, address);
  return toggle;
}


//------------------------------------------------------------------------------
ofxUISlider *ofxOscUICanvas::addSlider(string name, float width, float height, float min, float max, float value, string address) {
  ofxUISlider *slider = new ofxUISlider(width, height, min, max, value, name);
  addWidgetDown(slider);
  setAddress(slider, address);
  return slider;
}


//------------------------------------------------------------------------------
void ofxOscUICanvas::setAddress(ofxUIWidget *widget, string address) {
  AddressInfo *info = new AddressInfo(widget, address);
  address_map_.insert(make_pair(widget->getName(), info));
  address_map_.insert(make_pair(address, info));
}


//------------------------------------------------------------------------------
float ofxOscUICanvas::getValue(string address, bool scaled) {
  AddressInfo *info = getAddressInfo(address);
  if (info != NULL) {
    switch (info->widget()->getKind()) {
      case OFX_UI_WIDGET_BUTTON:
      case OFX_UI_WIDGET_TOGGLE:{
        return reinterpret_cast<ofxUIButton *>(info->widget())->getValue() ? 1.f : 0.f;
      }
      case OFX_UI_WIDGET_SLIDER_H:
      case OFX_UI_WIDGET_SLIDER_V: {
        ofxUISlider *slider = reinterpret_cast<ofxUISlider *>(info->widget());
        return scaled ? slider->getScaledValue() : slider->getValue();
      }
    }
  }
  return NAN;
}


//------------------------------------------------------------------------------
void ofxOscUICanvas::guiEvent(ofxUIEventArgs &event) {
  AddressInfo *info = getAddressInfo(event.widget->getName());
  if (info == NULL) return;

  float value = getValue(event.widget->getName(), false);
  if (value != value) {
    return;
  }
  ofxOscMessage message;
  message.setAddress(info->address());
  message.addFloatArg(value);
  osc_sender_.sendMessage(message);

  ofxOscEventArgs e(info->address(), getValue(event.widget->getName(), true), info->widget());
  ofNotifyEvent(messageReceived, e);
}


//------------------------------------------------------------------------------
void ofxOscUICanvas::update() {
  while (osc_receiver_.hasWaitingMessages()) {
    ofxOscMessage message;
    osc_receiver_.getNextMessage(&message);
//    ofLog(OF_LOG_ERROR, "%s", message.getAddress().c_str());
    
    float value = 0.f;
    switch (message.getArgType(0)) {
      case OFXOSC_TYPE_FLOAT: {
        value = message.getArgAsFloat(0);
        break;
      }
      case OFXOSC_TYPE_INT32: {
        value = message.getArgAsInt32(0);
        break;
      }
      default:;
    }
    ofxOscEventArgs e(message.getAddress(), value);

    AddressInfo *info = getAddressInfo(message.getAddress());
    if (info != NULL) {
      switch (info->widget()->getKind()) {
        case OFX_UI_WIDGET_BUTTON:
        case OFX_UI_WIDGET_TOGGLE: {
          reinterpret_cast<ofxUIButton *>(info->widget())->setValue(value > 0.f);
          break;
        }
        case OFX_UI_WIDGET_SLIDER_H:
        case OFX_UI_WIDGET_SLIDER_V: {
          if (message.getNumArgs() > 0) {
            ofxUISlider *slider = reinterpret_cast<ofxUISlider *>(info->widget());
            ofVec2f maxmin = slider->getMaxAndMind();
            value = ofMap(value, 0, 1, maxmin.y, maxmin.x, true);
            e.set_value(value);
            slider->setValue(value);
          }
          break;
        }
      }
      e.set_widget(info->widget());
    }

    ofNotifyEvent(messageReceived, e);
  }
}


//------------------------------------------------------------------------------
void ofxOscUICanvas::send(string address, float value) {
  ofxOscEventArgs e(address, value, NULL);
  float normalized_value = value;
  AddressInfo *info = getAddressInfo(address);
  if (info != NULL) {
    e.set_widget(info->widget());
    switch (info->widget()->getKind()) {
      case OFX_UI_WIDGET_BUTTON:
      case OFX_UI_WIDGET_TOGGLE: {
        reinterpret_cast<ofxUIButton *>(info->widget())->setValue(value > 0.f);
        break;
      }
      case OFX_UI_WIDGET_SLIDER_H:
      case OFX_UI_WIDGET_SLIDER_V: {
        ofxUISlider *slider = reinterpret_cast<ofxUISlider *>(info->widget());
        slider->setValue(value);
        ofVec2f maxmin = slider->getMaxAndMind();
        normalized_value = ofMap(value, maxmin.y, maxmin.x, 0, 1, true);
        break;
      }
    }
  }
  ofNotifyEvent(messageReceived, e);

  ofxOscMessage message;
  message.setAddress(address);
  message.addFloatArg(normalized_value);
  osc_sender_.sendMessage(message);
}


