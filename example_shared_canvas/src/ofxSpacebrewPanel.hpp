//
//  SpacebrewUI.hpp
//
//  Created by Adiel Fernandez on 2/20/16.
//
//

#ifndef SpacebrewUI_hpp
#define SpacebrewUI_hpp

#include <stdio.h>

#endif /* SpacebrewUI */

#include "ofMain.h"
#include "ofxSpacebrew.h"
#include "ofxSpacebrewPanelElement.hpp"

#pragma once


class ofxSpacebrewPanel{
    
public:
    
    ofxSpacebrewPanel();
    
    void setup(Spacebrew::Connection *_spacebrew);
    void update();
    void draw(float _x, float _y);

    void show();
    void hide();
    int getWidth();
    int getHeight();

    


private:
    
    void onSend( string& args );
    void onMessage( Spacebrew::Message& name );
    

    

    Spacebrew::Connection *spacebrew;
    
    //local list of pubs and subs
    vector<ofxSpacebrewPanelElement> publishers;
    vector<ofxSpacebrewPanelElement> subscribers;
    
    bool isVisible;
    bool isOpen;
    
    ofVec2f position;
    ofVec2f closedPos, openPos, animatePos;
    
    
    //Formatting
    ofTrueTypeFont font;
    ofTrueTypeFont titles;
    
    float width, height;
    
    //declare these globally so we can have
    //use global width/height variables
    string pubTitle;
    string subTitle;
    string hostTitle;
    string hostName;
    
    float nameWidth, typeWidth;
    float elementHeight, elementWidth;
    float outerCircleRad, innerCircleRad;
    float elementSpacing;
    float padding;
    float titleSpacing;
    
    
    //use an FBO so we can do the
    //slide out animation
    ofFbo fbo;
    
};



