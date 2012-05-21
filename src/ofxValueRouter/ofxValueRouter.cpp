//
//  ofxValueRouter.cpp
//  Particles3
//
//  Created by Saqoosha on 12/05/11.
//  Copyright (c) 2012 Saqoosha. All rights reserved.
//

#include "ofxValueRouter.h"


void ofxValueRouter::addRoute(string source_address, ofxValueTarget *target, string target_address, float low, float high) {
  if (target_address.empty()) {
    target_address = source_address;
  }
  routes_.insert(route_pair(source_address, new ofxValueTargetInfo(target, target_address, low, high)));
};


void ofxValueRouter::send(string address, float value) {
  route_map_pair p = routes_.equal_range(address);
  if (p.first == p.second) {
    ofLog(OF_LOG_VERBOSE, "No receivers! : %s = %f", address.c_str(), value);
  } else {
    for(; p.first != p.second; p.first++) {
      ofxValueTargetInfo *info = p.first->second;
      info->get_target()->receive(info->get_address(), info->getScaledValue(value));
    }
  }
};


void ofxValueRouter::send(string address, ofVec2f value) {
//  route_map_pair p = routes_.equal_range(address);
//  if (p.first == p.second) {
//    ofLog(OF_LOG_VERBOSE, "No receivers! : %s", address.c_str());
//  } else {
//    for(; p.first != p.second; p.first++) {
//      ofxValueTargetInfo *info = p.first->second;
//      info->get_target()->receive(info->get_address(), info->getScaledValue(value));
//    }
//  }
};


