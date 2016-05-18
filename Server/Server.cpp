/////////////////////////////////////////////////////////////////////////////
// Server.cpp - Send file to server and receive ACK from server            //
//																		   //
// ----------------------------------------------------------------------- //
// Language:    Visual C++, Visual Studio 2015                             //
// Platform:    Dell Inspiron 13i, Core i5, Windows 10                     //
// Application: CSE687 - Object Oriented Design Project4S16                //
// Author:      Varun Jindal, MSCE Current Graduate Student,               //
//              Syracuse Univesity                                         //
//              vjindal@syr.edu                                            //
/////////////////////////////////////////////////////////////////////////////

#include "Server.h"
#include "../Sender/Sender.h"
#include"../FileSystem/FileSystem.h"
#include "../XmlParser/XmlParser.h"
#include "../XmlParser/xmldocument.h"
#include "../XmlParser/XmlElement.h"

using Show = StaticLogger<1>;
using namespace Utilities;
using namespace XmlProcessing;
using sPtr = std::shared_ptr < AbstractXmlElement >;



void Server::startTask()
{
	std::thread rec = startReceiver();
	rec.join();	
}
// --------------------  This Function sends the requested file to the client and checks if the dependencies need to be sent--------------------
void Server::sendServerFiles(HttpMessage msg)
{	
	SocketSystem ss;
	SocketConnecter si;
	std::string cPort = msg.findValue("From-Port");
	int ClientPort = Converter<int>::toValue(cPort);	
	while (!si.connect("localhost", ClientPort))
	{
		::Sleep(100);
	}																					// Searches for the revlevant folder and then sends the file
	Sender send;	
	std::string DepsFlag = msg.findValue("ExtractWithDepsFlag");
	std::string fileToSend = msg.findValue("file");
	std::string path;	
	std::vector<std::string> dirs = FileSystem::Directory::getDirectories("./ServerDrive", fileToSend+"*");	
	if (dirs.size() == 0)
		Show::write("No such file present in the repository => " + fileToSend + "\n");
	if (dirs.size() == 1)
	{
		std::vector<std::string> files = FileSystem::Directory::getFiles("./ServerDrive/" + dirs[0], fileToSend + "*");
	    Show::write("\n Sending File :: " + files[0] + " to Client @: " + cPort);
		send.sendFile("./ServerDrive/" + dirs[0] + "/" + files[0], si, getServerPort(), ClientPort);
		path = "./ServerDrive/" + dirs[0] + "/";
		if (DepsFlag == "Yes")
			sendFileDependencies(msg, path,si);
	}
	if (dirs.size() > 1)
	{
		std::string latest_dir = getLatestDir(dirs,fileToSend);
		std::vector<std::string> files = FileSystem::Directory::getFiles("./ServerDrive/" + latest_dir, fileToSend + "*");
		Show::write("\n Sending File :: " + files[0] + " to Client @: " + cPort);
		send.sendFile("./ServerDrive/" + latest_dir + "/" + files[0], si , getServerPort(), ClientPort);
		path = "./ServerDrive/" + latest_dir + "/";
		if (DepsFlag == "Yes")
			sendFileDependencies(msg, path,si);
	}
	msg = makeMessage(1, "quit", "toAddr:localhost:"+cPort);
	send.sendMessage(msg, si);
}
// --------------------  This Function sends the Dependent files to the client--------------------
void Server::sendFileDependencies(HttpMessage msg, std::string path, Socket & socket)
{
	std::string cPort = msg.findValue("From-Port");
	int ClientPort = Converter<int>::toValue(cPort);
	std::string requestedFile = msg.findValue("file");
	Sender send;
	std::vector<std::string> depFile = FileSystem::Directory::getFiles(path, "*.xml");
	std::string src = path + depFile[0];
	XmlParser parser(src);
	XmlDocument* pDoc = parser.buildDocument();
	std::vector<std::string> depFileNames;
	std::vector<sPtr> found = pDoc->element("Dependencies").descendents().select();					// Read the dependencies xml
	if (found.size() > 0)
	{
		for (auto pElem : found)
		{		
			if (pElem->value() != "dep") 
				depFileNames.push_back(pElem->value());			
		}
	}
	for (std::string f : depFileNames) {																			// Sending each of the dependency file if exists
		std::string fileToSend = Utilities::StringHelper::trim(f);		
		std::vector<std::string> dirs = FileSystem::Directory::getDirectories("./ServerDrive/", fileToSend +"*");		
		if (dirs.size() == 0)
			Show::write("No such file present in the repository => " + fileToSend + "\n");		
		if (dirs.size() > 0)
		{
			std::string latest_dir = getLatestDir(dirs, fileToSend);			
			std::vector<std::string> files = FileSystem::Directory::getFiles("./ServerDrive/" + latest_dir, fileToSend + "*");
			Show::write("\nSending Dependency File of "+ requestedFile+ " => " + files[0] + " to client @: "+cPort);
			send.sendFile("./ServerDrive/" + latest_dir + "/" + files[0], socket , getServerPort(), ClientPort);
			path = "./ServerDrive/" + latest_dir + "/";			
		}
	}
	msg = makeMessage(1, "quit", "toAddr:localhost:"+cPort);
	send.sendMessage(msg, socket);
}
// ------------------------- Returns name of the latest folder with same names -------------------
std::string Server::getLatestDir(std::vector<std::string> dirs, std::string fileToSend)
{	
	return dirs[dirs.size()-1];
}

