/////////////////////////////////////////////////////////////////////////////
// Receiver.cpp - This package handles operations related to do once it    //
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


#include "Receiver.h"
#include "../HttpMessage/HttpMessage.h"
#include "../Sockets/Sockets.h"
#include "../FileSystem/FileSystem.h"
#include "../Logger/Cpp11-BlockingQueue.h"
#include "../Logger/Logger.h"
#include "../Utilities/Utilities.h"
#include <string>
#include <iostream>
#include<Windows.h>
#include<time.h>
#include "../XmlWriter/XmlWriter.h"
#include "../XmlParser/XmlParser.h"
#include "../XmlParser/xmldocument.h"
#include "../XmlParser/XmlElement.h"



using Show = StaticLogger<1>;
using namespace Utilities;
using namespace XmlProcessing;
using sPtr = std::shared_ptr < AbstractXmlElement >;

// --------------------------- << Overloaded Operator >> ------------------
void Receiver::operator()(Socket socket)
{
	while (true)
	{
		HttpMessage msg = readMessage(socket);
		if (connectionClosed_ || msg.bodyString() == "quit")
		{
			Show::write("\n\n  clienthandler thread is terminating");
			break;
		}
		msgQ_.enQ(msg);
	}
}

// -------------------- << Returns Server or Client >> --------------
std::string Receiver::getComponent()
{
	return component;
}

// ------------------------- << Reads received Messages and does further processing >> -----------------
HttpMessage Receiver::readMessage(Socket & socket)
{	connectionClosed_ = false;
	HttpMessage msg;	// read message attributes
	while (true)
	{		std::string attribString = socket.recvString('\n');
		if (attribString.size() > 1)		{			HttpMessage::Attribute attrib = HttpMessage::parseAttribute(attribString);
			msg.addAttribute(attrib);		}
		else
			break;
	}	// If client is done, connection breaks and recvString returns empty string
	if (msg.attributes().size() == 0)
	{		connectionClosed_ = true;
		return msg;	}	// read body if POST - all messages in this demo are POSTs
	if (msg.attributes()[0].first == "POST")
	{		// is this a file message?
		std::string filename = msg.findValue("file");
		if (filename != "")
		{			size_t contentSize;
			std::string sizeString = msg.findValue("content-length");
			if (sizeString != "")
				contentSize = Converter<size_t>::toValue(sizeString);
			else
				return msg;
			readFile(filename, contentSize, socket);
			std::string depCount = msg.findValue("Dep-Count");
			int dCount = Converter<int>::toValue(depCount);
			if (dCount > 0)
				createDeps(filename, socket, msg);			}
		if (filename != "")
		{			// construct message body
			msg.removeAttribute("content-length");
			std::string bodyString = "<file>" + filename + "</file>";
			std::string sizeString = Converter<size_t>::toString(bodyString.size());
			msg.addAttribute(HttpMessage::Attribute("content-length", sizeString));
			msg.addBody(bodyString);		}
		else
		{			// read message body
			size_t numBytes = 0;
			size_t pos = msg.findAttribute("content-length");
			if (pos < msg.attributes().size())
			{				numBytes = Converter<size_t>::toValue(msg.attributes()[pos].second);
				Socket::byte* buffer = new Socket::byte[numBytes + 1];
				socket.recv(numBytes, buffer);
				buffer[numBytes] = '\0';
				std::string msgBody(buffer);
				msg.addBody(msgBody);
				delete[] buffer;
			}		}	}	return msg;
}

// ------------------- << Helper function to create directory >> ----------------
std::wstring s2ws(const std::string& s)
{
	int len;
	int slength = (int)s.length() + 1;
	len = MultiByteToWideChar(CP_ACP, 0, s.c_str(), slength, 0, 0);
	wchar_t* buf = new wchar_t[len];
	MultiByteToWideChar(CP_ACP, 0, s.c_str(), slength, buf, len);
	std::wstring r(buf);
	delete[] buf;
	return r;
}

