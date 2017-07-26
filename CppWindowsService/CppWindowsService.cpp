/****************************** Module Header ******************************\
* Module Name:  CppWindowsService.cpp
* Project:      CppWindowsService
* Copyright (c) Microsoft Corporation.
* 
* The file defines the entry point of the application. According to the 
* arguments in the command line, the function installs or uninstalls or 
* starts the service by calling into different routines.
* 
* This source is subject to the Microsoft Public License.
* See http://www.microsoft.com/en-us/openness/resources/licenses.aspx#MPL.
* All other rights reserved.
* 
* THIS CODE AND INFORMATION IS PROVIDED "AS IS" WITHOUT WARRANTY OF ANY KIND, 
* EITHER EXPRESSED OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE IMPLIED 
* WARRANTIES OF MERCHANTABILITY AND/OR FITNESS FOR A PARTICULAR PURPOSE.
\***************************************************************************/

#pragma region Includes
#include <stdio.h>
#include <windows.h>
#include <shellapi.h>
#include <tchar.h>

#include "ServiceInstaller.h"
#include "ServiceBase.h"
#include "SampleService.h"
#include "resource.h"
#pragma endregion


// 
// Settings of the service
// 

// Internal name of the service
#define SERVICE_NAME             L"CppWindowsService"

// Displayed name of the service
#define SERVICE_DISPLAY_NAME     L"CppWindowsService skeleton project"

// Service start options.
#define SERVICE_START_TYPE       SERVICE_DEMAND_START

// List of service dependencies - "dep1\0dep2\0\0"
#define SERVICE_DEPENDENCIES     L""

// The name of the account under which the service should run
#define SERVICE_ACCOUNT          L"NT AUTHORITY\\LocalService"

// The password to the service account name
#define SERVICE_PASSWORD         NULL

CSampleService service(SERVICE_NAME);

long CALLBACK DlgProc(HWND hwnd, UINT message, WPARAM wParam, LPARAM);

//
//  FUNCTION: wmain(int, wchar_t *[])
//
//  PURPOSE: entrypoint for the application.
// 
//  PARAMETERS:
//    argc - number of command line arguments
//    argv - array of command line arguments
//
//  RETURN VALUE:
//    none
//
//  COMMENTS:
//    wmain() either performs the command line task, or run the service.
//
int CALLBACK WinMain(HINSTANCE hInstance, HINSTANCE, LPSTR lpCmdLine, int)
{
	bool bRun = true;
	bool bDebug = false;

	LPWSTR *szArgList;
	int argCount;

	szArgList = CommandLineToArgvW(GetCommandLine(), &argCount);
	if (szArgList) {

		if ((argCount > 1) && ((*szArgList[1] == L'-' || (*szArgList[1] == L'/'))))
		{
			if (_wcsicmp(L"install", szArgList[1] + 1) == 0)
			{
				// Install the service when the command is 
				// "-install" or "/install".
				InstallService(
					SERVICE_NAME,               // Name of service
					SERVICE_DISPLAY_NAME,       // Name to display
					SERVICE_START_TYPE,         // Service start type
					SERVICE_DEPENDENCIES,       // Dependencies
					SERVICE_ACCOUNT,            // Service running account
					SERVICE_PASSWORD            // Password of the account
					);

				bRun = false;
			}
			else if (_wcsicmp(L"remove", szArgList[1] + 1) == 0)
			{
				// Uninstall the service when the command is 
				// "-remove" or "/remove".
				UninstallService(SERVICE_NAME);

				bRun = false;
			}
			else if (_wcsicmp(L"debug", szArgList[1] + 1) == 0)
			{
				bDebug = true;
			}
		}


		LocalFree(szArgList);
	}

	if (bRun)
    {
        wprintf(L"Parameters:\n");
        wprintf(L" -install  to install the service.\n");
        wprintf(L" -remove   to remove the service.\n");
		wprintf(L" -debug    to run the service as a Windows application for debugging.\n");

		if (bDebug) {
			HWND g_hwnd = CreateDialogParam(hInstance, MAKEINTRESOURCE(ID_DLGMAIN), NULL, (DLGPROC)DlgProc, NULL);

			MSG msg;
			BOOL bRet;
			while ((bRet = GetMessage(&msg, 0, 0, 0)) != 0)
			{
				if (bRet == -1)
					return -1;
				if (!IsDialogMessage(g_hwnd, &msg))
				{
					TranslateMessage(&msg);
					DispatchMessage(&msg);
				}
			}

			return msg.wParam;
		}
		else if (!CServiceBase::Run(service))
        {
            wprintf(L"Service failed to run w/err 0x%08lx\n", GetLastError());
        }
    }

    return 0;
}

long CALLBACK DlgProc(HWND hwnd, UINT message, WPARAM wParam, LPARAM){

	switch (message){

	case WM_INITDIALOG:
		{
			SetWindowText(hwnd, SERVICE_DISPLAY_NAME);
			service.OnStart(0, NULL);
			return 1;
		}
	case WM_DESTROY:
		{
			PostQuitMessage(0);
		}
		return TRUE;
	case WM_CLOSE:
		{
			service.Stop();	
		}
		return TRUE;
	case WM_COMMAND:{
		switch (LOWORD(wParam)){
			case IDOK:
			case IDCANCEL:
				service.Stop();
				DestroyWindow(hwnd);
			}
		}
	}
	return FALSE;
}