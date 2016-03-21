//
//  ofxSpacebrewPanelElement.hpp
//  Drawing Tool
//
//  Created by Adiel Fernandez on 2/20/16.
//
//

#ifndef SpacebrewElement_hpp
#define SpacebrewElement_hpp

#include <stdio.h>

#endif /* SpacebrewElement_hpp */

#include "ofMain.h"
#include "ofxSpacebrew.h"
#pragma once



class ofxSpacebrewPanelElement{
    
public:
    
    ofxSpacebrewPanelElement(string _type, string _name, bool _isPublisher, ofTrueTypeFont *_font, bool placeHolder = false);
    
    void getDimensions(float elemH, float nameW, float typeW, float outRad, float inRad);
    
    void update();
    void draw();
    
    //Formatting
    float nameWidth, typeWidth;
    float elementHeight, elementWidth;
    float outerCircleRad, innerCircleRad;
    
    bool isPublisher;
    bool isPlaceHolder;
    bool eventTriggered, triggerColors;
    int triggerTime, showTriggeredDelay;
    
    
    string type, name;
    ofColor typeCol, nameCol;
    ofColor innerCircleCol, outerCircleCol;
    
    ofTrueTypeFont *font;
    
};