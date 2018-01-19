//
//  ofxSensu.cpp
//  emptyExample
//
//  Created by Oriol Ferrer Mesià on 10/16/16.
//
//

#include "ofxSensu.h"


ofxSensu::~ofxSensu(){
	waitForThread(true, reconnectWait * 1000);
};


void ofxSensu::setup(string ip, int port) {
	
	if (isSetup) {
		
		ofLogError("ofxSensu") << "Already setup! dont setup twice!";
		
	}else{
		
		ofLogNotice("ofxSensu") << "Setting up with host: " << ip << " port: " << port;
		settings.address = ip;
		settings.port = port;
		settings.blocking = true;
		settings.messageDelimiter = "\r\n";
		isSetup = true;
	}
}

bool ofxSensu::isServerThere(){
	
	if(!isSetup){
		ofLogError("ofxSensu") << "not setup! set me up first!";
		return false;
	}
	
	ofxTCPClient tempClient;
	bool ok = tempClient.setup(settings);
	bool ret = false;
	if(!ok){
		ofLogError("ofxSensu") << "Can't connect to Sensu Server!";
	}else{
		tempClient.send("ping");
		ofSleepMillis(1);
		string rec = tempClient.receiveRaw();
		tempClient.close();
		if(rec == "pong"){
			ret = true;
		}
	}
	tempClient.close();
	return ret;
}


bool ofxSensu::send(const string & alertName,
					const string & msg,
					Status status,
					const vector<string> & emailAddresses,
					const string & filePath,
					bool blocking
					){
	
	vector<string> files;
	if(filePath.size()) files.push_back(filePath);
	
	return send(alertName, msg, status, emailAddresses, files, blocking);
}


bool ofxSensu::send(const string & alertName,
					const string & msg,
					Status status,
					const vector<string> & emailAddresses,
					const vector<string> & filePaths,
					bool blocking
					){
	
	if(!isSetup){
		ofLogError("ofxSensu") << "not setup! set me up first!";
		return false;
	}
	
	if(emailAddresses.size() == 0){
		ofLogError("ofxSensu") << "can't send command without an email address!";
		return false;
	}
	
	ofLogNotice("ofxSensu") << 	"sending Alert: \"" << alertName << "\" msg: \"" << msg << "\" to " <<
	emailAddresses.size() << " email addresses with " << filePaths.size() << " files.";
	
	string command = buildCommand(alertName, msg, status, emailAddresses, filePaths);
	
	if(blocking){ //lets connect right
		return sendCmdBlocking(command);
	}else{
		lock();
		commands.push_back(command);
		unlock();
		if(!isThreadRunning()){
			startThread();
		}
	}
	return true; //hopefully true
}

void ofxSensu::buildStillAliveMessage(const string & name,
									  const int ttl,
									  const string & output,
									  const string & remediation,
									  const int occurrences,
									  const int severity){
	
	const char* format = "{"
	"\"name\": \"%s\", "
	"\"ttl\": %d, "
	"\"handlers\": [\"remediator\"], "
	"\"output\":\"%s\", "
	"\"remediation\": { "
	"\"%s\": "
	"{ "
	"\"occurrences\" : [%d], "
	"\"severities\" : [%d] "
	"}}}";
	
	auto size = std::snprintf(nullptr, 0, format,
							  name.c_str(),
							  ttl,
							  output.c_str(),
							  remediation.c_str(),
							  occurrences,
							  severity
							  );
	stillAliveMsg = string(size + 1, '\0');
	std::sprintf(&stillAliveMsg[0], format,
				 name.c_str(),
				 ttl,
				 output.c_str(),
				 remediation.c_str(),
				 occurrences,
				 severity
				 );
}

