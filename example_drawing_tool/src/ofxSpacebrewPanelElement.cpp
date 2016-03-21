//
//  ofxSpacebrewPanelElement.cpp
//  Drawing Tool
//
//  Created by Adiel Fernandez on 2/20/16.
//
//

#include "ofxSpacebrewPanelElement.hpp"


ofxSpacebrewPanelElement::ofxSpacebrewPanelElement(string _type, string _name, bool _isPublisher, ofTrueTypeFont *_font, bool placeHolder){
    
    
    type = _type;
    name = _name;
    font = _font;
    isPublisher = _isPublisher;
    
    //set colors depending on type
    if(type == "range"){
        typeCol.set(198, 0, 248);
    } else if(type == "string"){
        typeCol.set(255, 0, 0);
    } else if(type == "boolean"){
        typeCol.set(255, 174, 0);
    }
    
    
    
    if(isPublisher){
        nameCol.set(176, 237, 255);
    } else {
        nameCol.set(177, 255, 216);
    }
    

    //capitalize TYPE name
    for(int i = 0; i < type.length(); i++){
        type[i] = toupper(type[i]);
    }
    
    //flag to be flipped by incoming event
    eventTriggered = false;
    
    //use alternate colors when triggered
    triggerColors = false;
    
    //last time triggered
    triggerTime = 0;

    //how long to flash alt colors
    showTriggeredDelay = 200;
    
    innerCircleCol.set(255);
    outerCircleCol.set(0);
    
    

    isPlaceHolder = placeHolder;
    
    //override stuff if we're just a place holder
    if(isPlaceHolder){
        nameCol.set(150);
        typeCol.set(120);
        innerCircleCol.set(200);
        outerCircleCol.set(100);
    }

    
    
}

//get dimensions from the UI instance
void ofxSpacebrewPanelElement::getDimensions(float elemH, float nameW, float typeW, float outRad, float inRad){
    
    elementHeight = elemH;
    nameWidth = nameW;
    typeWidth = typeW;
    outerCircleRad = outRad;
    innerCircleRad = inRad;
    elementWidth = typeWidth + nameWidth + outerCircleRad;
    
}


void ofxSpacebrewPanelElement::update(){
    
    //change circle colors when an event is triggered
    if(!isPlaceHolder){

        if(eventTriggered){
            triggerTime = ofGetElapsedTimeMillis();
            triggerColors = true;
            eventTriggered = false;
        }
        
        
        if(triggerColors && ofGetElapsedTimeMillis() - triggerTime < showTriggeredDelay){
            innerCircleCol.set(0);
            outerCircleCol.set(4, 203, 255);
        } else {
            innerCircleCol.set(255);
            outerCircleCol.set(0);
        }
    
    }
    
    
}

void ofxSpacebrewPanelElement::draw(){
    
    
    ofPoint nameDim(font -> stringWidth(name), font -> stringHeight(name));
    ofPoint typeDim(font -> stringWidth(type), font -> stringHeight(type));

    float nameMargin = 15;
    

    
    ofSetCircleResolution(60);
    
    //Drawing Publisher from left to right is Type, Name then trigger circle
    //need ro reverse if it's a Subscriber
    if(isPublisher){
        
        //type
        ofSetColor(typeCol);
        ofDrawRectangle(0, 0, typeWidth, elementHeight);
        
        ofSetColor(255);
        font -> drawString(type, typeWidth/2 - typeDim.x/2, elementHeight/2 + typeDim.y/2);
        
        //name
        ofSetColor(nameCol);
        ofDrawRectangle(typeWidth, 0, nameWidth, elementHeight);

        if(!isPlaceHolder){
            ofSetColor(0);
        } else {
            ofSetColor(255);
        }
        font -> drawString(name, typeWidth + nameWidth - nameDim.x - nameMargin, elementHeight/2 + nameDim.y/2);
        
            
        //black circle outline
        ofSetColor(outerCircleCol);
        ofDrawCircle(nameWidth + typeWidth, elementHeight/2, outerCircleRad);
        //inner white circle
        ofSetColor(innerCircleCol);
        ofDrawCircle(nameWidth + typeWidth, elementHeight/2, innerCircleRad);
        
    } else {

        //push every thing in a little bit so
        //the circles align with boxes at the left
        ofPushMatrix();
        ofTranslate(outerCircleRad, 0);
        
        //name
        ofSetColor(nameCol);
        ofDrawRectangle(0, 0, nameWidth, elementHeight);
        
        if(!isPlaceHolder){
            ofSetColor(0);
        } else {
            ofSetColor(255);
        }
        font -> drawString(name, nameMargin, elementHeight/2 + nameDim.y/2);
        
        //type
        ofSetColor(typeCol);
        ofDrawRectangle(nameWidth, 0, typeWidth, elementHeight);
        
        ofSetColor(255);
        font -> drawString(type, nameWidth + typeWidth/2 - typeDim.x/2, elementHeight/2 + typeDim.y/2);
        
        //black circle outline
        ofSetColor(outerCircleCol);
        ofDrawCircle(0, elementHeight/2, outerCircleRad);
        //inner white circle
        ofSetColor(innerCircleCol);
        ofDrawCircle(0, elementHeight/2, innerCircleRad);
        
        ofPopMatrix();
    }
    
    
    
}