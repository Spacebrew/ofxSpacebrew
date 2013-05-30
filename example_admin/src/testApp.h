#pragma once

#include "ofMain.h"
#include "ofxSpacebrew.h"

class DrawableRoute : public ofRectangle {
public:
    
    DrawableRoute(){
        bMouseOver = false;
    }
    
    DrawableRoute( Spacebrew::RouteEndpoint r ){
        bMouseOver = false;
        calculateDimensions(r);
    }
    
    Spacebrew::RouteEndpoint route;
    
    void calculateDimensions( Spacebrew::RouteEndpoint r ){
        route = r;
        // from ofDrawBitmapStringHighlight
        vector<string> lines = ofSplitString( route.clientName +":"+ route.name , "\n");
        int textLength = 0;
        for(unsigned int i = 0; i < lines.size(); i++) {
            // tabs are not rendered
            int tabs = count(lines[i].begin(), lines[i].end(), '\t');
            int curLength = lines[i].length() - tabs;
            if(curLength > textLength) {
                textLength = curLength;
            }
        }
        int padding = 4;
        int fontSize = 8;
        float leading = 1.7;
        height = lines.size() * fontSize * leading - 1;
        width = textLength * fontSize;
    }
    
    void draw(){
        ofDrawBitmapStringHighlight(route.clientName +":"+ route.name, x, y, bMouseOver ? ofColor(255,0,0) : ofColor(150), ofColor(255));
    }
    
    void mouseMoved( int x, int y ){
        if ( hitTest(x+10, y+10)){
            bMouseOver = true;
        } else {
            bMouseOver = false;
        }
    }
    
    bool mousePressed( int x, int y ){
        if ( hitTest(x+10, y+10)){
            bMouseOver = true;
        } else {
            bMouseOver = false;
        }
        return bMouseOver;
    }
    
    void mouseReleased( int x, int y ){
        bMouseOver = false;
    }
    
protected:
    
    bool bMouseOver;
    
    bool hitTest( int mx, int my ){
        if ( mx >= x && mx <= x + width && my >= y && my <= y + height ){
            return true;
        }
        return false;
    }

};
class testApp : public ofBaseApp{

	public:
		void setup();
		void update();
		void draw();

		void keyPressed  (int key);
		void keyReleased(int key);
		void mouseMoved(int x, int y );
		void mouseDragged(int x, int y, int button);
		void mousePressed(int x, int y, int button);
		void mouseReleased(int x, int y, int button);
		void windowResized(int w, int h);
		void dragEvent(ofDragInfo dragInfo);
		void gotMessage(ofMessage msg);
		
        Spacebrew::AdminConnection spacebrewAdmin;

        // spacebrew admin events
        void onClientConnect( Spacebrew::Config & e );
        void onClientUpdated( Spacebrew::Config & e );
        void onClientDisconnect( Spacebrew::Config & e );
        void onRouteAdded( Spacebrew::Route & e );
        void onRouteRemoved( Spacebrew::Route & e );
        void onDataPublished( Spacebrew::DataMessage & e );

        // normal spacebrew message event
        void onMessage( Spacebrew::Message & m );
    
        // map of available subscribers by type
        typedef map< string, vector<DrawableRoute> >::iterator subscriberIt;
        map< string, vector<DrawableRoute> > currentSubscribers;
};
