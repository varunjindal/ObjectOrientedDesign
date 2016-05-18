/////////////////////////////////////////////////////////////////////////////
// Sender.h - This file provides functions to send different types of data //
//            to server.                                                   //
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
* This package provides basically functions or operations which are related to sent some data to Server.
* Here, it includes operations which send file upload, send test string data or send any other request from
* client to server.
*
* Public Interface:
* ==============================
* HttpMessage makeMessage(size_t n, const std::string& msgBody, const EndPoint& ep);
* void sendMessage(HttpMessage& msg, Socket& socket);
* bool sendFile(const std::string& filename, Socket& socket, int fPort, int tport);
* bool sendFilesWithDeps(const std::string& fileToSend, Socket& socket,std::vector<std::string> dependencies, int fPort, int tport);
* void getFiles(std::string filename, Socket& socket, bool ExtractWithDepsFlag, int cPort, int sPort);

* Required Files:
* ============================
* HttpMessage.cpp , HttpMessage.h
* Sender.cpp, Sender.h
* Sockets.cpp , Sockets.h
* FileManager.h, FileManager.cpp
* Filesystem.h FileSystem.cpp
* Utilities.h, Utilities.cpp, Logger.h, Logger.cpp
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
#include <fstream>
#include "../sockets/sockets.h"
#include "../HttpMessage/HttpMessage.h"

class Sender
{

public:
	using EndPoint = std::string;	
	HttpMessage makeMessage(size_t n, const std::string& msgBody, const EndPoint& ep);
	void sendMessage(HttpMessage& msg, Socket& socket);
	bool sendFile(const std::string& filename, Socket& socket, int fPort, int tport);
	bool sendFilesWithDeps(const std::string& fileToSend, Socket& socket,std::vector<std::string> dependencies, int fPort, int tport);
	void getFiles(std::string filename, Socket& socket, bool ExtractWithDepsFlag, int cPort, int sPort);
};
