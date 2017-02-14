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

// The linux build target ocassioanlly has a "Status" macro defined (enfuriating!)
// and this obviously screws up with our enum definition. Rather than change the enum
// name which would break compatibility with existing users of this addon, let's first
// attempt to undefine the macro. Worth noting that I'm going to go ahead and wrap this
// around a check for the Linux target just to be extra super-duper sure that we don't
// inadvertantly screw up existing code in some weird edge case (although this has been
// tested with compilation on both OSX and Linux and it seems to be okay).
#ifdef TARGET_LINUX
#undef Status
#endif

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
			  const vector<string> & emailAddresses,
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
	float reconnectWait = 10;
	
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

