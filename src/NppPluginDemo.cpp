//this file is part of notepad++
//Copyright (C)2003 Don HO <donho@altern.org>
//
//This program is free software; you can redistribute it and/or
//modify it under the terms of the GNU General Public License
//as published by the Free Software Foundation; either
//version 2 of the License, or (at your option) any later version.
//
//This program is distributed in the hope that it will be useful,
//but WITHOUT ANY WARRANTY; without even the implied warranty of
//MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
//GNU General Public License for more details.
//
//You should have received a copy of the GNU General Public License
//along with this program; if not, write to the Free Software
//Foundation, Inc., 675 Mass Ave, Cambridge, MA 02139, USA.

#include <stdio.h>

#include "PluginDefinition.h"

extern FuncItem funcItem[nbFunc];
extern NppData nppData;
extern bool doCloseTag;


BOOL APIENTRY DllMain(HANDLE hModule, DWORD  reasonForCall, LPVOID /*lpReserved*/)
{
    switch (reasonForCall)
    {
      case DLL_PROCESS_ATTACH:
        pluginInit(hModule);
        break;

      case DLL_PROCESS_DETACH:
        pluginCleanUp();
        break;

      case DLL_THREAD_ATTACH:
        break;

      case DLL_THREAD_DETACH:
        break;
    }

    return TRUE;
}


extern "C" __declspec(dllexport) void setInfo(NppData notpadPlusData)
{
	nppData = notpadPlusData;
	commandMenuInit();
}

extern "C" __declspec(dllexport) const TCHAR * getName()
{
	return NPP_PLUGIN_NAME;
}

extern "C" __declspec(dllexport) FuncItem * getFuncsArray(int *nbF)
{
	*nbF = nbFunc;
	return funcItem;
}

extern "C" __declspec(dllexport) void notifEHFFillLine(int ch)
{
	__int64 currentEdit;
	::SendMessage(nppData._nppHandle, NPPM_GETCURRENTSCINTILLA, 0, (LPARAM)& currentEdit);
	HWND hCurrentEditView = (currentEdit == 0) ? nppData._scintillaMainHandle : nppData._scintillaSecondHandle;
	__int64 currentPos = __int64(::SendMessage(hCurrentEditView, SCI_GETCURRENTPOS, 0, 0));

	// Primeiro apaga o ultimo caracter ja impresso.

	int insertStringSize = 0;
	char insertString[512];
	insertString[insertStringSize++] = '\n';
	for (; insertStringSize < 157; )
	{
		insertString[insertStringSize++] = ch;
	}

	insertString[insertStringSize] = '\0';

	::SendMessage(hCurrentEditView, SCI_BEGINUNDOACTION, 0, 0);
	::SendMessage(hCurrentEditView, SCI_SETSEL, currentPos - 1ul, currentPos - 1ul);
	::SendMessage(hCurrentEditView, SCI_REPLACESEL, 0, (LPARAM)insertString);
	currentPos += (insertStringSize);
	::SendMessage(hCurrentEditView, SCI_SETSEL, currentPos, currentPos);
	insertString[0] = '\n';
	insertString[1] = '\0';
	::SendMessage(hCurrentEditView, SCI_REPLACESEL, 0, (LPARAM)insertString);
	::SendMessage(hCurrentEditView, SCI_ENDUNDOACTION, 0, 0);

}

extern "C" __declspec(dllexport) void notifCloseTag(SCNotification* notifyCode)
{

}


extern "C" __declspec(dllexport) void beNotified(SCNotification *notifyCode)
{
	switch (notifyCode->nmhdr.code) 
	{
		case NPPN_SHUTDOWN:
		{
			commandMenuCleanUp();
		}
		break;

		default:
			return;
	}
}


// Here you can process the Npp Messages 
// I will make the messages accessible little by little, according to the need of plugin development.
// Please let me know if you need to access to some messages :
// http://sourceforge.net/forum/forum.php?forum_id=482781
//
extern "C" __declspec(dllexport) LRESULT messageProc(UINT /*Message*/, WPARAM /*wParam*/, LPARAM /*lParam*/)
{/*
	if (Message == WM_MOVE)
	{
		::MessageBox(NULL, "move", "", MB_OK);
	}
*/
	return TRUE;
}

#ifdef UNICODE
extern "C" __declspec(dllexport) BOOL isUnicode()
{
    return TRUE;
}
#endif //UNICODE
