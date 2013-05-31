//
//  DrawableRoute.h
//  example_admin
//
//  Created by BRenfer on 5/31/13.
//
//

#pragma once
#include "ofMain.h"
#include "ofxSpacebrew.h"

//-------------------------------------------------------
class DrawableRoute : public ofRectangle {
public:
    
    DrawableRoute(){
        bMouseOver = false;
    }
    
    DrawableRoute( Spacebrew::RouteEndpoint r ){
        bMouseOver = false;
        calculateDimensions(r);
    }
    
    Spacebrew::RouteEndpoint route;
    
    void calculateDimensions( Spacebrew::RouteEndpoint r ){
        route = r;
        // from ofDrawBitmapStringHighlight
        vector<string> lines = ofSplitString( route.name +":"+ route.type , "\n");
        int textLength = 0;
        for(unsigned int i = 0; i < lines.size(); i++) {
            // tabs are not rendered
            int tabs = count(lines[i].begin(), lines[i].end(), '\t');
            int curLength = lines[i].length() - tabs;
            if(curLength > textLength) {
                textLength = curLength;
            }
        }
        int padding = 4;
        int fontSize = 8;
        float leading = 1.7;
        height = lines.size() * fontSize * leading - 1;
        width = textLength * fontSize;
    }
    
    void draw(){
        ofDrawBitmapStringHighlight(route.name +":"+ route.type, x, y, bMouseOver ? ofColor(255,0,0) : ofColor(150), ofColor(255));
    }
    
    void mouseMoved( int x, int y ){
        if ( hitTest(x+10, y+10)){
            bMouseOver = true;
        } else {
            bMouseOver = false;
        }
    }
    
    bool mousePressed( int x, int y ){
        if ( hitTest(x+10, y+10)){
            bMouseOver = true;
        } else {
            bMouseOver = false;
        }
        return bMouseOver;
    }
    
    void mouseReleased( int x, int y ){
        bMouseOver = false;
    }
    
protected:
    
    bool bMouseOver;
    
    bool hitTest( int mx, int my ){
        if ( mx >= x && mx <= x + width && my >= y && my <= y + height ){
            return true;
        }
        return false;
    }
};

//-------------------------------------------------------
class DrawableClient : public ofRectangle {
public:
    
    DrawableClient( Spacebrew::Config & config )
    {
        // copy from temp
        client = config;
        setup();
    }
    
    void setup(){
        // get anything it publishes
        for (int i=0; i<client.getPublish().size(); i++){
            // routeendpoint is a simple container that describes something you can connect to
            Spacebrew::RouteEndpoint pub;
            pub.name            = client.getPublish()[i].name;
            pub.clientName      = client.clientName;
            pub.remoteAddress   = client.remoteAddress;
            pub.type            = client.getPublish()[i].type;
            
            currentPublishers.push_back( DrawableRoute(pub) );
            currentPublishers.back().x = 20;
        }
        
        // get anything it can subscribe to
        for (int i=0; i<client.getSubscribe().size(); i++){
            // routeendpoint is a simple container that describes something you can connect to
            Spacebrew::RouteEndpoint sub;
            sub.name            = client.getSubscribe()[i].name;
            sub.clientName      = client.clientName;
            sub.remoteAddress   = client.remoteAddress;
            sub.type            = client.getSubscribe()[i].type;
            
            currentSubscribers.push_back( DrawableRoute(sub) );
            currentSubscribers.back().x = 100;
        }
    }
    
    void draw( int _x, int _y ){
        x = _x;
        y = _y;
        
        int dY = y;
        
        ofDrawBitmapStringHighlight(client.clientName, x, dY, ofColor(255,255,0), ofColor(0) );
        
        dY += 25;
        
        for ( int i=0; i<currentPublishers.size(); i++){
            currentPublishers[i].x = x;
            currentPublishers[i].y = dY;
            currentPublishers[i].draw();
            
            dY += currentPublishers[i].height + 10;
        }
        
        height = dY;
        dY = y + 25;
        
        for ( int i=0; i<currentSubscribers.size(); i++){
            currentSubscribers[i].x = x + 500;
            currentSubscribers[i].y = dY;
            currentSubscribers[i].draw();
            dY += currentSubscribers[i].height + 10;
        }
        
        if ( dY > height ){
            height = dY;
        }
    }
    
    void mouseMoved( int mx, int my ){
        for ( int i=0; i<currentPublishers.size(); i++){
            currentPublishers[i].mouseMoved(mx, my);
        }
        for ( int i=0; i<currentSubscribers.size(); i++){
            currentSubscribers[i].mouseMoved(mx, my);
        }
    }
    
    bool mousePressed( int mx, int my ){
        for ( int i=0; i<currentPublishers.size(); i++){
            bool bPressed = currentPublishers[i].mousePressed(mx, my);
            if (bPressed){
                return true;
            }
        }
        for ( int i=0; i<currentSubscribers.size(); i++){
            bool bPressed = currentSubscribers[i].mousePressed(mx, my);
            if (bPressed){
                return true;
            }
        }
        return false;
    }
    
    void mouseReleased( int mx, int my ){
        for ( int i=0; i<currentPublishers.size(); i++){
            currentPublishers[i].mouseReleased(mx, my);
        }
        for ( int i=0; i<currentSubscribers.size(); i++){
            currentSubscribers[i].mouseReleased(mx, my);
        }
    }
    
    Spacebrew::Config client;
    vector<DrawableRoute> currentPublishers;
    vector<DrawableRoute> currentSubscribers;
    
protected:
    
};
