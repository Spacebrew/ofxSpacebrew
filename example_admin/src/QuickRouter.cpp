//
//  QuickRouter.cpp
//  example_admin
//
//  Created by BRenfer on 5/31/13.
//
//

#include "QuickRouter.h"

//--------------------------------------------------------------
void QuickRouter::setup( Spacebrew::AdminConnection & connection ){
    spacebrewAdmin = &connection;
    
    // listen to mouse events
    ofAddListener(ofEvents().mouseMoved, this, &QuickRouter::mouseMoved);
    ofAddListener(ofEvents().mousePressed, this, &QuickRouter::mousePressed);
    ofAddListener(ofEvents().mouseReleased, this, &QuickRouter::mouseReleased);
    
    // listen to spacebrew admin events
    Spacebrew::addAdminListener(this, *spacebrewAdmin);
}

//--------------------------------------------------------------
void QuickRouter::draw(){
    int x = 20;
    int y = 20;
    
    ofDrawBitmapStringHighlight("Click on a route to connect to it!", x, y, ofColor(255,0,255), ofColor(0) );
    y += 25;
    
    mutex.lock();
    for ( int i=0; i<clients.size(); i++){
        clients[i]->y = y;
        clients[i]->draw(x, y);
        y += clients[i]->height + 10;
    }
    mutex.unlock();
    
    // draw routes
    vector<Spacebrew::Route> routes = spacebrewAdmin->getCurrentRoutes();
    for ( int i=0; i<routes.size(); i++){
        ofPoint from, to;
        for ( int j=0; j<clients.size(); j++){
            for ( int k=0; k<clients[j]->currentPublishers.size(); k++){
                if ( clients[j]->currentPublishers[k].route == routes[i].getPublisher() ){
                    from.set( clients[j]->currentPublishers[k].x + clients[j]->currentPublishers[k].width, clients[j]->currentPublishers[k].y );
                }
            }
            
            for ( int k=0; k<clients[j]->currentSubscribers.size(); k++){
                if ( clients[j]->currentSubscribers[k].route == routes[i].getSubscriber() ){
                    to.set( clients[j]->currentSubscribers[k].x, clients[j]->currentSubscribers[k].y );
                }
            }
        }
        ofLine(from,to);
    }
}

//--------------------------------------------------------------
void QuickRouter::onClientConnect( Spacebrew::Config & e ){
    mutex.lock();
    clients.push_back( new DrawableClient(e) );
    mutex.unlock();
}

//--------------------------------------------------------------
void QuickRouter::onClientUpdated( Spacebrew::Config & e ){
    // find + update
    mutex.lock();
    for (int i=0; i<clients.size(); i++){
        if ( clients[i]->client == e ){
            cout << "erasing "<<e.clientName<<endl;
            clients.erase( clients.begin() + i);
        }
    }
    clients.push_back( new DrawableClient(e) );
    mutex.unlock();
}

//--------------------------------------------------------------
void QuickRouter::onClientDisconnect( Spacebrew::Config & e ){
    // find + remove
    mutex.lock();
    for (int i=0; i<clients.size(); i++){
        if ( clients[i]->client == e ){
            clients.erase( clients.begin() + i);
        }
    }
    mutex.unlock();
}

//--------------------------------------------------------------
void QuickRouter::onRouteAdded( Spacebrew::Route & e ){
}

//--------------------------------------------------------------
void QuickRouter::onRouteRemoved( Spacebrew::Route & e ){
}

//--------------------------------------------------------------
void QuickRouter::onDataPublished( Spacebrew::DataMessage & e ){
    
}

//--------------------------------------------------------------
void QuickRouter::mouseMoved( ofMouseEventArgs & e ){
    int x = e.x;
    int y = e.y;
    
    mutex.lock();
    for (int i=0; i<clients.size(); i++){
        clients[i]->mouseMoved(x, y);
    }
    mutex.unlock();
}

//--------------------------------------------------------------
void QuickRouter::mousePressed( ofMouseEventArgs & e ){
    int x = e.x;
    int y = e.y;
    
    mutex.lock();
    for (int i=0; i<clients.size(); i++){
        clients[i]->mousePressed(x, y);
    }
    mutex.unlock();
}

//--------------------------------------------------------------
void QuickRouter::mouseReleased( ofMouseEventArgs & e ){
    int x = e.x;
    int y = e.y;
    
    mutex.lock();
    for (int i=0; i<clients.size(); i++){
        clients[i]->mouseReleased(x, y);
    }
    mutex.unlock();
}