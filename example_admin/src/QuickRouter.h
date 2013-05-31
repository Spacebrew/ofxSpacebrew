//
//  QuickRouter.h
//  example_admin
//
//  Created by BRenfer on 5/31/13.
//
//

#pragma once

#include "ofMain.h"
#include "ofxSpacebrew.h"
#include "DrawableClient.h"

class QuickRouter {

public:
    
    void setup( Spacebrew::AdminConnection & connection );
    
    void draw();
    
    void onClientConnect( Spacebrew::Config & e );
    void onClientUpdated( Spacebrew::Config & e );
    void onClientDisconnect( Spacebrew::Config & e );
    
    void onRouteAdded( Spacebrew::Route & e );
    void onRouteRemoved( Spacebrew::Route & e );
    void onDataPublished( Spacebrew::DataMessage & e );
    
    void mouseMoved( ofMouseEventArgs & e );
    void mousePressed( ofMouseEventArgs & e );
    void mouseReleased( ofMouseEventArgs & e );
    
protected:
    
    Spacebrew::AdminConnection * spacebrewAdmin;
    
    ofMutex mutex;
    
    vector<DrawableClient *> clients;
    
};