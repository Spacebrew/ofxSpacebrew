#pragma once

#include "ofMain.h"
#include "ofxSpacebrew.h"
#include "ofxSpacebrewPanel.hpp"


class ofApp : public ofBaseApp{

	public:
		void setup();
		void update();
		void draw();

		void keyPressed(int key);
		void keyReleased(int key);
		void mouseMoved(int x, int y );
		void mouseDragged(int x, int y, int button);
		void mousePressed(int x, int y, int button);
		void mouseReleased(int x, int y, int button);
		void mouseEntered(int x, int y);
		void mouseExited(int x, int y);
		void windowResized(int w, int h);
		void dragEvent(ofDragInfo dragInfo);
		void gotMessage(ofMessage msg);
		
    // create your spacebrew object
    Spacebrew::Connection spacebrew;
    
    //The Spacebrew UI
    //Make sure you have the correct fonts in
    //your bin folder!
    ofxSpacebrewPanel brewUI;
    
    
    // listen to spacebrew Messages
    void onMessage( Spacebrew::Message & m );
    
    //vectors to store points and colors
    vector<ofVec2f> points;
    vector<int> hues;
    
    //make sure we store the current point
    //when we get an X and Y pair
    int numXYs;
    ofVec2f currentPoint;
    
    //Color
    int hue;
    
};
