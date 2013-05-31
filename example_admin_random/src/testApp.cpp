#include "testApp.h"

//--------------------------------------------------------------
void testApp::setup(){
    spacebrew.addPublish("Mouse X", Spacebrew::TYPE_RANGE);
    spacebrew.addPublish("Mouse Pressed", Spacebrew::TYPE_BOOLEAN);
    spacebrew.addPublish("Key Released", Spacebrew::TYPE_STRING);
    spacebrew.connect(Spacebrew::SPACEBREW_CLOUD, "OF Admin Randomizer");
    Spacebrew::addListener(this, spacebrew);
    Spacebrew::addAdminListener(this, spacebrew);
    
    bConnected = false;
    
    ofSetFrameRate(60);
    
    ofEnableSmoothing();
    ofEnableAlphaBlending();
    ofBackground(0);
    
    fbo.allocate(ofGetWidth(), ofGetHeight());
    fbo.begin();
    ofSetColor(0);
    ofRect(0,0,ofGetWidth(), ofGetHeight());
    fbo.end();
}

//--------------------------------------------------------------
void testApp::update(){

}

//--------------------------------------------------------------
void testApp::draw(){
    fbo.begin();
    ofEnableAlphaBlending();
    ofSetColor(0,5);
    ofRect(0,0,ofGetWidth(), ofGetHeight());
    ofSetColor(ofRandom(255), ofRandom(255), ofRandom(255), 255);
    ofCircle(mouseX, mouseY, ofRandom(3));
    fbo.end();
    
    ofSetColor(255);
    fbo.draw(0, 0);
    
    ofDrawBitmapString("Press 'r' to connect to a random route!", 20, ofGetHeight()-20);
    if ( bConnected ){
        ofDrawBitmapString( "Routed to", 20, 20 );
        ofDrawBitmapStringHighlight( "CLIENT: "+route.getSubscriber().clientName +"\nINPUT:  "+ route.getSubscriber().name,
                                    20, 40, ofColor(150,50,75), ofColor(255) );
    }
}

//--------------------------------------------------------------
void testApp::onMessage( Spacebrew::Message & m ){
    
}

//--------------------------------------------------------------
void testApp::onRouteAdded( Spacebrew::Route & e ){
    if ( e.getPublisher().clientName == spacebrew.getConfig()->clientName ){
        route = e;
        bConnected = true;
    }
}

//--------------------------------------------------------------
void testApp::onRouteRemoved( Spacebrew::Route & e ){
    if ( e.getPublisher().clientName == spacebrew.getConfig()->clientName ){
        bConnected = false;
    }
}

//--------------------------------------------------------------
void testApp::keyPressed(int key){
    if ( key == 'r' ){
        if ( bConnected ){
            spacebrew.removeRoute(route);
        }
        // pick a random connection
        vector<Spacebrew::Config> connections = spacebrew.getConnectedClients();
        if ( connections.size() == 1 ){
            ofLogWarning("Only one connection, can't route anywhere!");
            return;
        }
        Spacebrew::Config c = connections[ floor(ofRandom(connections.size())) ];
        
        while ( c.clientName == spacebrew.getConfig()->clientName || c.getSubscribe().size() == 0){
            c = connections[ floor(ofRandom(connections.size())) ];
        }
        Spacebrew::Message m = c.getSubscribe()[ floor(ofRandom( c.getSubscribe().size() ))];
        
        Spacebrew::RouteEndpoint sub;
        sub.clientName = c.clientName;
        sub.name = m.name;
        sub.remoteAddress = c.remoteAddress;
        sub.type = m.type;
        
        Spacebrew::RouteEndpoint pub;
        pub.clientName = spacebrew.getConfig()->clientName;
        pub.remoteAddress = spacebrew.getConfig()->remoteAddress;
        
        cout << sub.clientName << ":" << sub.type << ":::"<< pub.clientName<<endl;
        
        if ( sub.type == Spacebrew::TYPE_BOOLEAN ){
            pub.name = "Mouse Pressed";
            pub.type = Spacebrew::TYPE_BOOLEAN;
        } else if ( sub.type == Spacebrew::TYPE_RANGE ){
            pub.name = "Mouse X";
            pub.type = Spacebrew::TYPE_RANGE;
        } else if (sub.type == Spacebrew::TYPE_STRING ){
            pub.clientName = spacebrew.getConfig()->clientName;
            pub.name = "Key Released";
            pub.type = Spacebrew::TYPE_STRING;
        }
        
        spacebrew.addRoute( pub, sub );
    }
}

//--------------------------------------------------------------
void testApp::keyReleased(int key){
    string str =  "";
    str += key;
    spacebrew.sendString("Key Released", str);
}

//--------------------------------------------------------------
void testApp::mouseMoved(int x, int y ){
    spacebrew.sendRange("Mouse X", ofMap( x, 0, ofGetWidth(), 0, 1023 ));
}

//--------------------------------------------------------------
void testApp::mouseDragged(int x, int y, int button){}

//--------------------------------------------------------------
void testApp::mousePressed(int x, int y, int button){
    spacebrew.sendBoolean("Mouse Pressed", true);
    fbo.begin();
    ofSetColor(255);
    ofRect(0,0,ofGetWidth(), ofGetHeight());
    fbo.end();
}

//--------------------------------------------------------------
void testApp::mouseReleased(int x, int y, int button){
    spacebrew.sendBoolean("Mouse Pressed", false);
}

//--------------------------------------------------------------
void testApp::windowResized(int w, int h){}

//--------------------------------------------------------------
void testApp::gotMessage(ofMessage msg){}

//--------------------------------------------------------------
void testApp::dragEvent(ofDragInfo dragInfo){}