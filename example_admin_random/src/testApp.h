#pragma once

#include "ofMain.h"
#include "ofxSpacebrew.h"

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
		
        Spacebrew::AdminConnection spacebrew;
        void onMessage( Spacebrew::Message & m );
    
        // default Spacebrew::AdminConnection events
        void onClientConnect( Spacebrew::Config & e ){};
        void onClientUpdated( Spacebrew::Config & e ){};
        void onClientDisconnect( Spacebrew::Config & e ){};
        void onDataPublished( Spacebrew::DataMessage & e ){};
    
        // events we care about
        void onRouteAdded( Spacebrew::Route & e );
        void onRouteRemoved( Spacebrew::Route & e );
    
        bool bConnected;
        Spacebrew::Route route;
    
        // dumb FBO visualizer
        ofFbo fbo;
};
