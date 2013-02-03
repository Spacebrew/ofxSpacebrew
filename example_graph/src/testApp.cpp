#include "testApp.h"

//--------------------------------------------------------------
void testApp::setup(){
    ofSetFrameRate(60);
    ofBackground(0);
    
    string host = "sandbox.spacebrew.cc"; // change to localhost to test Spacebrew local server
    string name = "of-example";
    string description = "It's amazing";
    spacebrew.addSubscribe("line1", "range" );
    spacebrew.addSubscribe("line2", "range" );
    spacebrew.addSubscribe("line3", "range" );
    spacebrew.addSubscribe("box1", "range" );
    spacebrew.addSubscribe("box2", "range" );
    spacebrew.addSubscribe("box3", "range" );
    spacebrew.addSubscribe("lightX", "range" );
    spacebrew.addSubscribe("lightY", "range" );
    
    spacebrew.connect( host, name, description );
    
    Spacebrew::addListener(this, spacebrew);
    
    light.enable();
    light.setSpotlight();
    lightPos.set(ofGetWidth()/2.0f, ofGetHeight()/2.0f, 500);
    light.setPosition(lightPos);
    ofEnableLighting();
    ofSetLineWidth(2.0f);
    
	// this uses depth information for occlusion
	// rather than always drawing things on top of each other
	glEnable(GL_DEPTH_TEST);
}

//--------------------------------------------------------------
void testApp::update(){
    light.setPosition(lightPos);
}

//--------------------------------------------------------------
void testApp::draw(){
    ofDisableLighting();
    // draw lines
    ofSetColor(150,0,0);
    line1.draw();
    
    ofSetColor(0,0,150);
    line2.draw();
    
    ofSetColor(150,150,0);
    line3.draw();
    
    ofEnableLighting();
    // draw shapes
    ofSetColor(0,150,0);
    ofBox(ofGetWidth() - 200, 100, radius1);
    ofSetColor(255,150,0);
    
    ofPushMatrix();
        ofTranslate(ofGetWidth() - 200, 320);
        ofRotateX(ofGetElapsedTimef());
        ofRotateY(ofGetElapsedTimef() * 2.0f);
        ofBox(0,0, radius2 );
    ofPopMatrix();
    ofSetColor(150,0,255);
    ofBox(ofGetWidth() - 200, 640, radius3 );
}


void testApp::mouseMoved( int x, int y ){
    light.setPosition(mouseX, mouseY, 500);
}

//--------------------------------------------------------------
void testApp::addRange( ofPolyline & line, int value ){
    cout << "add "<< value << endl;
    line.addVertex( line.size() * 5, value );
    if ( line.size() > 100 ){
        vector<ofPoint> & verts = line.getVertices();
        // shift points over
        verts.erase(line.getVertices().begin());
        for ( int i=0; i<line.size(); i++){
            verts[i].x -= 10;
        }
    }
}

//--------------------------------------------------------------
void testApp::onMessage( Spacebrew::Message & msg ){
    
    // why all the / 1024.0 ? because spacebrew ranges are 0-1024!
    
    if ( msg.name == "line1"){
        addRange(line1, ofToInt(msg.value) / 1024.0 * 500);
    } else if ( msg.name == "line2"){
        addRange(line2, ofToInt(msg.value) / 1024.0 * 500);
    } else if ( msg.name == "line3"){
        addRange(line3, ofToInt(msg.value) / 1024.0 * 500);
    } else if ( msg.name == "box1"){
        radius1 = ofToFloat(msg.value) / 1024.0 * 200.0f;
    } else if ( msg.name == "box2"){
        radius2 = ofToFloat(msg.value) / 1024.0 * 200.0f;
    } else if ( msg.name == "box3"){
        radius3 = ofToFloat(msg.value) / 1024.0 * 200.0f;
    } else if ( msg.name == "lightX" ){
        lightPos.x = ofToInt(msg.value) / 1024.0 * ofGetWidth()/2.0f;
    } else if ( msg.name == "lightY" ){
        lightPos.y = ofToInt(msg.value) / 1024.0 * ofGetHeight()/2.0f;
    } else {
        cout <<"Unkown Message: "<< msg << endl;
    }
}