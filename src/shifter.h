#include "stdio.h"
#include "stdlib.h"
#include "Windowsx.h"
#include "Mmsystem.h"
#include "Winuser.h"
#include "ini.h"
#include "di.h"
#include <iostream>
#include <string>

#pragma comment(lib, "winmm.lib")  

bool didinit = false;
bool running = true;
bool listening = false;
int listenid = NULL;
bool hbutton = false;
bool dhbutton = false;
bool onoff = false;
long carptr = NULL;
long baseptr = NULL;
int cindex = 0;
long value = 0;
bool safety = false;
bool prevKeyDown0 = false;
bool prevKeyDown1 = false;

HWND hWin = NULL;
DWORD PID = NULL;

int Keys[10];
char tmpStr[255];
char tmpStr2[255];

HWND GetWindowHandle(DWORD tPID)
{
	//Get first window handle
	HWND res = FindWindow(NULL, NULL);
	DWORD mPID = NULL;
	while (res != 0)
	{
		if (!GetParent(res))
		{
			GetWindowThreadProcessId(res, &mPID);
			if (mPID == tPID)
				return res;
		}
		res = GetWindow(res, GW_HWNDNEXT);
	}
}

void Load()
{
	char* pStr, strPath[255];
	GetModuleFileName(NULL, strPath, 255);
	pStr = strrchr(strPath, '\\');
	if (pStr != NULL)
		*(++pStr) = '\0';

	sprintf(strMySystemFile, "%s%s", strPath, "\\plugins\\RealSequentialPlugin.ini");
	if (!ReadFromFile("MAIN", "ControlIndex") == 0) cindex = atoi(ReadFromFile("MAIN", "ControlIndex"));
	if (!ReadFromFile("MAIN", "Enabled") == 0) onoff = atoi(ReadFromFile("MAIN", "Enabled"));

	for (int i = 0; i < 3; i++)
	{
		sprintf(tmpStr, "Key%d", i);
		if (!ReadFromFile("MAIN", tmpStr) == 0) Keys[i] = strtoul(ReadFromFile("MAIN", tmpStr), (char**)NULL, 16);
	}
}

void Save()
{
	char* pStr, strPath[255];
	GetModuleFileName(NULL, strPath, 255);
	pStr = strrchr(strPath, '\\');
	if (pStr != NULL)
		*(++pStr) = '\0';

	sprintf(strMySystemFile, "%s%s", strPath, "\\plugins\\RealSequentialPlugin.ini");
	itoa(cindex, tmpStr2, 10);
	WriteToFile("MAIN", "ControlIndex", tmpStr2);
	itoa(onoff, tmpStr2, 10);
	WriteToFile("MAIN", "Enabled", tmpStr2);
	itoa(hbutton, tmpStr2, 10);

	for (int i = 0; i < 3; i++)
	{
		sprintf(tmpStr, "Key%d", i);
		itoa(Keys[i], tmpStr2, 16);
		WriteToFile("MAIN", tmpStr, tmpStr2);
	}
}

void Init()
{
	Load();
	InitDirectInput(hWin, cindex);
	didinit = true;
}

void PlayBlock()
{
	char exePath[MAX_PATH];
	GetModuleFileName(NULL, exePath, MAX_PATH);
	std::string directoryPath = exePath;
	std::string wavName = "\\Plugins\\RealSequential\\1.wav";
	std::string file = directoryPath.substr(0, directoryPath.find_last_of("\\/")) + wavName;
	PlaySound(TEXT(file.c_str()), NULL, SND_FILENAME);
}


DWORD WINAPI ShifterMain(LPVOID lpParam)
{
	while (hWin == 0)
		hWin = GetWindowHandle(GetCurrentProcessId());

	timeBeginPeriod(1);
	while (running == true)
	{
		if (!didinit) Init();
		PollDevices();

		carptr = *(long*)0x0165FC68;
		baseptr = *(long*)0x008EF660;

		if (onoff)
		{
			if (carptr)
			{
				if (KeyDown(Keys[2])) safety = true; // Check if the safety trigger is activated

				if (*(long*)0x165F10F)
				{
					if (KeyDown(Keys[0]) && !prevKeyDown0)
					{	
						if (*(long*)(baseptr + 0x1100) < 7) // Only upshifts if the current gear is less than sixth gear (7 in memory value)
						{
							*(long*)(baseptr + 0x1100) += 1;
							prevKeyDown0 = true; // Mark this button flag as pressed
						}						
					}
					else if (!KeyDown(Keys[0]))
					{
						prevKeyDown0 = false; // Reset the button flag to unpressed
					}
					if (KeyDown(Keys[1]) && !prevKeyDown1)
					{
						if (*(long*)(baseptr + 0x1100) > 0) // Only downshifts if the current gear is greater than reverse (0 in memory value)
						{
							if (*(long*)(baseptr + 0x1100) > 2) // If we are in a gear greater than first gear, we don´t need the safety trigger to downshift
							{
								*(long*)(baseptr + 0x1100) -= 1;
								prevKeyDown1 = true; // Mark this button flag as pressed
							}
							else if (*(long*)(baseptr + 0x1100) < 3 && safety == true) // For downshifting from first to neutral and neutral to reverse, we need the safety trigger
							{
								*(long*)(baseptr + 0x1100) -= 1;
								prevKeyDown1 = true; // Mark this button flag as pressed
							}
							else // If safety trigger is not pressed, play a metalic sound
							{
								PlayBlock();
							}
						}
					}
					else if (!KeyDown(Keys[1]))
					{
						prevKeyDown1 = false; // Reset the button flag to unpressed
					}
				}
				safety = false; // Reflag safety trigger as false
			}
		}		
		Sleep(1);
	}	
	return 1;
}

DWORD WINAPI Main2( LPVOID lpParam )
{
	return 1;
}

void StartShifter()
{
	DWORD dwThreadId, dwThrdParam = -1; 
	HANDLE hThread; 

	hThread = CreateThread( 
		NULL,                        // no security attributes 
		0,                           // use default stack size  
		ShifterMain,                  // thread function 
		&dwThrdParam,                // argument to thread function 
		0,                           // use default creation flags 
		&dwThreadId);                // returns the thread identifier		
}