#include "ofApp.h"

//--------------------------------------------------------------
void ofApp::setup(){
    ofBackground(0);
    spacebrewAdmin.addPublish("Mouse X", Spacebrew::TYPE_RANGE );
    spacebrewAdmin.addPublish("Mouse Y", Spacebrew::TYPE_RANGE );
    spacebrewAdmin.connect(Spacebrew::SPACEBREW_CLOUD, "OF Admin app");
    Spacebrew::addListener(this, spacebrewAdmin);
    Spacebrew::addAdminListener(this, spacebrewAdmin);
    
    // set up our quick router: listen to mouse events, listen to spacebrew events
    router.setup( spacebrewAdmin );
}

//--------------------------------------------------------------
void ofApp::update(){
}

//--------------------------------------------------------------
void ofApp::draw(){
    router.draw();
}

// spacebrew admin events
//--------------------------------------------------------------
void ofApp::onClientConnect( Spacebrew::Config & e ){
    cout << "got a new client "<<e.clientName <<":"<<e.remoteAddress <<endl;
}

//--------------------------------------------------------------
void ofApp::onClientUpdated( Spacebrew::Config & e ){
    cout << "client "<<e.clientName <<":"<<e.remoteAddress <<" updated"<<endl;
}

//--------------------------------------------------------------
void ofApp::onClientDisconnect( Spacebrew::Config & e ){
    cout << "client "<<e.clientName <<":"<<e.remoteAddress <<" disconnected"<<endl;
}

//--------------------------------------------------------------
void ofApp::onRouteAdded( Spacebrew::Route & e ){
    cout << "got a new route "<<e.getPublisher().name << " to " << e.getSubscriber().name << endl;
    cout << "got a new route "<<e.getPublisher().clientName << " to " << e.getSubscriber().clientName << endl;
}

//--------------------------------------------------------------
void ofApp::onRouteRemoved( Spacebrew::Route & e ){
    cout << "removed route "<<e.getPublisher().name << " to " << e.getSubscriber().name << endl;
}

//--------------------------------------------------------------
void ofApp::onDataPublished( Spacebrew::DataMessage & e ){
    
}

//--------------------------------------------------------------
void ofApp::onMessage( Spacebrew::Message & m ){
    
}


//--------------------------------------------------------------
void ofApp::keyPressed(int key){}

//--------------------------------------------------------------
void ofApp::keyReleased(int key){}

//--------------------------------------------------------------
void ofApp::mouseMoved(int x, int y ){}

//--------------------------------------------------------------
void ofApp::mouseDragged(int x, int y, int button){}

//--------------------------------------------------------------
void ofApp::mousePressed(int x, int y, int button){}

//--------------------------------------------------------------
void ofApp::mouseReleased(int x, int y, int button){}

//--------------------------------------------------------------
void ofApp::windowResized(int w, int h){}

//--------------------------------------------------------------
void ofApp::gotMessage(ofMessage msg){}

//--------------------------------------------------------------
void ofApp::dragEvent(ofDragInfo dragInfo){ }