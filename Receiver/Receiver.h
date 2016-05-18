/////////////////////////////////////////////////////////////////////////////
// Receiver.h - This package handles operations related to do once it      //
//              recevied any request from client or server.                //
//																		   //
// ----------------------------------------------------------------------- //
// Language:    Visual C++, Visual Studio 2015                             //
// Platform:    Dell Inspiron 13i, Core i5, Windows 10                     //
// Application: CSE687 - Object Oriented Design Project4S16                //
// Author:      Varun Jindal, MSCE Current Graduate Student,               //
//              Syracuse Univesity                                         //
//              vjindal@syr.edu                                            //
/////////////////////////////////////////////////////////////////////////////

/*
* Package Operations:
* ===============================
* This package provides operations to do once server receives any request from client or server.
* such that file write if file upload request come, send string message if any ACK request come.
*
* Public Interface:
* ==============================
* void operator()(Socket socket);
* std::string getComponent();
*
* Required Files:
* ============================
* Receiver.cpp, Receiver.h, HttpMessage.cpp , HttpMessage.h
* Sockets.cpp , Sockets.h, XMLElement.h,XMLElement.cpp
*  FileManager.h, FileManager.cpp, XMLParser.h,XMLParser.cpp
* Filesystem.h FileSystem.cpp 
* Utilities.h, Utilities.cpp, Logger.h, Logger.cpp
* Cpp11-BlockingQueue.h,Cpp11-BlockingQueue.cpp
*
* Build Command:
* ============================
* devenv Project4.sln /rebuild debug
*
* Maintenance History:
* ============================
* ver 1.0 : 3 May 16
*/

#pragma once

#include <iostream>
#include <string>
#include <map>
#include <iostream>
#include <stdlib.h>
#include "../sockets/sockets.h"
#include "../HttpMessage/HttpMessage.h"


#define CLIENTROOTPATH "./download/"

class Receiver
{

public:
	Receiver(BlockingQueue<HttpMessage>& msgQ, std::string _component) : msgQ_(msgQ),component(_component) {}
	void operator()(Socket socket);
	std::string getComponent();
	
private:
	bool connectionClosed_;
	HttpMessage readMessage(Socket& socket);
	bool readFile(const std::string& filename, size_t fileSize, Socket& socket);
	void createDeps(const std::string& filename, Socket& socket, HttpMessage msg);
	std::string getTimeStamp();
	BlockingQueue<HttpMessage>& msgQ_;
	std::string component;
	std::string getLatestDir(std::vector<std::string> dirs, std::string fileToSend);
};

