#include "testApp.h"

//--------------------------------------------------------------
void testApp::setup(){
    ofSetFrameRate(60);
    string host = Spacebrew::SPACEBREW_CLOUD; // "localhost";
    string name = "of-noise-example";
    string description = "Super simple ofNoise sender";
    
    spacebrew.addPublish("noiseFast", "range");
    spacebrew.addPublish("noiseSlow", "range");
    spacebrew.connect( host, name, description );
}

//--------------------------------------------------------------
void testApp::update(){
    if ( spacebrew.isConnected() ){
        float noiseFast = ofNoise(ofGetElapsedTimeMillis() * .1) * 1024.0f;
        float noiseSlow = ofNoise(ofGetElapsedTimef() * .1) * 1024.0f;
        spacebrew.send("noiseFast", (int) (noiseFast) );
        spacebrew.send("noiseSlow", (int) (noiseSlow) );
    }
}

//--------------------------------------------------------------
void testApp::draw(){
}