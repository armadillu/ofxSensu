//
//  ofxSensu.h
//  emptyExample
//
//  Created by Oriol Ferrer Mesià on 10/16/16.
//
//

#pragma once
#include "ofMain.h"
#include "ofxNetwork.h"

class ofxSensu : public ofThread{

public:
	
	enum Status{
		RESOLVED = 0,
		WARNING = 1,
		CRITICAL_ERROR = 2
	};
	
	~ofxSensu();
	
	void setup(string host = "127.0.0.1", int port = 3030);
	bool isServerThere(); //ping! blocks!

	bool send(const string & alertName,
			  const string & msg,
			  Status status,
			  const string emailAddress,
			  const string & filePath,
			  bool blocking
			  );

	bool send(const string & alertName,
			  const string & msg,
			  Status status,
			  const vector<string> & emailAddresses,
			  const vector<string> & filePaths,
			  bool blocking
			  );

protected:

	ofxTCPSettings settings = ofxTCPSettings("127.0.0.1", 3030);
	
	bool isSetup = false;
	float reconnectWait = 3;
	
	string buildCommand(const string & alertName,
						const string & msg,
						Status status,
						const vector<string> & emailAddresses,
						const vector<string> & filePaths);
	
	void threadedFunction();
	
	bool connect(ofxTCPClient & client);
	bool sendCmdBlocking(const string & cmd);
	
	vector<string> commands;
};

