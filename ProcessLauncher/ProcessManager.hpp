#pragma once

/*
	Program Name:	Brady's Process Launcher
	Programmer:		Braydon Alan Deroy
	Created:		10/3/2017
	it's Purpose:	A program to launch multiple processes based on certain localgroups.
						Each Local group has one or many processes to run concurrently, and runs each group serially.
						Output's process information from each groups ran processes. If any errors ocurr display them as well.
*/

#ifndef PROCESSMANAGER_HPP
#define PROCESSMANAGER_HPP


#include <string>
#include <iostream>
#include <fstream>
#include <map>
#include <memory>
#include <iomanip>
#include "Process.hpp"
#include <stdio.h>

#define _CRTDBG_MAP_ALLOC  
#include <stdlib.h>  
#include <crtdbg.h> 

using namespace std;

class ProcessManager {
	typedef map<int, unique_ptr<Process>> processMap;

	//members
	wstring* fileLine;
	size_t firstCommaPos;
	size_t lastCommaPos;
	unique_ptr<processMap> processList;
	int _groupNum;

	//methods
	void	OrganizeProcesses(const string*&);
	bool	GroupCheck(const wstring&&);

public:
	//constructors
	ProcessManager() {}
	ProcessManager(const string*&);
	~ProcessManager();

	//methods
	void	RunProcesses();
	void	To_String();
};

/*	
	Method:		RunProcesses
	Purpose:	Call each Local group Process to run it's set of processes
	Returns:	Void
	Params:		None
*/
inline void ProcessManager::RunProcesses()
{
	for (processMap::iterator it = processList->begin(); it != processList->end(); ++it)
		if(!it->second->ErrorCheck())
			it->second->RunProcess();	
}

/*
	Method:		GroupCheck
	Purpose:	Checks the first substr for Local group Number. It does 
					nessesary checks to determine it is a valid Integer.
	Returns:	bool	
	Params:		wstring
*/
inline bool ProcessManager::GroupCheck(const wstring&& groupStr)
{
	if (groupStr.empty())
		return false;

	for (size_t i = 0; i < groupStr.length(); ++i)
	{
		if (!isdigit(groupStr[i]))
			return false;
	}
	return true;
}

#endif // !PROCESSMANAGER_HPP