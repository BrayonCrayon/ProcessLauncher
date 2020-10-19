#include "Process.hpp"
#include <iostream>

/*
		Program Name:	Brady's Process Launcher
		Programmer:		Braydon Alan Deroy
		Created:		10/3/2017
		it's Purpose:	A program to launch multiple processes based on certain localgroups.
							Each Local group has one or many processes to run concurrently, and runs each group serially.
							Output's process information from each groups ran processes. If any errors ocurr display them as well.
*/
using namespace std;

/*
	Method:		Constructor
	Purpose:	Setup members and check if input contains invalid input. If so then
					add parameters to Errorinformation container.
	Return:		None
	Params:		int, wstring, wstring
*/
Process::Process(const int& groupNum, const std::wstring& exeName, const std::wstring& params, BOOL&& error) : _isError(error)
{
	_localGroup = groupNum;
	_commandLine = new wchar_t[CP_MAX_COMMANDLINE];
	(!_isError) ? AddProcess(exeName, params) : AddErrorInfo(exeName, params);
}

/*
	Method:		Deconstructor
	Purpose:	Clean up variables to avoid memory leaks!!!
	Returns:	Void
	Params:		None
*/
Process::~Process()
{
	for (size_t i = 0; i < _programInfo.size(); ++i)
	{
		for (size_t k = 0; k < PROCESS_COMMAND_LIMIT; ++k)
			(_programInfo)[i][k].clear();
	}

	_programInfo.clear();
	_processParams.clear();
	_processesInformaion.clear();
	_processesInformaion.~vector();
	_errorInfo.clear();
	delete _commandLine;
}

/*
	Method:		AddProcess
	Purpose:	To add another process based on the same group, 
						with provided validation and space removal
	Params:		String, String
	Returns:	Void
*/
void Process::AddProcess(const std::wstring& procPath, const std::wstring& params)
{
	//add process information for each process added
	_processesInformaion.push_back(PROCESS_INFORMATION{ 0 });

	//trim the remain white space befor and after the exit and wait params
	_processParams = params;
	if(_processParams.find_first_not_of(SPACE_DELIMITER) != 0)
		LTrim(_processParams);

	if (_processParams.find_last_not_of(SPACE_DELIMITER) != (_processParams.length() - TRANSITION_DELIMITER))
		RTrim(_processParams);

	//get space position between params and substring them into an array.
	firstSpacePos = _processParams.find_first_of(SPACE_DELIMITER);
	lastSpacePos = _processParams.find_last_of(SPACE_DELIMITER);

	if (firstSpacePos != _processParams.npos)
		_programInfo.push_back(std::array<std::wstring, PROCESS_COMMAND_LIMIT>() =
	{ procPath,  _processParams.substr(STRING_STARTING_POSITION, firstSpacePos), _processParams.substr((lastSpacePos + TRANSITION_DELIMITER), _processParams.length()) });
	else
		_programInfo.push_back(std::array<std::wstring, PROCESS_COMMAND_LIMIT>() = { procPath, _processParams });
}

/*
	Method:		RunProcess
	Purpose:	To run one or many processes recorded in a member container with it's assosiated 
					container of process_information structures.
	Params:		None
	Returns:	Void
*/
void Process::RunProcess()
{
	//Start running processes based on group. 
	startInfo = { 0 };
	startInfo.cb = sizeof(STARTUPINFO);

	for (size_t i = 0; i < _programInfo.size();)
	{
		try {

			wcsncpy_s(_commandLine, CP_MAX_COMMANDLINE, (_programInfo[i][0] + L" " + _programInfo[i][1] + L" " + _programInfo[i][2]).c_str(),
														(_programInfo[i][0] + L" " + _programInfo[i][1] + L" " + _programInfo[i][2]).size() + 1);

			//if process failed remove it from the process group and log the Error 
			if (!CreateProcess(nullptr, _commandLine, NULL, NULL, false, CREATE_NEW_CONSOLE, NULL, NULL, &startInfo, &_processesInformaion[i]))
			{
				TerminateProcess(_processesInformaion[i].hProcess, 0);
				_errorInfo.push_back(_commandLine);
				_programInfo.erase(_programInfo.begin() + i);
				CloseHandle(_processesInformaion[i].hProcess);
				CloseHandle(_processesInformaion[i].hThread);
				_processesInformaion.erase(_processesInformaion.end() - TRANSITION_DELIMITER);
			}
			else
				++i;
		}
		catch (bad_alloc&)
		{
			TerminateProcess(_processesInformaion[i].hProcess, 0);
			_errorInfo.push_back(_commandLine);
			_programInfo.erase(_programInfo.begin() + i);
			CloseHandle(_processesInformaion[i].hProcess);
			CloseHandle(_processesInformaion[i].hThread);
			_processesInformaion.erase(_processesInformaion.end() - TRANSITION_DELIMITER);
		}
	}

	for (size_t  i = 0; i < _processesInformaion.size(); ++i)
		WaitForSingleObject(_processesInformaion[i].hProcess, INFINITE);
}

