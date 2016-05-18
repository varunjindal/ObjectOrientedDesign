/////////////////////////////////////////////////////////////////////////////
// Client.h - Send file to server and receive ACK from server              //
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
* This package demonstrate the server and client communication. First of all, Sender will do request
* to server for file upload and read file in blocks and send them to server. Once server wil done with
* fle upload message. Server will enqueue that message along with filename and sender ip and port information.
* Now, server's sender function will read this enqueue message from Blocking queue which is shared between Receiver
* and server.
*
* Public Interface:
* ==============================
void startReceiverThread();
std::thread startSender();
void startSenderThread();
std::thread startReceiver();
void startTask();
HttpMessage makeMessage(size_t n, const std::string& body, const EndPoint& ep);
std::vector<std::string> analyzeInputFolder(std::string);
void analyzeInputFile(std::string filename, Socket & socket);
void checkin(XmlDocument* pDoc, Socket & socket);
void extraction(XmlDocument* pDoc, Socket & socket);
std::vector<std::string> extractDeps(XmlDocument* pDoc);
int getServerPort()
void setServerPort(int port)
int getClientPort()
void setClientPort(int port)
*
* Required Files:
* ============================
* Client.h, Client.cpp, HttpMessage.cpp , HttpMessage.h
* Sender.cpp, Sender.h, Reciever.cpp, Reciever.h
* Sockets.cpp , Sockets.h, XMLParser.h,XMLParser.cpp
* Display.h,XMLElement.h,XMLElement.cpp,Filesystem.h FileSystem.cpp 
* FileManager.h, FileManager.cpp
*
* Build Command:
* ============================
* devenv Project4.sln /rebuild debug
*
* Maintenance History:
* ============================
* ver 1.0 : 3 May 15
*/


#pragma once
#include <string>
#include <iostream>
#include "../HttpMessage/HttpMessage.h"
#include "../sockets/sockets.h"
#include "../Receiver/Receiver.h"
#include "../XmlParser/XmlParser.h"
#include "../XmlParser/xmldocument.h"
#include "../XmlParser/XmlElement.h"
using namespace XmlProcessing;
using sPtr = std::shared_ptr < AbstractXmlElement >;

class Client
{
public:
	Client(std::string address = "localhost", int port = 8080) : address_(address), port_(port)	
	{
	}
	void startReceiverThread();
	std::thread startSender();
	void startSenderThread();
	std::thread startReceiver();
	void startTask();
	using EndPoint = std::string;
	HttpMessage makeMessage(size_t n, const std::string& body, const EndPoint& ep);	
	std::vector<std::string> analyzeInputFolder(std::string);
	void analyzeInputFile(std::string filename, Socket & socket);
	void checkin(XmlDocument* pDoc, Socket & socket);
	void extraction(XmlDocument* pDoc, Socket & socket);
	std::vector<std::string> extractDeps(XmlDocument* pDoc);
	int getServerPort()
	{
		return serverPort;
	}
	void setServerPort(int port)
	{
		serverPort = port;
	}
	int getClientPort()
	{
		return port_;
	}
	void setClientPort(int port)
	{
		port_ = port;
	}

private:
	BlockingQueue<HttpMessage> sendingQueue;	
	std::string address_;
	int port_;	
	int serverPort;	
};
