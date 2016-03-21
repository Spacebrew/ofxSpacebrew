#include "ofApp.h"

//--------------------------------------------------------------
void ofApp::setup(){

    ofSetFrameRate(60);
    
    string host = Spacebrew::SPACEBREW_CLOUD; // "localhost";
    string name = "of-canvas-example";
    string description = "Draw all the things";
    
    spacebrew.addSubscribe("X", Spacebrew::TYPE_RANGE);
    spacebrew.addSubscribe("Y", Spacebrew::TYPE_RANGE);
    spacebrew.addSubscribe("hue", Spacebrew::TYPE_RANGE);
    spacebrew.addSubscribe("clear", Spacebrew::TYPE_BOOLEAN);
    
    
    spacebrew.connect( host, name, description );
    
    Spacebrew::addListener(this, spacebrew);
    
    brewUI.setup(&spacebrew);
    
    //how many X and Y values we've gotten
    //only add a point to the vector if we've received
    //an even number, i.e. X AND Y
    numXYs = 0;
    
    //the color we're drawing with
    hue = 0;
    
}

//--------------------------------------------------------------
void ofApp::update(){

    //update the UI tool
    brewUI.update();
    
}

//--------------------------------------------------------------
void ofApp::draw(){
    
    
    ofBackgroundGradient(100, 0);
    
    ofSetLineWidth(4);
    
    //if we have points...
    if(points.size() > 0){
        
        //Go through them and draw a line from this point to the next
        for(int i = 0; i < points.size() - 1; i++){
            
            //need to make sure we have enough colors to
            ofSetColor(ofColor::fromHsb(hues[i], 200, 200));
            ofDrawLine(points[i], points[i + 1]);
            
        }
    }
    
    
    //draw UI
    brewUI.draw(10, 10);
    
    
    //Draw app info to screen
    string msg;
    
    msg += "App for receiving drawing data from Spacebrew \"example_drawing_tool\"\n";
    msg += "Don't forget to add routes from drawing tool to shared canvas at \"" + spacebrew.getHost() + "\"!\n";
    msg += "\n";
    msg += "Points received: " + ofToString(points.size()) + "\n";
    msg += "Press 'h' to hide Spacebrew Admin Info\n";
    msg += "Press 's' to show Spacebrew Admin Info\n";

    
    
    ofSetColor(255);
    ofDrawBitmapString(msg, 10, ofGetHeight() - 80);
    
}


//--------------------------------------------------------------
void ofApp::onMessage( Spacebrew::Message & msg ){

    
    //go through message and see what the values are
    if(msg.name == "X"){
        currentPoint.x = ofMap(msg.valueRange(), 0, 1023, 0, ofGetWidth());
        numXYs++;
        
    } else if(msg.name == "Y"){
        currentPoint.y = ofMap(msg.valueRange(), 0, 1023, 0, ofGetHeight());
        numXYs++;
        
    } else if(msg.name == "hue"){
        hue = msg.valueRange();

        
    } else if(msg.name == "clear"){
        
        points.clear();
        hues.clear();
        numXYs = 0;
        
    }
    
    
    //if we've received 2 points THEN add them
    //to the vector
    if(msg.name == "X" || msg.name == "Y"){
        if(numXYs % 2 == 0){
            points.push_back(currentPoint);
            hues.push_back(hue);
        }
    }
    
}

//--------------------------------------------------------------
void ofApp::keyPressed(int key){

    
    //show the gui if it's hidden
    if(key == 's'){
        brewUI.show();
    }
    
    //hide the gui if it's shown
    if(key == 'h'){
        brewUI.hide();
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
