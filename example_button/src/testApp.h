#pragma once

#include "ofMain.h"

#import "ofxSpacebrew.h"

class testApp : public ofBaseApp{

	public:
		void setup();
		void update();
		void draw();

		void mousePressed(int x, int y, int button);
		void mouseReleased(int x, int y, int button);
    
        // create your spacebrew object
        Spacebrew::Connection spacebrew;
    
        // button stuff
        int  radius;
        bool bButtonPressed;
    
        // useful quick test
        bool checkInsideCircle( ofPoint point, ofPoint position, int radius );
};
