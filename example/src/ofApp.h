#pragma once

#include "ofMain.h"
#include "ofxSensu.h"

class ofApp : public ofBaseApp{
	public:
		void setup();
		void update();
		void draw();
		
		void keyPressed(int key);
	
		ofxSensu sensu;
};
