#include "testApp.h"

//--------------------------------------------------------------
void testApp::setup(){
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
void testApp::update(){
}

//--------------------------------------------------------------
void testApp::draw(){
    router.draw();
}

// spacebrew admin events
//--------------------------------------------------------------
void testApp::onClientConnect( Spacebrew::Config & e ){
    cout << "got a new client "<<e.clientName <<":"<<e.remoteAddress <<endl;
}

//--------------------------------------------------------------
void testApp::onClientUpdated( Spacebrew::Config & e ){
    cout << "client "<<e.clientName <<":"<<e.remoteAddress <<" updated"<<endl;
}

//--------------------------------------------------------------
void testApp::onClientDisconnect( Spacebrew::Config & e ){
    cout << "client "<<e.clientName <<":"<<e.remoteAddress <<" disconnected"<<endl;
}

//--------------------------------------------------------------
void testApp::onRouteAdded( Spacebrew::Route & e ){
    cout << "got a new route "<<e.getPublisher().name << " to " << e.getSubscriber().name << endl;
    cout << "got a new route "<<e.getPublisher().clientName << " to " << e.getSubscriber().clientName << endl;
}

//--------------------------------------------------------------
void testApp::onRouteRemoved( Spacebrew::Route & e ){
    cout << "removed route "<<e.getPublisher().name << " to " << e.getSubscriber().name << endl;
}

//--------------------------------------------------------------
void testApp::onDataPublished( Spacebrew::DataMessage & e ){
    
}

//--------------------------------------------------------------
void testApp::onMessage( Spacebrew::Message & m ){
    
}


//--------------------------------------------------------------
void testApp::keyPressed(int key){}

//--------------------------------------------------------------
void testApp::keyReleased(int key){}

//--------------------------------------------------------------
void testApp::mouseMoved(int x, int y ){}

//--------------------------------------------------------------
void testApp::mouseDragged(int x, int y, int button){}

//--------------------------------------------------------------
void testApp::mousePressed(int x, int y, int button){}

//--------------------------------------------------------------
void testApp::mouseReleased(int x, int y, int button){}

//--------------------------------------------------------------
void testApp::windowResized(int w, int h){}

//--------------------------------------------------------------
void testApp::gotMessage(ofMessage msg){}

//--------------------------------------------------------------
void testApp::dragEvent(ofDragInfo dragInfo){ }