#include "ofApp.h"

//--------------------------------------------------------------
void ofApp::setup(){

    ofSetFrameRate(60);
    
    string host = Spacebrew::SPACEBREW_CLOUD; // "localhost";
    string name = "of-drawing-tool-example";
    string description = "Draw all the things";
    
    spacebrew.addPublish("X", Spacebrew::TYPE_RANGE);
    spacebrew.addPublish("Y", Spacebrew::TYPE_RANGE);
    spacebrew.addPublish("hue", Spacebrew::TYPE_RANGE);
    spacebrew.addPublish("clear", Spacebrew::TYPE_BOOLEAN);

    spacebrew.connect( host, name, description );
    
    Spacebrew::addListener(this, spacebrew);

    
    //set up the gui tool by passing it your spacebrew
    //connection by reference (i.e. with the & symbol)
    brewUI.setup( &spacebrew );
    

    numXYs = 0;
    hue = 0;
    
}

//--------------------------------------------------------------
void ofApp::update(){

    brewUI.update();
    
}

//--------------------------------------------------------------
void ofApp::draw(){
    
    ofBackgroundGradient(100, 0);
    
    ofSetLineWidth(4);
    
    if(points.size()){
        for(int i = 0; i < points.size() - 1; i++){
            
            //need to make sure we have enough colors to
            ofSetColor(ofColor::fromHsb(hues[i], 200, 200));
            ofDrawLine(points[i], points[i + 1]);
            
        }
    }
    
    
    
    //draw UI
    brewUI.draw(ofGetWidth() - brewUI.getWidth() - 10, 10);
    

    
    //Draw app info to screen
    string msg;
    msg += "App for sending drawing data from Spacebrew \"example_shared_canvas\"\n";
    msg += "Don't forget to add routes from drawing tool to shared canvas at \"" + spacebrew.getHost() + "\"!\n";
    msg += "\n";
    msg += "Click and Drag to draw and send X/Y/Color Values\n";
    msg += "Press SPACE to send a message to clear the canvas (and also clear locally)\n";
    msg += "\n";
    msg += "Points sent: " + ofToString(points.size()) + "\n";
    msg += "Press 'h' to hide Spacebrew Admin Info\n";
    msg += "Press 's' to show Spacebrew Admin Info\n";
    
    
    
    ofSetColor(255);
    ofDrawBitmapString(msg, 10, ofGetHeight() - 125);
    
    
}


//--------------------------------------------------------------
void ofApp::onMessage( Spacebrew::Message & msg ){

    

    
}

//--------------------------------------------------------------
void ofApp::keyPressed(int key){

    if(key == 's'){
        brewUI.show();
    }
    
    if(key == 'h'){
        brewUI.hide();
    }
    
    
    if(key == ' '){
        
        //send a boolean message to tell the canvas to clear
        spacebrew.sendBoolean("clear", true);
        
        //erase things locally too
        points.clear();
        hues.clear();
        
    }
    
    
}

//--------------------------------------------------------------
void ofApp::keyReleased(int key){

}

//--------------------------------------------------------------
void ofApp::mouseMoved(int x, int y ){

}

//--------------------------------------------------------------
void ofApp::mouseDragged(int x, int y, int button){
    
    spacebrew.sendRange("X", ofMap(x, 0, ofGetWidth(), 0, 1023));
    spacebrew.sendRange("Y", ofMap(y, 0, ofGetHeight(), 0, 1023));
    spacebrew.sendRange("hue", hue);
    
    points.push_back(ofVec2f(x, y));
    hues.push_back(hue);
    
    hue += 2;
    
    if(hue > 255) hue = 0;
    
}

//--------------------------------------------------------------
void ofApp::mousePressed(int x, int y, int button){
    
    
}

//--------------------------------------------------------------
void ofApp::mouseReleased(int x, int y, int button){

}

//--------------------------------------------------------------
void ofApp::mouseEntered(int x, int y){

}

//--------------------------------------------------------------
void ofApp::mouseExited(int x, int y){

}

//--------------------------------------------------------------
void ofApp::windowResized(int w, int h){

}

//--------------------------------------------------------------
void ofApp::gotMessage(ofMessage msg){

}

//--------------------------------------------------------------
void ofApp::dragEvent(ofDragInfo dragInfo){ 

}
