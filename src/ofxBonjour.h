//
//  ofxBonjour.h
//  Particles3
//
//  Created by Saqoosha on 12/05/10.
//  Copyright (c) 2012 Saqoosha. All rights reserved.
//

#pragma once

#include <string>

using namespace std;

class ofxBonjour {
 public:
  ofxBonjour();
  ~ofxBonjour();
  
  void publish(string protocol, int port, string name = "ofxBonjour") {
    publish("local", protocol, port, name);
  }
  void publish(string domain, string protocol, int port, string name);
//  void browse(string service_type, string domain_name = "");
  
 private:
  void *service_;
  void *browser_;
  void *delegate_;
};
