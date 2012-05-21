//
//  ofxBonjour.mm
//  Particles3
//
//  Created by Saqoosha on 12/05/10.
//  Copyright (c) 2012 Saqoosha. All rights reserved.
//

#include "ofxBonjour.h"


@interface ofxBonjourDelegate : NSObject {
  ofxBonjour *parent_;
}
- (id)initWithParent:(ofxBonjour *)parent;
@end


@implementation ofxBonjourDelegate
- (id)initWithParent:(ofxBonjour *)parent {
  self = [super init];
  if (self) {
    parent_ = parent;
  }
  return self;
}
- (void)netServiceBrowserWillSearch:(NSNetServiceBrowser *)netServiceBrowser {
  NSLog(@"netServiceBrowserWillSearch: %@", netServiceBrowser);
}
- (void)netServiceBrowser:(NSNetServiceBrowser *)netServiceBrowser didFindDomain:(NSString *)domainName moreComing:(BOOL)moreDomainsComing {
  NSLog(@"didFindDomain: %@, %d", domainName, (int)moreDomainsComing);
}
- (void)netServiceBrowser:(NSNetServiceBrowser *)netServiceBrowser didFindService:(NSNetService *)netService moreComing:(BOOL)moreServicesComing {
  NSLog(@"didFindService: %@, %d, %@", netService, (int)moreServicesComing, [netService domain]);
}
- (void)netServiceBrowser:(NSNetServiceBrowser *)netServiceBrowser didNotSearch:(NSDictionary *)errorInfo {
  NSLog(@"didNotSearch: %@", errorInfo);
}
@end



ofxBonjour::ofxBonjour() : service_(nil) {
}


ofxBonjour::~ofxBonjour() {
  [(NSNetService *)service_ release];
  service_ = nil;
}


void ofxBonjour::publish(string domain, string protocol, int port, string name) {
  NSAutoreleasePool *pool = [NSAutoreleasePool new];
  
  NSString *_domain = [NSString stringWithCString:domain.c_str() encoding:NSUTF8StringEncoding];
  NSString *_protocol = [NSString stringWithCString:protocol.c_str() encoding:NSUTF8StringEncoding];
  NSString *_name = [NSString stringWithCString:name.c_str() encoding:NSUTF8StringEncoding];
  
  service_ = [[NSNetService alloc] initWithDomain:_domain type:_protocol name:_name port:port];
  //    [self.netService scheduleInRunLoop:[NSRunLoop currentRunLoop] forMode:NSRunLoopCommonModes];
  [(NSNetService *)service_ publish];
  //    [self.netService setDelegate:self];
  
  [pool release];
}


//void ofxBonjour::browse(string service_type, string domain_name) {
//  NSAutoreleasePool *pool = [NSAutoreleasePool new];
//  
//  browser_ = [[NSNetServiceBrowser alloc] init];
//  [browser_ scheduleInRunLoop:[NSRunLoop currentRunLoop] forMode:NSRunLoopCommonModes];
//  delegate_ = [[ofxBonjourDelegate alloc] initWithParent:this];
//  [browser_ setDelegate:(id)delegate_];
//  NSString *type = [NSString stringWithCString:service_type.c_str() encoding:NSUTF8StringEncoding];
//  NSString *domain = [NSString stringWithCString:domain_name.c_str() encoding:NSUTF8StringEncoding];
//  [browser_ searchForServicesOfType:type inDomain:domain];
//  
//  [pool release];
//}

