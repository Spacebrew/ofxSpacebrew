//
//  ofxSpacebrewPanel.cpp
//
//  Created by Adiel Fernandez on 2/20/16.
//
//

#include "ofxSpacebrewPanel.hpp"


ofxSpacebrewPanel::ofxSpacebrewPanel(){
    
}

void ofxSpacebrewPanel::setup(Spacebrew::Connection *_spacebrew){
    
    //get local copy of pointer to the connection object
    spacebrew = _spacebrew;

    
    //Load UI fonts
    titles.load("fonts/DINPro-Bold.otf", 16, true);
    font.load("fonts/DINPro-Medium.otf", 10, true);

    
    isVisible = true;
    isOpen = true;
    
    
    //Individual Element formatting
    //Ideally hese should be defined in the ofxSpacebrewPanelElement class
    //but we need them them below to set the width/height values.
    //Make SpacebrewUI namespace then have these globally declared and accessible by this class and the element class via SpacebrewUI::ElementWidth, etc.?
    nameWidth = 220;
    typeWidth = 80;
    outerCircleRad = 7.5;
    innerCircleRad = 4.0;
    elementHeight = 25;
    elementWidth = typeWidth + nameWidth + outerCircleRad;
    
    
    //Get the publishers by going through the connection object THEN
    //through the Config object THEN to the getPublish vector
    auto pubs = spacebrew -> getConfig() -> getPublish();
    
    for(int i = 0; i < pubs.size(); i++){
        
        Spacebrew::Message m = pubs[i];
        
        ofxSpacebrewPanelElement elem(m.type, m.name, true, &font);
        elem.getDimensions(elementHeight, nameWidth, typeWidth, outerCircleRad, innerCircleRad);
        
        publishers.push_back(elem);
        
        
    }
    
    //do the same for the subscribers
    auto subs = spacebrew -> getConfig() -> getSubscribe();

    for(int i = 0; i < subs.size(); i++){
        
        Spacebrew::Message m = subs[i];
        
        ofxSpacebrewPanelElement elem(m.type, m.name, false, &font);
        elem.getDimensions(elementHeight, nameWidth, typeWidth, outerCircleRad, innerCircleRad);
        
        subscribers.push_back(elem);
        
    }
    
    //if there are no subs and/or subs then add a placeholder
    if(publishers.size() == 0){
        
        ofxSpacebrewPanelElement placeholder("N/A", "No Publishers Created", true, &font, true);
        placeholder.getDimensions(elementHeight, nameWidth, typeWidth, outerCircleRad, innerCircleRad);
        publishers.push_back(placeholder);
        
    }
    
    if(subscribers.size() == 0){
        
        ofxSpacebrewPanelElement placeholder("N/A", "No Subscribers Created", false, &font, true);
        placeholder.getDimensions(elementHeight, nameWidth, typeWidth, outerCircleRad, innerCircleRad);
        subscribers.push_back(placeholder);
        
    }
    
    
    //add up all elements, spacing and font heights to get total box height
    int numElements = publishers.size() + subscribers.size();
    
    //General Panel Formatting
    pubTitle = "/ PUBLISHERS";
    subTitle = "/ SUBSCRIBERS";
    hostTitle = "/ HOST";
    hostName = spacebrew -> getHost();
    
    elementSpacing = 5;
    padding = 15;
    titleSpacing = elementSpacing * 3;
    
    width = elementWidth + padding * 2;
    height = padding * 2 + numElements * elementHeight + (numElements - 1) * elementSpacing;
    
    height += titleSpacing + titles.stringHeight(pubTitle);
    height += titleSpacing + titles.stringHeight(subTitle);
    height += titleSpacing + titles.stringHeight(hostTitle);
    height += titleSpacing + titles.stringHeight(hostName);

    
    //initialize Pos
    openPos.set(0, 0);
    closedPos.set(0, -height - 10);
    
    //start as open
    animatePos.set(0, 0);
    
    //need to scale FBO then draw it shrunken down
    //or else the FBO makes everything look pixelated
    fbo.allocate(width * 2, height * 2);
    
    //clear out the FBO
    fbo.begin();
    ofClear(255, 255, 255, 255);
    fbo.end();
    
    
    //add listener to detect messages
    ofAddListener( spacebrew -> onMessageEvent, this, &ofxSpacebrewPanel::onMessage );
    ofAddListener( spacebrew -> onSendEvent, this, &ofxSpacebrewPanel::onSend);
    
}