// ------------------------------------ Starts Receiver for the this component ---------------------------
void Server::startReceiverThread()
{
	BlockingQueue<HttpMessage> msgQ;
	try
	{
		SocketSystem ss;
		SocketListener sl(getServerPort(), Socket::IP6);
		Receiver cp(msgQ,"Server");
		sl.start(cp);
		/*
		* Since this is a server the loop below never terminates.
		* We could easily change that by sending a distinguished
		* message for shutdown.
		*/
		while (true)
		{			
			HttpMessage msg = msgQ.deQ();
			std::string cPort = msg.findValue("From-Port");
			if (msg.attributes()[0].first == "GET") {												// Checks if its an file extraction request
				std::string filename = msg.findValue("file");				
				Show::write("\n\n Client @: "+cPort+ " => requested file :: " + filename);
				sendServerFiles(msg);
			}
			else
			Show::write("\n\n Message from client @:"+cPort +"  =>  " + msg.bodyString());
		}
	}
	catch (std::exception& exc)
	{
		Show::write("\n  Exeception caught: ");
		std::string exMsg = "\n  " + std::string(exc.what()) + "\n\n";
		Show::write(exMsg);
	}
}
using EndPoint = std::string;
HttpMessage Server::makeMessage(size_t n, const std::string& body, const EndPoint& ep)
{
	HttpMessage msg;
	HttpMessage::Attribute attrib;
	EndPoint myEndPoint = "localhost:8080";  // ToDo: make this a member of the sender
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

// ---------------- < Called to start the receiver > -------------------------
std::thread Server::startReceiver()
{
	std::thread t1(
		[&]() { startReceiverThread(); } 
	);
	return t1;
}


#ifdef TEST_SERVER

int main(int argc, char* argv[])
{
	Server s1;
	::SetConsoleTitle(L"HttpMessage Server - Runs Forever");
	Show::attach(&std::cout);
	Show::start();
	Show::title("\n  HttpMessage Server started");
	std::string arg = argv[1];
	//std::cout << "\n ARG :: " << arg;
	int ArgPort = Converter<int>::toValue(arg);
	//std::cout << "\n ArgPort :: " << ArgPort;
	int server_port = ArgPort;
	s1.setServerPort(server_port);
	s1.startTask();		
}

#endif // TEST_SERVER