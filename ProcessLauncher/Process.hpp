#pragma once

/*
		Program Name:	Brady's Process Launcher
		Programmer:		Braydon Alan Deroy
		Created:		10/3/2017
		it's Purpose:	A program to launch multiple processes based on certain localgroups. 
							Each Local group has one or many processes to run concurrently, and runs each group serially.
							Output's process information from each groups ran processes. If any errors ocurr display them as well.
*/

#ifndef PROCESS_HPP
#define PROCESS_HPP


#include <string>
#include <array>
#include <vector>
#include <Windows.h>
#include <process.h>
#include <iomanip>
#include <memory>
#include <sstream>
#include <process.h>
#include "Constants.hpp"

#define _CRTDBG_MAP_ALLOC  
#include <stdlib.h>  
#include <crtdbg.h> 

using namespace std;


class Process {
	//types
	typedef std::vector<std::array<std::wstring, PROCESS_COMMAND_LIMIT>> processesV;

	//Class members
	int _localGroup;
	BOOL _isError;
	BOOL _isProcessFailed;
	std::wstring _processParams;
	size_t firstSpacePos;
	size_t lastSpacePos;

	//Members for Process Launch
	STARTUPINFO startInfo;
	wchar_t* _commandLine;
	processesV _programInfo;
	std::vector<PROCESS_INFORMATION> _processesInformaion;

	//output memebers
	SYSTEMTIME kTime, uTime;
	DWORD exitCode;
	FILETIME creationTime, exitTime, kernalTime, userTime;

	//error Declarations
	std::vector<std::wstring> _errorInfo;
public:
	//constructors
	Process() {}
	Process(const int&, const std::wstring&, const std::wstring&, BOOL&&);
	~Process();

	//methods
private:
	unsigned long long		GetNumberTime(FILETIME&);
	void					To_ErrorString();
	size_t					FindLastSlash(std::wstring&);
	bool					DigitCheck(const wstring&);

public:
	void					AddErrorInfo(const std::wstring&, const std::wstring&);
	void					AddProcess(const std::wstring&, const std::wstring&);
	void					LTrim(std::wstring &);
	void					RTrim(std::wstring &);
	void					RunProcess();
	void					To_String();

	BOOL					ErrorCheck() const { return _isError; }
};

inline size_t Process::FindLastSlash(wstring& parameter)
{
	return (parameter.find_last_of('\\') > parameter.find_last_of('/')) ? parameter.find_last_of('\\') : parameter.find_last_of('/');
}

/*
	Method:		LTrim
	Purpose:	To trim off spaces off the left side of a string
	Returns:	Void
	Params:		String
*/
inline void Process::LTrim(std::wstring &s)
{
	s.erase(STRING_STARTING_POSITION, (s.find_first_not_of(SPACE_DELIMITER)));
}

/*
	Method:		RTrim
	Purpose:	To trim off spaces off the Right side of a string
	Returns:	Void
	Params:		String
*/
inline void Process::RTrim(std::wstring &s)
{
	s.erase((s.find_last_not_of(SPACE_DELIMITER) + TRANSITION_DELIMITER), s.length());
}

/*
	Method:		AddErrorInfo
	Purpose:	Add an Error params based on the passed .exe path, and it's parameters
	Returns:	Void
	Params:		wstring, wstring
*/
inline void Process::AddErrorInfo(const std::wstring& exePath, const std::wstring& params)
{
	_errorInfo.push_back(exePath + L"," + params);
}

/*
Method:		DigitCheck
Purpose:	Checks if the string passed in can be 
				converted into a int without failure.
Returns:	bool
Params:		wstring
*/
inline bool Process::DigitCheck(const wstring& groupStr)
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


#endif // !PROCESS_HPP