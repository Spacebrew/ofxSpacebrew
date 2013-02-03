#pragma once

#include "ofMain.h"

#import "ofxSpacebrew.h"

class testApp : public ofBaseApp{

	public:
		void setup();
		void update();
		void draw();
    
        // create your spacebrew object
        Spacebrew::Connection spacebrew;
};