// ----------------------- << Reads file and does further processing >> -------------------
bool Receiver::readFile(const std::string & filename, size_t fileSize, Socket & socket)
{
	std::string onlyFileName = FileSystem::Path::getName(filename, true);
	std::string fqname;	
	if (getComponent() == "Server") {
		std::wstring stemp = s2ws("./ServerDrive/" + onlyFileName + "_" + getTimeStamp());
		LPCWSTR result = stemp.c_str();
		CreateDirectory(result, NULL);
		fqname = "./ServerDrive/" + onlyFileName + "_" + getTimeStamp()+"/" + onlyFileName;
	}
	else
		fqname = "./ClientDrive/" + onlyFileName;
	FileSystem::File file(fqname);
	file.open(FileSystem::File::out, FileSystem::File::binary);
	if (!file.isGood())
	{		/*
		* This error handling is incomplete.  The client will continue
		* to send bytes, but if the file can't be opened, then the server
		* doesn't gracefully collect and dump them as it should.  That's
		* an exercise left for students.
		*/
		Show::write("\n\n  can't open file " + fqname);
		return false;
	}
	const size_t BlockSize = 2048;
	Socket::byte buffer[BlockSize];
	size_t bytesToRead;
	while (true)
	{
		if (fileSize > BlockSize)
			bytesToRead = BlockSize;
		else
			bytesToRead = fileSize;
		socket.recv(bytesToRead, buffer);
		FileSystem::Block blk;
		for (size_t i = 0; i < bytesToRead; ++i)
			blk.push_back(buffer[i]);
		file.putBlock(blk);
		if (fileSize < BlockSize)
			break;
		fileSize -= BlockSize;
	}
	file.close();
	return true;
}
//-------------------------- << Creates dependency xml in the directory >> -------------------------------
void Receiver::createDeps(const std::string & filename, Socket & socket, HttpMessage msg)
{
	std::vector<std::string> deps;
	size_t noOfDeps;
	std::string dep_count = msg.findValue("Dep-Count");
	if (dep_count != "")
		noOfDeps = Converter<size_t>::toValue(dep_count);
	for (size_t i = 0; i < noOfDeps; i++) {
		std::string iter = Converter<size_t>::toString(i);
		std::string depFile = msg.findValue("dep" + iter);
		deps.push_back(depFile);
	}	
	sPtr root = makeTaggedElement("Dependencies");
	for (size_t j = 0; j < deps.size(); j++) 
	{
		sPtr child = makeTaggedElement("dep");
		child->addChild(makeTextElement(deps[j]));
		root->addChild(child);
	}
	sPtr docEl = makeDocElement(root);
	std::string contents = docEl->toString();
	std::string onlyFileName = FileSystem::Path::getName(filename, true);
	std::vector<std::string> dirs = FileSystem::Directory::getDirectories("./ServerDrive", onlyFileName + "*");
	std::string latest_dir = getLatestDir(dirs, onlyFileName);
	std::ofstream out("./ServerDrive/" + latest_dir + "/" + "dep.xml");
	if (out.good())
	{
		out << contents;
		out.close();
	}
}
// ------------------ << Generates Time Stamp >> ------------------
std::string Receiver::getTimeStamp()
{
	time_t now;
	char the_date[20];
	struct tm  tstruct;
	the_date[0] = '\0';
	now = time(NULL);
	localtime_s(&tstruct, &now);
	if (now != -1)
	{
		strftime(the_date, 20, "%m%d%Y%I%M%S", &tstruct);
	}
	return std::string(the_date);
}
// ------------ << Returns latest Directory >> ------------------------
std::string Receiver::getLatestDir(std::vector<std::string> dirs, std::string fileToSend)
{
	return dirs[dirs.size() - 1];
}

#ifdef DEBUG

int main()
{
	return 0;
}
#endif // DEBUG