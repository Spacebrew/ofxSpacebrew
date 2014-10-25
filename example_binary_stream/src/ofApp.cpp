#include "ofApp.h"

ofBuffer buff, inBuff;
ofImage test, test2;
bool bLoad = false;
int sendInterval = 500;
int lastSent = 0;

//--------------------------------------------------------------
void ofApp::setup(){
    spacebrew.addPublish("videoStream", "image");
    spacebrew.connect( "127.0.0.1", "videoStreamer" );
    
    grabber.initGrabber(320, 240);
    
    ofAddListener(spacebrew.onBinaryMessage, this, &ofApp::onBinaryMessage);
}

//--------------------------------------------------------------
void ofApp::update(){
    // incoming
    if ( inBuff.size() != 0 && bLoad ){
        jpeg.load(inBuff, test2);
        inBuff.clear();
    }
    // outgoing
    grabber.update();
    if ( grabber.isFrameNew() && ofGetElapsedTimeMillis() - lastSent > sendInterval ){
        lastSent = ofGetElapsedTimeMillis();
        unsigned long size = 0;
        unsigned char * data = jpeg.compress(grabber, 50, &size);
        
        buff.clear();
        buff.append(reinterpret_cast<char *>(data), size);
        spacebrew.sendBinary("videoStream", "image", buff);
    }
}

//--------------------------------------------------------------
void ofApp::draw(){
    grabber.draw(0,0);
}

//--------------------------------------------------------------
void ofApp::keyPressed(int key){
}

//--------------------------------------------------------------
void ofApp::onBinaryMessage( Spacebrew::BinaryMessage & e ){
    if ( e.name == "loadImage" ){
        
        cout << "got data of size "<<e.data().size()<<endl;
        inBuff.clear();
        inBuff.append(e.data().getBinaryBuffer(), e.data().size());
        bLoad = true;
    } else {
        cout <<"wtf "<<e.name<<endl;
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
void ofApp::windowResized(int w, int h){

}

//--------------------------------------------------------------
void ofApp::gotMessage(ofMessage msg){

}

//--------------------------------------------------------------
void ofApp::dragEvent(ofDragInfo dragInfo){}
