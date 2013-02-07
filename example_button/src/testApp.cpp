#include "testApp.h"

//--------------------------------------------------------------
void testApp::setup(){
    ofSetFrameRate(60);
    string host = Spacebrew::SPACEBREW_CLOUD; // "localhost";
    string name = "of-button-example";
    string description = "It's amazing";
    
    spacebrew.addPublish("button", Spacebrew::TYPE_BOOLEAN);
    spacebrew.addSubscribe("backgroundOn", Spacebrew::TYPE_BOOLEAN); //"boolean" ); // just typing "boolean" also works
    spacebrew.connect( host, name, description );
    
    // listen to spacebrew events
    Spacebrew::addListener(this, spacebrew);
    
    // circle stuff
    bButtonPressed  = false;
    radius          = 200;
    
    // background
    bBackgroundOn   = false;
    
    // layout stuff
    ofBackground(0);
    ofSetRectMode(OF_RECTMODE_CENTER);
    ofEnableSmoothing();
    ofSetCircleResolution(100);
}

//--------------------------------------------------------------
void testApp::update(){}

//--------------------------------------------------------------
void testApp::draw(){
    if ( !bBackgroundOn ){
        ofBackgroundGradient(ofColor(0,0,0), ofColor(50,50,50));
    } else {
        ofBackgroundGradient(ofColor(100,0,0), ofColor(150,150,0));
    }
    
    string textToDraw = "PRESS ME";
    if ( bButtonPressed ){
        ofSetColor( 150, 0, 0 );
        textToDraw = "THANKS";
    } else {
        ofSetColor(150);
    }
    ofCircle(ofGetWidth() / 2.0f, ofGetHeight()/2.0f, radius);
    ofSetColor(255);
    ofDrawBitmapString(textToDraw, ofGetWidth() / 2.0f - 30, ofGetHeight()/2.0f);
}

//--------------------------------------------------------------
void testApp::onMessage( Spacebrew::Message & m ){
    if ( m.name == "backgroundOn" ){
        bBackgroundOn = m.valueBoolean();
    }
}

//--------------------------------------------------------------
void testApp::mousePressed(int x, int y, int button){
    if ( checkInsideCircle( ofPoint(x,y), ofPoint(ofGetWidth() / 2.0f, ofGetHeight()/2.0f), radius) ){
        bButtonPressed = true;
        spacebrew.sendBoolean("button", true);
    }
}

//--------------------------------------------------------------
void testApp::mouseReleased(int x, int y, int button){
    if (bButtonPressed){
        spacebrew.sendBoolean("button", false);
    }
    bButtonPressed = false;
}

//--------------------------------------------------------------
bool testApp::checkInsideCircle( ofPoint point, ofPoint position, int radius ){
    return ( point.x < position.x + radius
            && point.x > position.x - radius
            && point.y < position.y + radius
            && point.y > position.y - radius );
}