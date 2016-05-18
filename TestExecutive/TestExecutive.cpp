///////////////////////////////////////////////////////////////////////
// TestExecutive.cpp - Automated test suite for Dependency-Based	 //
//					 Remote Code Repository							 //
// ver 1.0                                                           //
// Language:    C++, Visual Studio 2015                              //
// Application: Remote Code Repository,CSE687-Object Oriented Design //
// Author:      Varun Jindal, vjindal@syr.edu                        //
///////////////////////////////////////////////////////////////////////


#include "TestExecutive.h"
#include <iostream>



void TestExecutive::Req1()
{
	std::cout << "\n\n********************* REQUIREMENT 1 ****************************" ;
	std::cout << std::endl << "================================================================================" << std::endl;
	std::cout << "Used Visual Studio 2015 and its C++ Windows console projects, as provided in the ECS computer labs." << std::endl;
}
void TestExecutive::Req2()
{
	std::cout << "\n\n********************* REQUIREMENT 2 ****************************";
	std::cout << std::endl << "================================================================================" << std::endl;
	std::cout << "Used the C++ standard library's streams for all console I/O and new and delete for all heap-based memory management" << std::endl;
}

void TestExecutive::Req3()
{
	std::cout << "\n\n********************* REQUIREMENT 3 ****************************";
	std::cout << std::endl << "================================================================================" << std::endl;
	std::cout << "Provided a Repository program that provides a module and/or package checkin process. Each checkin will support copying and providing metadata for all modules or packages in the Repository. Metadata contains a description of the module/package and a list of other modules/packages on which it depends." << std::endl;
	std::cout << std::endl << "Results displayed on server and client console" << std::endl;
}

void TestExecutive::Req4()
{
	std::cout << "\n\n********************* REQUIREMENT 4 ****************************";
	std::cout << std::endl << "================================================================================" << std::endl;
	std::cout << "User provides dependencies Manually" << std::endl;
	
}

void TestExecutive::Req5()
{
	std::cout << "\n\n********************* REQUIREMENT 5 ****************************";
	std::cout << std::endl << "================================================================================" << std::endl;
	std::cout << "Each File Check-In creates a new directory with timestamp and places the file and metadata in the directory" << std::endl;
	std::cout << std::endl << "Results can be checked in ../ServerDrive folder" << std::endl;
}

void TestExecutive::Req6()
{
	std::cout << "\n\n********************* REQUIREMENT 6 ****************************";
	std::cout << std::endl << "================================================================================" << std::endl;
	std::cout << "Each File Check-In creates a new directory with timestamp" << std::endl;
	std::cout << std::endl << "Results can be checked in ../ServerDrive folder" << std::endl;
}

void TestExecutive::Req7()
{
	std::cout << "\n\n********************* REQUIREMENT 7 ****************************";
	std::cout << std::endl << "================================================================================" << std::endl;
	std::cout << "Provided an extraction process for modules or packages with or without dependencies" << std::endl;
	std::cout << std::endl << "Results can be checked in ../ClientDrive folder" << std::endl;
}
void TestExecutive::Req8()
{
	std::cout << "\n\n********************* REQUIREMENT 8 ****************************";
	std::cout << std::endl << "================================================================================" << std::endl;
	std::cout << "Provided a message-passing communication system used to access the Repository's functionality from another process or machine." << std::endl;
	
}
void TestExecutive::Req9()
{
	std::cout << "\n\n********************* REQUIREMENT 9 ****************************";
	std::cout << std::endl << "================================================================================" << std::endl;
	std::cout << "The communication system is provided to support for passing HTTP style messages using either synchronous request/response or asynchronous one-way messaging." << std::endl;
	
}
void TestExecutive::Req10()
{
	std::cout << "\n\n********************* REQUIREMENT 10 ****************************";
	std::cout << std::endl << "================================================================================" << std::endl;
	std::cout << "The communication system supports sending and receiving streams of bytes6. Streams will be established with an initial exchange of messages." << std::endl;
	
}
void TestExecutive::Req11()
{
	std::cout << "\n\n********************* REQUIREMENT 11 ****************************";
	std::cout << std::endl << "================================================================================" << std::endl;
	std::cout << "TestExecutive provides automated test suite" << std::endl;
	
}

#ifdef TEST_EXECUTIVE


int main()
{
	std::cout << "\n\n********************* START OF TEST EXECUTIVE ****************************\n\n\n";
	
	TestExecutive::Req1();
	TestExecutive::Req2();
	TestExecutive::Req3();
	TestExecutive::Req4();
	TestExecutive::Req5();
	TestExecutive::Req6();
	TestExecutive::Req7();
	TestExecutive::Req8();
	TestExecutive::Req9();
	TestExecutive::Req10();
	TestExecutive::Req11();
	std::cout << "\n\n********************* END OF TEST EXECUTIVE ****************************";
	getchar();
}


#endif // TEST_EXECUTIVE