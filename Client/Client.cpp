/////////////////////////////////////////////////////////////////////////////
// Client.cpp - Send file to server and receive ACK from server            //
//																		   //
// ----------------------------------------------------------------------- //
// Language:    Visual C++, Visual Studio 2015                             //
// Platform:    Dell Inspiron 13i, Core i5, Windows 10                     //
// Application: CSE687 - Object Oriented Design Project4S16                //
// Author:      Varun Jindal, MSCE Current Graduate Student,               //
//              Syracuse Univesity                                         //
//              vjindal@syr.edu                                            //
/////////////////////////////////////////////////////////////////////////////

#include "Client.h"
#include "../Sender/Sender.h"
#include "../HttpMessage/HttpMessage.h"
#include "../Sockets/Sockets.h"
#include "../FileSystem/FileSystem.h"
#include "../Logger/Logger.h"
#include "../Utilities/Utilities.h"
#include <string>
#include <iostream>
#include <thread>
#include "../XmlParser/XmlParser.h"
#include "../XmlParser/xmldocument.h"
#include "../XmlParser/XmlElement.h"

using Show = StaticLogger<1>;
using namespace Utilities;
using Utils = StringHelper;
using namespace XmlProcessing;
using sPtr = std::shared_ptr < AbstractXmlElement >;

using EndPoint = std::string;
HttpMessage Client::makeMessage(size_t n, const std::string& body, const EndPoint& ep)
{
	HttpMessage msg;
	HttpMessage::Attribute attrib;
	EndPoint myEndPoint = "localhost:8081";  // ToDo: make this a member of the sender
											 // given to its constructor.
	switch (n)
	{
	case 1:
		msg.clear();
		msg.addAttribute(HttpMessage::attribute("POST", "Message"));
		msg.addAttribute(HttpMessage::Attribute("mode", "oneway"));
		msg.addAttribute(HttpMessage::parseAttribute("toAddr:" + ep));
		msg.addAttribute(HttpMessage::parseAttribute("fromAddr:" + myEndPoint));

		msg.addBody(body);
		if (body.size() > 0)
		{
			attrib = HttpMessage::attribute("content-length", Converter<size_t>::toString(body.size()));
			msg.addAttribute(attrib);
		}
		break;
	default:
		msg.clear();
		msg.addAttribute(HttpMessage::attribute("Error", "unknown message type"));
	}
	return msg;
}

//--------------------------------< Analyses and return vector of user input files> ----------------
std::vector<std::string> Client::analyzeInputFolder(std::string inputFolder)
{
	std::vector<std::string> files = FileSystem::Directory::getFiles("./"+inputFolder, "*.xml");	
	return files;
}

//--------------------------------< Analyses user input file and starts operations > ----------------
void Client::analyzeInputFile(std::string filename, Socket & socket)
{
	std::string src = filename;
	XmlParser parser(src);
	XmlDocument* pDoc = parser.buildDocument();	
	std::vector<sPtr> found = pDoc->element("test").descendents().element("Operation").descendents().select();
	if (found.size() > 0)
	{
		for (auto pElem : found)										// Read input xml
		{
			if (pElem->value() == "Check-In")
			{
				checkin(pDoc,socket);				
			}
			if (pElem->value() == "Extraction")
			{
				extraction(pDoc, socket);
			}
		}
	}
	
}
//------------------------ << Starts file Checkin process > --------------------------
void Client::checkin(XmlDocument * pDoc, Socket & socket)
{
	std::string ClientPort = Converter<int>::toString(getClientPort());
	std::string ServerPort = Converter<int>::toString(getServerPort());
	std::string fileToSend;
	bool depFlag;
	std::vector<sPtr> file = pDoc->element("test").descendents().element("FileName").descendents().select();
	if (file.size() > 0)
	{
		for (auto pFileName : file)
		{
			fileToSend = pFileName->value();
		}
	}
	std::vector<sPtr> dFlag = pDoc->element("test").descendents().element("DepFlag").descendents().select();
	if (dFlag.size() > 0)
	{
		for (auto pFlagName : dFlag)
		{			
			if (pFlagName->value() == "Yes") depFlag = true;
			else
				depFlag = false;
		}
	}	
	std::vector<std::string> dependencies = extractDeps(pDoc);
	Sender s;
	if (depFlag) {
		Show::write("\n\n Client @: " + ClientPort + " :: Checking-in file = " + fileToSend + " with dependency info on server repository @: " + ServerPort);
		s.sendFilesWithDeps(fileToSend, socket, dependencies, getClientPort(), getServerPort());
	}
	else {
		Show::write("\n\n Client @: " + ClientPort + " :: Checking-in file = " + fileToSend + " without dependency info on server repository @: " + ServerPort);
		s.sendFile(fileToSend, socket, getClientPort(), getServerPort());
	}
}

