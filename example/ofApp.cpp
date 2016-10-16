#include "ofApp.h"


void ofApp::setup(){

	ofBackground(22);
	ofSetFrameRate(60);
	sensu.setup("192.168.103.10", 3030);
}


void ofApp::update(){
}


void ofApp::draw(){

}


void ofApp::keyPressed(int key){

	float time = ofGetElapsedTimef();
	switch (key) {
		case '1': sensu.send("ofxSensuAlert", "testing ofxSensu", ofxSensu::RESOLVED, "oriol@localprojects.com", "banana.jpg", true); break;
		case '2': sensu.send("ofxSensuAlert", "testing ofxSensu", ofxSensu::WARNING, "oriol@localprojects.com", "banana.jpg", true); break;
		case '3': sensu.send("ofxSensuAlert", "testing ofxSensu", ofxSensu::CRITICAL_ERROR, "oriol@localprojects.com", "banana.jpg", true); break;

		case '4': sensu.send("ofxSensuAlert", "testing ofxSensu", ofxSensu::RESOLVED, "oriol@localprojects.com", "banana.jpg", false); break;
		case '5': sensu.send("ofxSensuAlert", "testing ofxSensu", ofxSensu::WARNING, "oriol@localprojects.com", "banana.jpg", false); break;
		case '6': sensu.send("ofxSensuAlert", "testing ofxSensu", ofxSensu::CRITICAL_ERROR, "oriol@localprojects.com", "banana.jpg", false); break;
	}
	
	ofLogNotice("ofApp") << "Sending Command took " << ofGetElapsedTimef() - time << " seconds";
}