void ofxSpacebrewPanel::onMessage( Spacebrew::Message& args ){
    
    //go through subscribers and see which one this message belongs to
    for(int i = 0; i < subscribers.size(); i++){
        
        if(subscribers[i].name == args.name){
            subscribers[i].eventTriggered = true;
        }
        
    }
    
}

void ofxSpacebrewPanel::onSend( string& name ){
    
    //go through subscribers and see which one this message belongs to
    for(int i = 0; i < publishers.size(); i++){
        
        if(publishers[i].name == name){
            publishers[i].eventTriggered = true;
        }
        
    }
    
}


void ofxSpacebrewPanel::update(){
    
    //update the publisher & subscriber elements
    for(int i = 0; i < publishers.size(); i++){
        publishers[i].update();
    }
    
    for(int i = 0; i < subscribers.size(); i++){
        subscribers[i].update();
    }
    
    
    
    float animateSpeed = 0.09;
    
    if(isOpen){
    
        //prevents unnecessary calculations as interpolations gets 99.999% there
        if(ofDist(animatePos.x, animatePos.y, openPos.x, openPos.y) > 0.001){
            animatePos = animatePos.getInterpolated(openPos, animateSpeed);
        }
        
    } else {

        //prevents unnecessary calculations as interpolations gets 99.999% there
        if(ofDist(animatePos.x, animatePos.y, closedPos.x, closedPos.y) > 0.001){
            animatePos = animatePos.getInterpolated(closedPos, animateSpeed);
        } else {
            isVisible = false;
        }
        
    }
    
    
}

int ofxSpacebrewPanel::getWidth(){
    return width;
}

int ofxSpacebrewPanel::getHeight(){
    return height;
}
    
void ofxSpacebrewPanel::show(){
    isVisible = true;
    isOpen = true;
}

void ofxSpacebrewPanel::hide(){
    isOpen = false;
}

void ofxSpacebrewPanel::draw(float _x, float _y){

    
    
    if(isVisible){
    
        fbo.begin();
        ofEnableAlphaBlending();
        
        ofClear(0, 0);
        
        ofPushMatrix();
        ofScale(2, 2);
        ofTranslate(animatePos.x, animatePos.y);
        
        //draw white background around everything
        ofSetColor(255, 220);
        ofDrawRectangle(0, 0, width, height);
        
        //host title
        ofSetColor(0);
        titles.drawString(hostTitle, padding, padding + titleSpacing);
        ofTranslate(0, titles.stringHeight(hostTitle) + titleSpacing);
        
        //host name
        ofSetColor(50);
        float centerText = (width - titles.stringWidth(hostName))/2;
        titles.drawString(hostName, centerText, padding + titleSpacing);
        ofTranslate(0, titles.stringHeight(hostName) + titleSpacing);
        
        //publishers title
        ofSetColor(0);
        titles.drawString(pubTitle, padding, padding + titleSpacing);
        ofTranslate(0, titles.stringHeight(pubTitle) + titleSpacing);
        
        //draw publishers
        ofTranslate(padding, elementSpacing);
        for(int i = 0; i < publishers.size(); i++){
            publishers[i].draw();
            ofTranslate(0, publishers[i].elementHeight + elementSpacing);
        }
        
        //add a little extra gap between pubs and subs
        ofTranslate(0, elementSpacing);

        //subs title
        ofSetColor(0);
        titles.drawString(subTitle, 0, 20);
        ofTranslate(0, titles.stringHeight(subTitle) + titleSpacing);
        
        //draw subs
        for(int i = 0; i < subscribers.size(); i++){
            subscribers[i].draw();
            ofTranslate(0, subscribers[i].elementHeight + elementSpacing);
        }
        
        ofPopMatrix();
        
        fbo.end();
        
        ofSetColor(255);        
        fbo.draw(_x, _y, width, height);
        
        
    }
    
    
}



