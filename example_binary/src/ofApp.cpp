#include "ofApp.h"

ofBuffer buff, inBuff;
ofImage test, test2;
bool bLoad = false;

//--------------------------------------------------------------
void ofApp::setup(){
    spacebrew.addPublish("newImage", "image");
    spacebrew.addSubscribe("loadImage", "image");
    spacebrew.connect( "localhost", "binaryTester_"+ofToString(ofRandom(10)) );
    
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
    if ( toLoad.length() != 0 ){
        jpeg.load(toLoad, test);
        toLoad = "";
    }
}

//--------------------------------------------------------------
void ofApp::draw(){
    ofPushMatrix();
    float height = max((test.height + test2.height), ofGetHeight());
    
    float scale = ofGetHeight()/height;
    ofScale(scale, scale);
    if ( test.isAllocated()){
        test.draw(0,0);
    }
    if ( test2.isAllocated() ){
        test2.draw(0,test.isAllocated() ? test.height : 0);
    }
    ofPopMatrix();
    ofDrawBitmapString("Drag an image on the window to load.\nPress 's' to send to Spacebrew!", 20, 20 );
}

//--------------------------------------------------------------
void ofApp::keyPressed(int key){
    if ( key == 's' ){
        if ( test.isAllocated() ){
            unsigned long size = 0;
            unsigned char * data = jpeg.compress(test, 90, &size);
            
            buff.clear();
            buff.append(reinterpret_cast<char *>(data), size);
            
            cout << "DATA "<<size<<":"<<buff.size()<< endl;
            spacebrew.sendBinary("newImage", "image", buff);
        }
    }
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
void ofApp::dragEvent(ofDragInfo dragInfo){ 
    for (int i=0; i<dragInfo.files.size(); i++){
        string file = dragInfo.files[i];
        ofFile f(file);
        string ex = f.getExtension();
        std::transform(ex.begin(), ex.end(),ex.begin(), ::toupper);
        
        if ( ex == "JPG" || ex == "JPEG" || ex == "PNG" || ex == "GIF" ){
            toLoad = file;
        }
    }
}