string ofxSensu::buildCommand(const string & alertName,
							  const string & msg,
							  Status status,
							  const vector<string> & emailAddresses,
							  const vector<string> & filePaths){
	
	string emailList;
	for(auto & s : emailAddresses){
		emailList += "\"" + s + "\", ";
	}
	if(emailAddresses.size()) emailList = emailList.substr(0, emailList.size()-2); //remove last comma
	
	string fileList;
	for(auto & s : filePaths){
		string p = ofToDataPath(s, true);
#ifdef TARGET_WIN32
		ofStringReplace(p, "\\", "/");
#endif
		fileList += "\"" + p + "\", ";
	}
	if(fileList.size()) fileList = fileList.substr(0, fileList.size()-2); //remove last comma
	
	if(status < RESOLVED) status = RESOLVED;
	if(status > CRITICAL_ERROR) status = CRITICAL_ERROR;
	
	string cleanMsg = msg;
	ofStringReplace(cleanMsg, "\"", "\\\""); //escape quotes for json to be valid
	ofStringReplace(cleanMsg, "\n", "\\n"); //escape newlines for json to be valid
	
	
	
	string command = 	"{\"name\" : \"" + alertName + "\", " +
	"\"output\" : \"" + cleanMsg + "\", " +
	"\"status\" : " + ofToString((int)status) + ", " +
	"\"handlers\" : [\"email\"], " +
	"\"email_to\" : [" + emailList + "], " +
	"\"files\": [" + fileList + "], " +
	"\"occurrences\": 1, " +
	"\"refresh\": 60 " +
	"}";
	return command;
}

void ofxSensu::sendStillAlive(){
	
	if(!isSetup){
		ofLogError("ofxSensu") << "not setup! set me up first!";
		return;
	}
	if(stillAliveMsg.length() == 0){
		ofLogError("ofxSensu") << "No still alive message specified, call buildStillAliveMessage first!";
		return;
	}
	
	lock();
	commands.push_back(stillAliveMsg);
	unlock();
	if(!isThreadRunning()){
		startThread();
	}
}

bool ofxSensu::connect(ofxTCPClient & client){
	
	bool connected = client.setup(settings);
	int numFail = 0;
	
	while(!connected && isThreadRunning()){ //try reconnect if not connected
		numFail++;
		ofLogWarning("ofxSensu") 	<< "can't connect to Sensu Server at \"" << settings.address << ":" << settings.port
		<< " - will keep trying... (" << numFail << ")" ;
		int n = 100;
		for(int i = 0; i < n; i++){
			sleep(1000 * reconnectWait / float(n));
			if(!isThreadRunning()) return false; //we have been asked to stop the thread, so we return early
		}
		connected = client.setup(settings);
	}
	return connected;
}


void ofxSensu::threadedFunction(){
	
#ifdef TARGET_WIN32
#elif defined(TARGET_LINUX)
	pthread_setname_np(pthread_self(), "ofxSensu");
#else
	pthread_setname_np("ofxSensu");
#endif
	
	int nCommands = 0;
	
	lock();
	nCommands = commands.size();
	unlock();
	
	//at this point we should be connected
	while(nCommands > 0){
		
		ofxTCPClient client; //each new command starts a new TCP connection!
		bool connected = connect(client);
		
		if(!connected){ //this will only happen if we are trying to stop the thread
			return; //otherwise connect will keep trying forever
		}
		lock();
		string cmd = commands.front();
		commands.erase(commands.begin());
		unlock();
		
		bool sent = client.send(cmd);
		ofSleepMillis(1);
		string rec = client.receiveRaw();
		client.close();
		
		ofLogNotice("ofxSensu") << "command sent '" << cmd << "'";
		ofLogNotice("ofxSensu") << "Server returned '" << rec << "'";
		
		lock();
		nCommands = commands.size();
		unlock();
	}
}


bool ofxSensu::sendCmdBlocking(const string & cmd){
	ofxTCPClient tempClient;
	bool ok = tempClient.setup(settings);
	if(ok){
		tempClient.send(cmd);
		ofSleepMillis(1);
		string rec = tempClient.receiveRaw();
		ofLogNotice("ofxSensu") << "sendCmdBlocking() '" << cmd << "' and it returned '" << rec << "'";
		tempClient.close();
		return true;
	}else{
		ofLogError("ofxSensu") << "cant connect to SensuServer! cant send comand!";
		return false;
	}
}