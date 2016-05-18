/////////////////////////////////////////////////////////////////////////////
// Server.h - Send file to server and receive ACK from server              //
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
* This package demonstrate the server and client communication.Sever class starts its own sender
* and receiver. Server will start listening on some port. Once data or file will be received from Client
* then server will add those messages in blocking queue. If there is file block request then it will
* directly write on file. Sever's sender will dequeue those messages and send to Client as per their ip
* and port numbers.
*
* Public Interface:
* ==============================
* void startReceiverThread();
* void startReceiver();
* HttpMessage makeMessage(size_t n, const std::string& body, const EndPoint& ep);
* void startTask();
* void sendServerFiles(HttpMessage msg);
* void sendFileDependencies(HttpMessage msg,std::string path, Socket & socket);
* std::string getLatestDir(std::vector<std::string> dirs, std::string fileToSend);
* int getServerPort()
* void setServerPort(int port)
* Required Files:
* ============================
* Server.h, Server.cpp, HttpMessage.cpp , HttpMessage.h
* Sender.cpp, Sender.h, Reciever.cpp, Reciever.h
* Sockets.cpp , Sockets.h,XMLParser.h,XMLParser.cpp
* FileManager.h, FileManager.cpp,XMLElement.h,XMLElement.cpp
* Filesystem.h FileSystem.cpp
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
#include <string>
#include <iostream>
#include "../HttpMessage/HttpMessage.h"
#include "../sockets/sockets.h"
#include "../Receiver/Receiver.h"

class Server
{
public:
	Server(std::string address = "localhost", int port = 9085) : address_(address), port_(port){}
	void startReceiverThread();
	std::thread startReceiver();	
	using EndPoint = std::string;
	HttpMessage makeMessage(size_t n, const std::string& body, const EndPoint& ep);
	void startTask();
	void sendServerFiles(HttpMessage msg);
	void sendFileDependencies(HttpMessage msg,std::string path, Socket & socket);
	std::string getLatestDir(std::vector<std::string> dirs, std::string fileToSend);
	int getServerPort()
	{
		return port_;
	}

	void setServerPort(int port)
	{
		port_ = port;
	}

private:
	BlockingQueue<std::string> queue;
	std::string address_;
	int port_;
};