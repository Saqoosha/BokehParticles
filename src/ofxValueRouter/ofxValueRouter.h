//
//  ofxValueRouter.h
//  Particles3
//
//  Created by Saqoosha on 12/05/11.
//  Copyright (c) 2012 Saqoosha. All rights reserved.
//

#pragma once

#include "ofMain.h"
#include "ofxOsc.h"
#include "ofxBonjour.h"


class ofxValueSource;
class ofxValueTarget;


//--------------------------------------------------------------------------------
class ofxValueTargetInfo {
 public:
  ofxValueTargetInfo(ofxValueTarget *target, string address, float low, float high)
      : target_(target),
        address_(address),
        low_(low),
        high_(high) {
  };
  
  inline float getScaledValue(float value) {
    return low_ + (high_ - low_) * value;
  };
  
  inline ofxValueTarget *get_target() { return target_; };
  inline string get_address() { return address_; };

 protected:
  ofxValueTarget *target_;
  string address_;
  float low_;
  float high_;
};


//--------------------------------------------------------------------------------
typedef multimap<string, ofxValueTargetInfo *> route_map;
typedef pair<string, ofxValueTargetInfo *> route_pair;
typedef pair<route_map::iterator, route_map::iterator> route_map_pair;

class ofxValueRouter {
 public:
  ofxValueRouter() {};
  
  void addRoute(string source_address, ofxValueTarget *target, string target_address = "", float low = 0.0f, float high = 1.0f);

  void send(string address, float value);
  void send(string address, ofVec2f value);
  
 protected:
  route_map routes_;
};


//--------------------------------------------------------------------------------
class ofxValueSource {
public:
  ofxValueSource() : router_(NULL) {};

  void setup(ofxValueRouter &router) {
    router_ = &router;
  };
  
  virtual inline void send(string address, float value) {
    router_->send(address, value);
  };
  
  virtual inline void send(string address, ofVec2f value) {
    router_->send(address, value);
  }
  
 protected:
  ofxValueRouter *router_;
};


//--------------------------------------------------------------------------------
class ofxValueTarget {
public:
  ofxValueTarget() {};
  virtual void receive(string address, float value) {
    ofLog(OF_LOG_VERBOSE, "ofxValueTarget: %s = %f", address.c_str(), value);
  };
};


//--------------------------------------------------------------------------------
class ofxOscValueSource : public ofxValueSource {
 public:
  ofxOscValueSource() {};

  void setup(ofxValueRouter &router, int port = 8100, string name = "ofxOscValueSource") {
    ofxValueSource::setup(router);
    osc_receiver_.setup(port);
    bonjour_.publish("_osc._udp", port, name);
    ofAddListener(ofEvents.update, this, &ofxOscValueSource::onUpdate);
  }
  
  void onUpdate(ofEventArgs &data) {
    while (osc_receiver_.hasWaitingMessages()) {
      ofxOscMessage message;
      osc_receiver_.getNextMessage(&message);
      float value = message.getNumArgs() > 0 ? message.getArgAsFloat(0) : 1;
      send(message.getAddress(), value);
    }
  }
  
 protected:
  ofxOscReceiver osc_receiver_;
  ofxBonjour bonjour_;
};


//--------------------------------------------------------------------------------
class ofxOscValueTarget : public ofxValueTarget {
 public:
  ofxOscValueTarget() : ofxValueTarget() {};
  
  void setup(string hostname, int port) {
    osc_sender_.setup(hostname, port);
  };
  
  void receive(string address, float value) {
    ofxOscMessage message;
    message.setAddress(address);
    message.addFloatArg(value);
    osc_sender_.sendMessage(message);
  };

 protected:
  ofxOscSender osc_sender_;
//  ofxBonjour bonjour_;
};