//-------------------------- < Returns vector with all the dependent files > --------------------
std::vector<std::string> Client::extractDeps(XmlDocument * pDoc)
{
	std::vector<std::string> deps;
	std::vector<sPtr> file = pDoc->element("test").descendents().element("Dependencies").descendents().select();
	if (file.size() > 0)
	{
		for (auto pFileName : file)
		{
			if(pFileName->tag() == "")
			deps.push_back(pFileName->value());
		}
	}
	return deps;
}

// -------------------------- << Starts File extraction process from server >> --------------------------

void Client::extraction(XmlDocument * pDoc, Socket & socket)
{
	std::string ClientPort = Converter<int>::toString(getClientPort());
	std::string ServerPort = Converter<int>::toString(getServerPort());
	std::string fileToGet;
	bool ExtractWithDepsFlag;
	std::vector<sPtr> file = pDoc->element("test").descendents().element("FileName").descendents().select();
	if (file.size() > 0)
	{
		for (auto pFileName : file)
		{
			fileToGet = pFileName->value();
		}
	}
	std::vector<sPtr> dFlag = pDoc->element("test").descendents().element("ExtractWithDeps").descendents().select();
	if (dFlag.size() > 0)
	{
		for (auto pFlagName : dFlag)
		{
			if (pFlagName->value() == "Yes") ExtractWithDepsFlag = true;
			else
				ExtractWithDepsFlag = false;
		}
	}
	Sender s;
	if(ExtractWithDepsFlag) 
		Show::write("\n\n Client @: " + ClientPort + " :: Requesting file = " + fileToGet + " with all dependency from server repository @: " + ServerPort);
	else
		Show::write("\n\n Client @: " + ClientPort + " :: Requesting file = " + fileToGet + " without dependency from server repository @: " + ServerPort);

	s.getFiles(fileToGet, socket, ExtractWithDepsFlag,getClientPort(),getServerPort());
}

// ------------------------------------ < Start Sender Thread > --------------
void Client::startSenderThread()
{	
	Show::attach(&std::cout);
	Show::start();	
	try
	{
		SocketSystem ss;
		SocketConnecter si;		
		while (!si.connect("localhost", getServerPort()))
		{
			::Sleep(100);
		}
		std::vector<std::string> files = analyzeInputFolder("user1input");
		for (size_t i = 0; i < files.size(); ++i)
		{
			analyzeInputFile("./user1input/" + files[i], si);
		}		
	}
	catch (std::exception& exc)
	{
		Show::write("\n  Exeception caught: ");
		std::string exMsg = "\n  " + std::string(exc.what()) + "\n\n";
		Show::write(exMsg);
	}
}

// ---------------------------------- << Start Receiver >> ----------------------
std::thread Client::startReceiver()
{
	std::thread t1(
		[&]() { startReceiverThread(); } 
	);
	return t1;
}
// ---------------------------------- << Start Receiver Thread>> ----------------------
void Client::startReceiverThread()
{
	BlockingQueue<HttpMessage> msgQ;
	try
	{
		SocketSystem ss;
		SocketListener sl(getClientPort(), Socket::IP6);
		Receiver cp(msgQ,"Client");
		sl.start(cp);
		/*
		* Since this is a server the loop below never terminates.
		* We could easily change that by sending a distinguished
		* message for shutdown.
		*/
		while (true)
		{
			HttpMessage msg = msgQ.deQ();
			std::string ServerPort = msg.findValue("From-Port");
			std::string ClientPort = msg.findValue("To-Port");
			Show::write("\n\n Client@: "+ClientPort+":: Message from Server @:" + ServerPort + "  =>  " + msg.bodyString());
		}
	}
	catch (std::exception& exc)
	{
		Show::write("\n  Exeception caught: ");
		std::string exMsg = "\n  " + std::string(exc.what()) + "\n\n";
		Show::write(exMsg);
	}
}
// ---------------------------------- << Start Sender >> ----------------------
std::thread Client::startSender()
{
		std::thread t1(
		[&]() { startSenderThread(); } 
	);
	return t1;
}


// ------------------------- << Starting Receiver and Senders > ---------------------
void Client::startTask()
{
	std::thread rec = startReceiver();
	std::thread send =startSender();
	rec.join();
	send.join();
}

#ifdef TEST_CLIENT

int main(int argc, char* argv[])
{
	::SetConsoleTitle(L"Clients Running on Threads");

	Show::title("Demonstrating two HttpMessage Clients each running on a child thread");

	std::string cport = argv[1];	
	int clport = Converter<int>::toValue(cport);
	std::string sport = argv[2];	
	int srport = Converter<int>::toValue(sport);

	Client c1;

	c1.setServerPort(srport);

	c1.setClientPort(clport);

	c1.startTask();

}

#endif // TEST_CLIENT