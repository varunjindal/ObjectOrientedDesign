/////////////////////////////////////////////////////////////////////////////
// Sender.cpp-This file provides functions to send different types of data //
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

#include "Sender.h"
#include "../HttpMessage/HttpMessage.h"
#include "../Sockets/Sockets.h"
#include "../FileSystem/FileSystem.h"
#include "../Logger/Logger.h"
#include "../Utilities/Utilities.h"
#include <string>
#include <iostream>
#include <thread>

using Show = StaticLogger<1>;
using namespace Utilities;
using Utils = StringHelper;
using EndPoint = std::string;

HttpMessage Sender::makeMessage(size_t n, const std::string& body, const EndPoint& ep)
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
		//msg.addAttribute(HttpMessage::parseAttribute("fromAddr:" + myEndPoint));

		msg.addBody(body);
		if (body.size() > 0)
		{
			attrib = HttpMessage::attribute("content-length", Converter<size_t>::toString(body.size()));
			msg.addAttribute(attrib);
		}
		break;
	case 2:
		msg.clear();
		msg.addAttribute(HttpMessage::attribute("GET", "Message"));
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
// -------------- << Sends message via socket >> -----------------
void Sender::sendMessage(HttpMessage & msg, Socket & socket)
{
	std::string msgString = msg.toString();
	socket.send(msgString.size(), (Socket::byte*)msgString.c_str());
}
// -------------- << Sends File via socket >> -----------------
bool Sender::sendFile(const std::string & filename, Socket & socket, int fPort, int tport)
{	
	std::string fqname = filename;
	FileSystem::FileInfo fi(fqname);
	size_t fileSize = fi.size();
	std::string sizeString = Converter<size_t>::toString(fileSize);
	std::string toPort = Converter<int>::toString(tport);
	std::string fromPort = Converter<int>::toString(fPort);
	FileSystem::File file(fqname);
	file.open(FileSystem::File::in, FileSystem::File::binary);
	if (!file.isGood())
		return false;
	HttpMessage msg = makeMessage(1, "", "localhost::"+toPort);
	msg.addAttribute(HttpMessage::Attribute("file", filename));
	msg.addAttribute(HttpMessage::Attribute("content-length", sizeString));
	msg.addAttribute(HttpMessage::Attribute("To-Port", toPort));
	msg.addAttribute(HttpMessage::Attribute("From-Port", fromPort));
	sendMessage(msg, socket);
	const size_t BlockSize = 2048;
	Socket::byte buffer[BlockSize];
	while (true)
	{
		FileSystem::Block blk = file.getBlock(BlockSize);
		if (blk.size() == 0)
			break;
		for (size_t i = 0; i < blk.size(); ++i)
			buffer[i] = blk[i];
		socket.send(blk.size(), buffer);
		if (!file.isGood())
			break;
	}
	file.close();
	return true;
}
// -------------- << Sends Files + Dependencies via socket >> -----------------
bool Sender::sendFilesWithDeps(const std::string & fileToSend, Socket & socket, std::vector<std::string> dependencies, int fPort, int tport)
{
	std::string fqname = fileToSend;
	FileSystem::FileInfo fi(fqname);
	size_t fileSize = fi.size();
	std::string sizeString = Converter<size_t>::toString(fileSize);
	std::string toPort = Converter<int>::toString(tport);
	std::string fromPort = Converter<int>::toString(fPort);
	std::string noOfDeps = Converter<size_t>::toString(dependencies.size());
	FileSystem::File file(fqname);
	file.open(FileSystem::File::in, FileSystem::File::binary);
	if (!file.isGood())
		return false;
	HttpMessage msg = makeMessage(1, "", "localhost::"+toPort);
	msg.addAttribute(HttpMessage::Attribute("file", fileToSend));
	msg.addAttribute(HttpMessage::Attribute("content-length", sizeString));
	msg.addAttribute(HttpMessage::Attribute("Dep-Count", noOfDeps));
	msg.addAttribute(HttpMessage::Attribute("To-Port", toPort));
	msg.addAttribute(HttpMessage::Attribute("From-Port", fromPort));
	for (size_t i = 0; i < dependencies.size(); i++)
	{
		std::string iter = Converter<size_t>::toString(i);
		msg.addAttribute(HttpMessage::Attribute("dep"+iter, dependencies[i]));
	}	
	sendMessage(msg, socket);
	const size_t BlockSize = 2048;
	Socket::byte buffer[BlockSize];
	while (true)
	{
		FileSystem::Block blk = file.getBlock(BlockSize);
		if (blk.size() == 0)
			break;
		for (size_t i = 0; i < blk.size(); ++i)
			buffer[i] = blk[i];
		socket.send(blk.size(), buffer);
		if (!file.isGood())
			break;
	}
	file.close();
	return true;
}
// -------------- << Process file extraction request of client via socket >> -----------------
void Sender::getFiles(std::string filename, Socket & socket, bool ExtractWithDepsFlag, int cPort, int sPort)
{
	std::string clientPort = Converter<int>::toString(cPort);
	std::string serverPort = Converter<int>::toString(sPort);
	HttpMessage msg = makeMessage(2, "", "localhost::8080");
	msg.addAttribute(HttpMessage::Attribute("file", filename));
	msg.addAttribute(HttpMessage::Attribute("To-Port", serverPort));
	msg.addAttribute(HttpMessage::Attribute("From-Port", clientPort));
	std::string depFlag;
	if (ExtractWithDepsFlag) depFlag = "Yes";
	else
		depFlag = "No";
	msg.addAttribute(HttpMessage::Attribute("ExtractWithDepsFlag", depFlag));
	sendMessage(msg, socket);
}

#ifdef DEBUG



int main()
{
	return 0;
}

#endif // DEBUG