#include "testApp.h"

//--------------------------------------------------------------
void testApp::setup(){
    ofBackground(0);
    spacebrewAdmin.addPublish("Mouse X", Spacebrew::TYPE_RANGE );
    spacebrewAdmin.addPublish("Mouse Y", Spacebrew::TYPE_RANGE );
    spacebrewAdmin.connect(Spacebrew::SPACEBREW_CLOUD, "OF Admin app");
    Spacebrew::addListener(this, spacebrewAdmin);
    Spacebrew::addAdminListener(this, spacebrewAdmin);
}

//--------------------------------------------------------------
void testApp::update(){
}

//--------------------------------------------------------------
void testApp::draw(){
    int x = 20;
    int y = 20;
    subscriberIt it = currentSubscribers.begin();
    
    ofDrawBitmapStringHighlight("Click on a route to connect to it!", x, y, ofColor(255,0,255), ofColor(0) );
    y += 25;
    
    for ( it; it != currentSubscribers.end(); it++){
        ofSetColor(255,0,0);
        ofDrawBitmapStringHighlight(it->first, x, y, ofColor(255,255,0), ofColor(0) );
        y += 25;
        
        for ( int i=0; i< it->second.size(); i++){
            ofSetColor( 255 );
            it->second[i].x = x;
            it->second[i].y = y;
            it->second[i].draw();
            y += it->second[i].height + 10;
        }
        y += 10;
    }
}

//--------------------------------------------------------------
void testApp::keyPressed(int key){}

//--------------------------------------------------------------
void testApp::keyReleased(int key){}

//--------------------------------------------------------------
void testApp::mouseMoved(int x, int y ){
    
    subscriberIt it = currentSubscribers.begin();
    for ( it; it != currentSubscribers.end(); it++){
        for ( int i=0; i< it->second.size(); i++){
            it->second[i].mouseMoved(x,y);
        }
    }
}

//--------------------------------------------------------------
void testApp::mouseDragged(int x, int y, int button){}

//--------------------------------------------------------------
void testApp::mousePressed(int x, int y, int button){
    subscriberIt it = currentSubscribers.begin();
    for ( it; it != currentSubscribers.end(); it++){
        for ( int i=0; i< it->second.size(); i++){
            bool bClicked = it->second[i].mousePressed(x,y);
            if ( bClicked ){
                Spacebrew::RouteEndpoint pub;
                pub.clientName = spacebrewAdmin.getConfig()->clientName;
                pub.remoteAddress = spacebrewAdmin.getConfig()->remoteAddress;
                pub.type = Spacebrew::TYPE_RANGE;
                pub.name = "Mouse X";
                
                spacebrewAdmin.addRoute( pub, it->second[i].route );
            }
        }
    }
}

//--------------------------------------------------------------
void testApp::mouseReleased(int x, int y, int button){
    subscriberIt it = currentSubscribers.begin();
    for ( it; it != currentSubscribers.end(); it++){
        for ( int i=0; i< it->second.size(); i++){
            it->second[i].mouseReleased(x,y);
        }
    }
}

//--------------------------------------------------------------
void testApp::windowResized(int w, int h){}

//--------------------------------------------------------------
void testApp::gotMessage(ofMessage msg){}

//--------------------------------------------------------------
void testApp::dragEvent(ofDragInfo dragInfo){ }

// spacebrew admin events
//--------------------------------------------------------------
void testApp::onClientConnect( Spacebrew::Config & e ){
    cout << "got a new client "<<e.clientName <<":"<<e.remoteAddress <<endl;

    // get anything it can subscribe to
    for (int i=0; i<e.getSubscribe().size(); i++){
        // routeendpoint is a simple container that describes something you can connect to
        Spacebrew::RouteEndpoint sub;
        sub.name            = e.getSubscribe()[i].name;
        sub.clientName      = e.clientName;
        sub.remoteAddress   = e.remoteAddress;
        sub.type            = e.getSubscribe()[i].type;
        
        currentSubscribers[ sub.type ].push_back( DrawableRoute(sub) );
    }
}

//--------------------------------------------------------------
void testApp::onClientUpdated( Spacebrew::Config & e ){
    cout << "client "<<e.clientName <<":"<<e.remoteAddress <<" updated"<<endl;
    
    // update any current subscribers
    
    for (int i=0; i<e.getSubscribe().size(); i++){
        Spacebrew::RouteEndpoint sub;
        sub.name            = e.getSubscribe()[i].name;
        sub.clientName      = e.clientName;
        sub.remoteAddress   = e.remoteAddress;
        sub.type            = e.getSubscribe()[i].type;
        
        bool bFound = false;
        
        for ( int j=0; j<currentSubscribers[ sub.type ].size(); j++){
            if ( currentSubscribers[ sub.type ][j].route == sub){
                bFound = true;
            }
        }
        
        // new subscriber! add it to the stack
        if ( !bFound ){
            currentSubscribers[ sub.type ].push_back( DrawableRoute(sub) );
        }
    }
}

//--------------------------------------------------------------
void testApp::onClientDisconnect( Spacebrew::Config & e ){
    cout << "client "<<e.clientName <<":"<<e.remoteAddress <<" disconnected"<<endl;
    // update any current subscribers
    
    for (int i=0; i<e.getSubscribe().size(); i++){
        Spacebrew::RouteEndpoint sub;
        sub.name            = e.getSubscribe()[i].name;
        sub.clientName      = e.clientName;
        sub.remoteAddress   = e.remoteAddress;
        sub.type            = e.getSubscribe()[i].type;
        
        for ( int j=currentSubscribers[ sub.type ].size()-1; j>=0; j--){
            if ( currentSubscribers[ sub.type ][j].route == sub){
                currentSubscribers[ sub.type ].erase( currentSubscribers[ sub.type ].begin() + j);
            }
        }
    }
}

//--------------------------------------------------------------
void testApp::onRouteAdded( Spacebrew::Route & e ){
    cout << "got a new route "<<e.getPublisher().name << " to " << e.getSubscriber().name << endl;
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