#include "ofApp.h"

//--------------------------------------------------------------
void ofApp::setup(){

	sensu.setup("192.168.103.10", 3030);
}

//--------------------------------------------------------------
void ofApp::update(){
}

//--------------------------------------------------------------
void ofApp::draw(){

}

//--------------------------------------------------------------
void ofApp::keyPressed(int key){

	
	switch (key) {
		case '1': sensu.send("ofxSensuAlert", "testing ofxSensu", ofxSensu::RESOLVED, "oriol@localprojects.com", "banana.jpg", true); break;
		case '2': sensu.send("ofxSensuAlert", "testing ofxSensu", ofxSensu::WARNING, "oriol@localprojects.com", "banana.jpg", true); break;
		case '3': sensu.send("ofxSensuAlert", "testing ofxSensu", ofxSensu::CRITICAL_ERROR, "oriol@localprojects.com", "banana.jpg", true); break;

		case '4': sensu.send("ofxSensuAlert", "testing ofxSensu", ofxSensu::RESOLVED, "oriol@localprojects.com", "banana.jpg", false); break;
		case '5': sensu.send("ofxSensuAlert", "testing ofxSensu", ofxSensu::WARNING, "oriol@localprojects.com", "banana.jpg", false); break;
		case '6': sensu.send("ofxSensuAlert", "testing ofxSensu", ofxSensu::CRITICAL_ERROR, "oriol@localprojects.com", "banana.jpg", false); break;
			
  	default:
			break;
	}
}

//--------------------------------------------------------------
void ofApp::keyReleased(int key){

}

//--------------------------------------------------------------
void ofApp::mouseMoved(int x, int y){

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