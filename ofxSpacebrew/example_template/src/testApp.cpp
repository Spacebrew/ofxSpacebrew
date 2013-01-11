#include "testApp.h"

ofColor color;

//--------------------------------------------------------------
void testApp::setup(){
    string host = "ec2-184-72-140-184.compute-1.amazonaws.com"; // change to ec2-184-72-140-184.compute-1.amazonaws.com to test Spacebrew Cloud server
    string name = "of-example";
    string description = "It's amazing";
    spacebrew.addSubscribe("backgroundOn", "boolean");
    spacebrew.addPublish("of-mouse", "boolean", "false");
    spacebrew.connect( host, name, description );
    ofAddListener(spacebrew.onMessageEvent, this, &testApp::onMessage);
}

//--------------------------------------------------------------
void testApp::update(){

}

//--------------------------------------------------------------
void testApp::draw(){
    ofBackground(color);

}

//--------------------------------------------------------------
void testApp::onMessage( Spacebrew::Message & msg ){
    if ( msg.name == "backgroundOn"){
        if ( msg.value == "true"){
            color.set(255);
        } else {
            color.set(0);
        }
    }
}

//--------------------------------------------------------------
void testApp::keyPressed(int key){}

//--------------------------------------------------------------
void testApp::keyReleased(int key){}

//--------------------------------------------------------------
void testApp::mouseMoved(int x, int y){}

//--------------------------------------------------------------
void testApp::mouseDragged(int x, int y, int button){}

//--------------------------------------------------------------
void testApp::mousePressed(int x, int y, int button){
    spacebrew.send("of-mouse", "boolean", "true");
}

//--------------------------------------------------------------
void testApp::mouseReleased(int x, int y, int button){
    spacebrew.send("of-mouse", "boolean", "false");
}

//--------------------------------------------------------------
void testApp::windowResized(int w, int h){}

//--------------------------------------------------------------
void testApp::gotMessage(ofMessage msg){}

//--------------------------------------------------------------
void testApp::dragEvent(ofDragInfo dragInfo){ }