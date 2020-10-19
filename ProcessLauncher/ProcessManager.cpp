/*
	Program Name:	Brady's Process Launcher
	Programmer:		Braydon Alan Deroy
	Created:		10/3/2017
	it's Purpose:	A program to launch multiple processes based on certain localgroups.
						Each Local group has one or many processes to run concurrently, and runs each group serially.
						Output's process information from each groups ran processes.
*/

#include "ProcessManager.hpp"

#define _CRTDBG_MAP_ALLOC  
#include <stdlib.h>  
#include <crtdbg.h> 

/*
	Method:		Constructor
	Purpose:	Setup class members 
	Returns:	Void	
	Param:		String
*/
ProcessManager::ProcessManager(const string*& filePath)
{
	processList.reset(new processMap());
	fileLine = new wstring();
	OrganizeProcesses(filePath);

	delete filePath;
	filePath = nullptr;
}

/*
	Method:		OrganizeProcesses
	Purpose:	Read in each line of a given file. Substring each line using ',' delimiter to obtain	
					the integer, program name, programs parameters
	Returns:	Void
	Params:		String
	
	****************
	******NOTE******
	****************
	local Group: '-1' is assigned for Error Reporting

*/
void ProcessManager::OrganizeProcesses(const string*& filePath)
{
	ifstream file(*filePath);
	if (file.good())
	{
		string line;
		while (getline(file, line))
		{
			try 
			{

				//Assign the string to a wstring
				fileLine->assign(line.begin(), line.end());
				firstCommaPos = fileLine->find_first_of(COMMA_DELIMITER);
				lastCommaPos = fileLine->find_last_of(COMMA_DELIMITER);
			
				//check to see if the a localgroup exists and is convertable to a number.
				_groupNum = (this->GroupCheck(fileLine->substr(STRING_STARTING_POSITION, firstCommaPos)) && firstCommaPos != fileLine->npos)
																			? stoi(fileLine->substr(STRING_STARTING_POSITION, firstCommaPos)) : -1;
			}
			catch (...)
			{
				_groupNum = -1;
			}

			if (!processList->empty() && processList->find(_groupNum) != processList->end())
			{
				if (_groupNum != -1)
				{
					(*processList)[_groupNum]->AddProcess(fileLine->substr((firstCommaPos + TRANSITION_DELIMITER), (lastCommaPos - (firstCommaPos + TRANSITION_DELIMITER))),
						fileLine->substr((lastCommaPos + TRANSITION_DELIMITER), fileLine->length()));
					continue;
				}

				(*processList)[_groupNum]->AddErrorInfo(fileLine->substr(STRING_STARTING_POSITION, fileLine->length()), L"");
			}
			else
			{
				(*processList)[_groupNum] = unique_ptr<Process>(new Process(_groupNum,
					fileLine->substr((firstCommaPos + TRANSITION_DELIMITER), (lastCommaPos - (firstCommaPos + TRANSITION_DELIMITER))),
					fileLine->substr((lastCommaPos + TRANSITION_DELIMITER), fileLine->length()), (_groupNum == -1) ? true : false ));
			}
		}
		file.clear();
		file.close();
		this->RunProcesses();
		this->To_String();
	}
	else
		cout << "Provide a Valid File Path.\n" << endl;
}


/*
	Deconstructor
	Clean up pointers
*/
ProcessManager::~ProcessManager()
{
	delete fileLine;
	fileLine = nullptr;
}

/*
	Method:		To_String()
	Purpose:	To display the output of the processes that ran through
					the program.
	Return:		Void
	Params:		None;
*/
void ProcessManager::To_String()
{
	cout << setw(70) << setfill(' ') << "------------------------------" << endl;
	cout << setw(70) << setfill(' ') << "---Brady's Process Launcher---" << endl;
	cout << setw(70) << setfill(' ') << "______________________________\n" << endl;

	printf("\t%s", "Local Group");
	printf("\t%s", "Kernal Time");
	printf("\t%s", "User Time");
	printf("\t%s", "Exit Code");
	printf("\t%s", "Program Name");
	printf("\t%s\n", "Command Parameters");
	cout << "\t" << setw(100) << setfill('_') << "" << endl;

	for (processMap::iterator it = processList->begin(); it != processList->end(); ++it)
		it->second->To_String();
}


int main(int argc, char* argv[])
{
	_CrtSetDbgFlag(_CRTDBG_ALLOC_MEM_DF | _CRTDBG_LEAK_CHECK_DF);

	if (argc < 2)
	{
		cout << "No File Argument Provieded. EX: ....ProcessLauncher.exe FilePath/FileName.txt" << endl;
		return EXIT_FAILURE;
	}

	const string *it = new string(argv[1]);
	unique_ptr<ProcessManager> process(new ProcessManager(it));

	process.reset();
	_CrtDumpMemoryLeaks();
}