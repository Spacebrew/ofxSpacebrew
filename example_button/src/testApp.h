#pragma once

#include "ofMain.h"

#include "ofxSpacebrew.h"

class testApp : public ofBaseApp{

	public:
		void setup();
		void update();
		void draw();

		void mousePressed(int x, int y, int button);
		void mouseReleased(int x, int y, int button);
    
        // create your spacebrew object
        Spacebrew::Connection spacebrew;
    
        // listen to spacebrew Messages
        void onMessage( Spacebrew::Message & m );
    
        // button stuff
        int  radius;
        bool bButtonPressed;
        bool bBackgroundOn;
    
        // useful quick test
        bool checkInsideCircle( ofPoint point, ofPoint position, int radius );
};
