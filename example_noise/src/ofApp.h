#pragma once

#include "ofMain.h"

#import "ofxSpacebrew.h"

class ofApp : public ofBaseApp{

	public:
		void setup();
		void update();
		void draw();
    
        // create your spacebrew object
        Spacebrew::Connection spacebrew;
};