/*
	Method:		To_String
	Purpose:	To output the localgroups ran process information with time 
					converted so that the user can understand.
	Returns:	Void
	Params:		None
*/
void Process::To_String()
{
	//display the output of the ran processes.
	if (!_isError)
	{
		for (size_t i = 0; i < _processesInformaion.size(); ++i)
		{
			GetExitCodeProcess(_processesInformaion[i].hProcess, &exitCode);
			GetProcessTimes(_processesInformaion[i].hProcess, &creationTime, &exitTime, &kernalTime, &userTime);
			FileTimeToSystemTime(&kernalTime, &kTime);
			FileTimeToSystemTime(&userTime, &uTime);

			if ( !this->DigitCheck(_programInfo[i][1]) || exitCode != (_programInfo[i][1].empty() ? 0 : stoi(_programInfo[i][1])) )
			{
				_errorInfo.push_back(_programInfo[i][0] + L" " + _programInfo[i][1] + L" " + _programInfo[i][2]);
				CloseHandle(_processesInformaion[i].hProcess);
				CloseHandle(_processesInformaion[i].hThread);
				continue;
			}

			cout << "\t" << _localGroup;
			cout << "\t\t" << kTime.wHour << ":" << kTime.wMinute << ":" << kTime.wSecond << ":" << kTime.wMilliseconds << SPACE_DELIMITER;
			cout << "\t" << (GetNumberTime(exitTime) - GetNumberTime(creationTime)) / DIVID_DELIMITER << SPACE_DELIMITER;
			cout << "\t\t" << exitCode;
			wcout << "\t\t" << _programInfo[i][0].substr(this->FindLastSlash(_programInfo[i][0]) + TRANSITION_DELIMITER, _programInfo[i][0].length());
			cout << "\t\t" << string(_programInfo[i][1].begin(), _programInfo[i][1].end());
			cout << SPACE_DELIMITER << string(_programInfo[i][2].begin(), _programInfo[i][2].end()) << endl;

			CloseHandle(_processesInformaion[i].hProcess);
			CloseHandle(_processesInformaion[i].hThread);
		}

		if (!_errorInfo.empty())
			this->To_ErrorString();
	}
	else
		this->To_ErrorString();
}

/*
	Method:		GetNumberTime
	Purpose:	To convert a fileTime to a number that a user can understand. As well to remove code clutterness.
	Returns:	unsigned long long
	Params:		FILETIME
*/
unsigned long long Process::GetNumberTime(FILETIME& time)
{
	return *reinterpret_cast<unsigned long long*>(&time);
}

/*
	Method:		To_ErrorString
	Purpose:	To output info on input errors from error_info container 
	Returns:	Void
	Params:		None
*/
void Process::To_ErrorString()
{
	//Get params of error info and output the error errorStr 
	for (size_t k = 0; k < _errorInfo.size(); ++k)
	{
		wcout << "\t\t" << "****Error Information-" << "\t" << "Process Failure with Params: '" << 
			_errorInfo[k].substr(this->FindLastSlash(_errorInfo[k]) + TRANSITION_DELIMITER, _errorInfo[k].length()) << "' ****" << endl;
	}
}