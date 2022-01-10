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
#include "menuCmdID.h"
#include <cassert>

//
// The plugin data that Notepad++ needs
//
FuncItem funcItem[nbFunc];

//
// The data of Notepad++ that you can use in your plugin commands
//
NppData nppData;

//
// Initialize your plugin data here
// It will be called while plugin loading   
void pluginInit(HANDLE /*hModule*/)
{
}

//
// Here you can do the clean up, save the parameters (if any) for the next session
//
void pluginCleanUp()
{
}

//
// Initialization of your plugin commands
// You should fill your plugins commands here
void commandMenuInit()
{
	ShortcutKey* hyphenShKey = new ShortcutKey;
	hyphenShKey->_isAlt = true;
	hyphenShKey->_isCtrl = false;
	hyphenShKey->_isShift = false;
	hyphenShKey->_key = 0x2D; //Hyphen
	setCommand(0, TEXT("Pattern Fill Line "), ehfDoFillLine, hyphenShKey, false);

	// EHF 
	ShortcutKey* sShKey = new ShortcutKey;
	sShKey->_isAlt = true;
	sShKey->_isCtrl = false;
	sShKey->_isShift = false;
	//	sShKey->_key = 0x53; //VK_S
	sShKey->_key = 0x54; //VK_T
	setCommand(1, TEXT("Timestamp Ruller "), ehfTheTimeStamp, sShKey, false);
	// Sample:
	//==============================================================
	//:20121017175457 #--# 17:54 quarta-feira, 17 de outubro de 2012
	//==============================================================
	// Nova compilação em 2019 com UNICODE e 64bits
	//=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-==-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-
	//:20190927145312 #--# 14:53 sexta-feira, 27 de setembro de 2019
	//=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-==-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-
	//= -= -= -= -= -= -= -= -= -= -= -= -= -= -= -= -= -= -= -= -= -= -= -= -= -= -= -= -= -= -= -= -= = -= -= -= -= -= -= -= -= -= -= -= -= -= -= -= -= -= -= -= -= -= -= -= -= -= -= -= -= -= -= -= -= -= -= -= -= -= -= -= -= -= -= -= -= -= -= -= -= -= = -= -= -= -= -= -= -= -= -= -= -= -= -= -= -= -= -= -= -= -= -= -= -= -= -= -= -= -= -= -= -= -= -= -= -= -= -= -= -= -= -= -= -
	//:20190930151925 #--# 15 : 19 segunda - feira, 30 de setembro de 2019 - Plugin Developed by Elias Henrique Ferreira.
	//= -= -= -= -= -= -= -= -= -= -= -= -= -= -= -= -= -= -= -= -= -= -= -= -= -= -= -= -= -= -= -= -= = -= -= -= -= -= -= -= -= -= -= -= -= -= -= -= -= -= -= -= -= -= -= -= -= -= -= -= -= -= -= -= -= -= -= -= -= -= -= -= -= -= -= -= -= -= -= -= -= -= = -= -= -= -= -= -= -= -= -= -= -= -= -= -= -= -= -= -= -= -= -= -= -= -= -= -= -= -= -= -= -= -= -= -= -= -= -= -= -= -= -= -= -


	// Underlining the text
	ShortcutKey* sShKey2 = new ShortcutKey;
	sShKey2->_isAlt = true;
	sShKey2->_isCtrl = false;
	sShKey2->_isShift = false;
	sShKey2->_key = 0x4C; //VK_L
	setCommand(2, TEXT("Underline Pattern "), ehfUnderlineText, sShKey2, false);


}

//
// Here you can do the clean up (especially for the shortcut)
//
void commandMenuCleanUp()
{
	// Don't forget to deallocate your shortcut here
}


//
// This function help you to initialize your plugin commands
//
bool setCommand(size_t index, TCHAR *cmdName, PFUNCPLUGINCMD pFunc, ShortcutKey *sk, bool check0nInit) 
{
    if (index >= nbFunc)
        return false;

    if (!pFunc)
        return false;

    lstrcpy(funcItem[index]._itemName, cmdName);
    funcItem[index]._pFunc = pFunc;
    funcItem[index]._init2Check = check0nInit;
    funcItem[index]._pShKey = sk;

    return true;
}

const int ehf_line_lenght = 170;
const int max_pattern_lengh = 50;
unsigned char pattern[max_pattern_lengh + 1];
bool use_new_pattern = false;
int index_pattern_lenght = 0;

void ehfDoFillLine()
{
	unsigned char aux;
	int currentEdit;
	__int64 currentPos;
	int byteCounter = 0;
	unsigned char insertString[(5 * ehf_line_lenght) + 5];

	::SendMessage(nppData._nppHandle, NPPM_GETCURRENTSCINTILLA, 0, (LPARAM)& currentEdit);
	HWND curScint = (currentEdit == 0) ? nppData._scintillaMainHandle : nppData._scintillaSecondHandle;

	currentPos = __int64(::SendMessage(curScint, SCI_GETCURRENTPOS, 0, 0));

	// Lets find a pattern at the beggining of the line.
	// +§-_0
	int index_pattern_found = 0;
	int indx = 1;
	int pos;

	do
	{	
		pos = currentPos - indx;
		
		aux = (wchar_t)(::SendMessage(curScint, SCI_GETCHARAT, pos, pos));
		if (aux == TEXT('\n')) {
			if (indx > 1) {
				index_pattern_found = indx;
			} // If indx == 1 and aux == '\n' there is no pattern here.
			break;
		}
		if (indx == 1) {
			if (aux == TEXT(' ')) break;
		}
		indx++;
	} while (indx < max_pattern_lengh);

	// No new pattern found and the previous pattern must start in the next line.
	if (indx == max_pattern_lengh)
	{
		insertString[byteCounter++] = TEXT('\n');
	}

	if (index_pattern_found == indx)
	{
		// New pattern found
		use_new_pattern = true;
		index_pattern_lenght = index_pattern_found - 1;
		for (int i = 0; i < index_pattern_lenght; i++)
		{
			pos++;	// Points to the first char in the pattern
			pattern[i] = (char)(::SendMessage(curScint, SCI_GETCHARAT, pos, pos));
		}
	}
	else
	{
		// New Pattern not found
		if (use_new_pattern)
		{	// Use previous pattern

		}
		else
		{	// We must create the first pattern
			index_pattern_lenght = 1;
			pattern[0] = TEXT('-');
		}
	}

	for (int i = 0, j = 0; (j < ehf_line_lenght) && (byteCounter < sizeof(insertString)); )
	{
		// This is the character counter
		j++;	

		unsigned char first = pattern[i % index_pattern_lenght];
		/*  UTF-8 char counting
		------------------------------------------------------------------------------
		 Hex Min  | Hex Max  | Byte Sequence in Binary
		----------+----------+--------------------------------------------------------
		 00000000 | 0000007F | 0vvvvvvv
		 00000080 | 000007FF | 110vvvvv 10vvvvvv
		 00000000 | 0000FFFF | 1110vvvv 10vvvvvv 10vvvvvv
		 00000000 | 001FFFFF | 11110vvv 10vvvvvv 10vvvvvv 10vvvvvv
		 00000000 | 03FFFFFF | 111110vv 10vvvvvv 10vvvvvv 10vvvvvv 10vvvvvv
		 00000000 | 7FFFFFFF | 1111110v 10vvvvvv 10vvvvvv 10vvvvvv 10vvvvvv 10vvvvvv
		------------------------------------------------------------------------------
		*/
		insertString[byteCounter++] = first;
		i++;
		if ((first & 0xE0) == 0xC0) {
			insertString[byteCounter++] = pattern[i % index_pattern_lenght];
			i++;
			continue;
		}
		if ((first & 0xF0) == 0xE0) {
			insertString[byteCounter++] = pattern[i % index_pattern_lenght];
			i++;
			insertString[byteCounter++] = pattern[i % index_pattern_lenght];
			i++;
			continue;
		}
		if ((first & 0xF8) == 0xF0) {
			insertString[byteCounter++] = pattern[i % index_pattern_lenght];
			i++;
			insertString[byteCounter++] = pattern[i % index_pattern_lenght];
			i++;
			insertString[byteCounter++] = pattern[i % index_pattern_lenght];
			i++;
			continue;
		}
		if ((first & 0xFC) == 0xF8) {
			insertString[byteCounter++] = pattern[i % index_pattern_lenght];
			i++;
			insertString[byteCounter++] = pattern[i % index_pattern_lenght];
			i++;
			insertString[byteCounter++] = pattern[i % index_pattern_lenght];
			i++;
			insertString[byteCounter++] = pattern[i % index_pattern_lenght];
			i++;
			continue;
		}
		if ((first & 0xFE) == 0xFC) {
			insertString[byteCounter++] = pattern[i % index_pattern_lenght];
			i++;
			insertString[byteCounter++] = pattern[i % index_pattern_lenght];
			i++;
			insertString[byteCounter++] = pattern[i % index_pattern_lenght];
			i++;
			insertString[byteCounter++] = pattern[i % index_pattern_lenght];
			i++;
			insertString[byteCounter++] = pattern[i % index_pattern_lenght];
			i++;
			continue;
		}



	}

	insertString[byteCounter++] = TEXT('\n');
	insertString[byteCounter++] = TEXT('\0');

	::SendMessage(curScint, SCI_BEGINUNDOACTION, 0, 0);
	::SendMessage(curScint, SCI_SETSEL, currentPos, currentPos);
	::SendMessage(curScint, SCI_REPLACESEL, 0, (LPARAM)insertString);

//#ifdef UNICODE
//	int encoding = (int)::SendMessage(curScint, SCI_GETCODEPAGE, 0, 0);
//	char lineA[2*MAX_PATH];
//
//	memset(lineA, 0, (2 * MAX_PATH));
//
//	WideCharToMultiByte(encoding, 0, insertString, -1, lineA, insertStringSize, NULL, NULL);
//	::SendMessage(curScint, SCI_REPLACESEL, 0, (LPARAM)lineA);
//#else
//	::SendMessage(curScint, SCI_REPLACESEL, 0, (LPARAM)dateTime);
//#endif
	::SendMessage(curScint, SCI_ENDUNDOACTION, 0, 0);

}


// EHF
void ehfTheTimeStamp()
{
	TCHAR date[128];
	TCHAR time[128];
	TCHAR dateTime[256];
	TCHAR sep_sup[1024];
	TCHAR sep_inf[1024];

	SYSTEMTIME st;
	::GetLocalTime(&st);
	::GetDateFormat(LOCALE_USER_DEFAULT, DATE_LONGDATE, &st, NULL, date, 128);
	::GetTimeFormat(LOCALE_USER_DEFAULT, TIME_FORCE24HOURFORMAT | TIME_NOSECONDS, &st, NULL, time, 128);

/*
	##//=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-
	##//:20190930152315 #--# 15:23 segunda-feira, 30 de setembro de 2019 - Plugin Developed by Elias Henrique Ferreira.
	##//=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-
##══════════════════════════════════════════════════════════════════════════════════════════════════════════════════════════════════════════════════════════════════════════════════════════════════════
##  :20210618103516 #--# 10:35 AM Friday, June 18, 2021 - Timestamp Ruller and Line Pattern Separator Developed by EHF.
##0──────<010──────<020──────<030──────<040──────<050──────<060──────<070──────<080──────<090──────<100──────<110──────<120──────<130──────<140──────<150──────<160──────<170──────<180──────<190──────<

*/
	wsprintf(sep_sup, TEXT("##══════════════════════════════════════════════════════════════════════════════════════════════════════════════════════════════════════════════════════════════════════════════════════════════════════"));
	wsprintf(sep_inf, TEXT("##0──────<010──────<020──────<030──────<040──────<050──────<060──────<070──────<080──────<090──────<100──────<110──────<120──────<130──────<140──────<150──────<160──────<170──────<180──────<190──────<"));
	wsprintf(dateTime,TEXT("##  :%4.4d%2.2d%2.2d%2.2d%2.2d%2.2d #--# %s %s - Timestamp Ruller and Line Pattern Separator Developed by EHF."), st.wYear, st.wMonth, st.wDay, st.wHour, st.wMinute, st.wSecond, time, date);

	int currentEdit;
	::SendMessage(nppData._nppHandle, NPPM_GETCURRENTSCINTILLA, 0, (LPARAM)& currentEdit);
	HWND curScint = (currentEdit == 0) ? nppData._scintillaMainHandle : nppData._scintillaSecondHandle;
#ifdef UNICODE
	int encoding = (int)::SendMessage(curScint, SCI_GETCODEPAGE, 0, 0);
	char dateTimeA[256];
	char separatorA[2048];
	char nextLine[] = "\n";

	::SendMessage(curScint, SCI_REPLACESEL, 0, (LPARAM)nextLine);
	WideCharToMultiByte(encoding, 0, sep_sup, -1, separatorA, 1024, NULL, NULL);
	::SendMessage(curScint, SCI_REPLACESEL, 0, (LPARAM)separatorA);
	::SendMessage(curScint, SCI_REPLACESEL, 0, (LPARAM)nextLine);
	WideCharToMultiByte(encoding, 0, dateTime, -1, dateTimeA, 256, NULL, NULL);
	::SendMessage(curScint, SCI_REPLACESEL, 0, (LPARAM)dateTimeA);
	::SendMessage(curScint, SCI_REPLACESEL, 0, (LPARAM)nextLine);
	WideCharToMultiByte(encoding, 0, sep_inf, -1, separatorA, 1024, NULL, NULL);
	::SendMessage(curScint, SCI_REPLACESEL, 0, (LPARAM)separatorA);
	::SendMessage(curScint, SCI_REPLACESEL, 0, (LPARAM)nextLine);
#else
	::SendMessage(curScint, SCI_REPLACESEL, 0, (LPARAM)dateTime);
#endif

}


#define UTF8_LEN	6
#define MAX_CHARS	10		// Maximum chars in a pattern
#define MAX_CHARS_LEN	(UTF8_LEN*MAX_CHARS)	// 
#define MAX_UNDERLINE_BYTES	150

unsigned char charBufferPattern[MAX_CHARS_LEN] = { '-' };
size_t pattern_len = 1;		// The quantity of bytes in this pattern.
size_t pattern_chars = 1;	// The quantity of characters or columns in this pattern.

int is_UTF8(unsigned char first, int utf8_qt)
{
	if (utf8_qt > 0) {
		return utf8_qt - 1;
	}

	/*  UTF-8 char counting
	------------------------------------------------------------------------------
	 Hex Min  | Hex Max  | Byte Sequence in Binary
	----------+----------+--------------------------------------------------------
	 00000000 | 0000007F | 0vvvvvvv
	 00000080 | 000007FF | 110vvvvv 10vvvvvv
	 00000000 | 0000FFFF | 1110vvvv 10vvvvvv 10vvvvvv
	 00000000 | 001FFFFF | 11110vvv 10vvvvvv 10vvvvvv 10vvvvvv
	 00000000 | 03FFFFFF | 111110vv 10vvvvvv 10vvvvvv 10vvvvvv 10vvvvvv
	 00000000 | 7FFFFFFF | 1111110v 10vvvvvv 10vvvvvv 10vvvvvv 10vvvvvv 10vvvvvv
	------------------------------------------------------------------------------
	*/
	if ((first & 0xE0) == 0xC0) {
		return 1;
	}
	if ((first & 0xF0) == 0xE0) {
		return 2;
	}
	if ((first & 0xF8) == 0xF0) {
		return 3;
	}
	if ((first & 0xFC) == 0xF8) {
		return 4;
	}
	if ((first & 0xFE) == 0xFC) {
		return 5;
	}
	if ((first & 0x80) == 0x80) {
		/// Exception error	/// EHF_TO_DO
		assert(0);
	}
	return 0;



}

void createPatternLine(HWND curScint, __int64 currentPos, int spc, int qtPatterns, int delPatterns, bool bCR)
{
	unsigned char aux;
	int currentEdit;
	int byteCounter = 0;
	unsigned char insertString[(UTF8_LEN * ehf_line_lenght) + UTF8_LEN];
	int qtChars = qtPatterns * pattern_chars;

	if (bCR) insertString[byteCounter++] = TEXT('\r');
	insertString[byteCounter++] = TEXT('\n');


	for (int i = 0, j = 0; (j < (qtChars+spc)) && (byteCounter < sizeof(insertString)); )
	{
		// This is the character counter
		j++;

		// First we put spaces
		if (j <= spc) {
			insertString[byteCounter++] = TEXT(' ');			
			continue;
		}

		unsigned char first = charBufferPattern[i % pattern_len]; i++;
		/*  UTF-8 char counting
		------------------------------------------------------------------------------
		 Hex Min  | Hex Max  | Byte Sequence in Binary
		----------+----------+--------------------------------------------------------
		 00000000 | 0000007F | 0vvvvvvv
		 00000080 | 000007FF | 110vvvvv 10vvvvvv
		 00000000 | 0000FFFF | 1110vvvv 10vvvvvv 10vvvvvv
		 00000000 | 001FFFFF | 11110vvv 10vvvvvv 10vvvvvv 10vvvvvv
		 00000000 | 03FFFFFF | 111110vv 10vvvvvv 10vvvvvv 10vvvvvv 10vvvvvv
		 00000000 | 7FFFFFFF | 1111110v 10vvvvvv 10vvvvvv 10vvvvvv 10vvvvvv 10vvvvvv
		------------------------------------------------------------------------------
		*/
		insertString[byteCounter++] = first;
		if ((first & 0xE0) == 0xC0) {
			insertString[byteCounter++] = charBufferPattern[i % pattern_len];	i++;
			continue;
		}
		if ((first & 0xF0) == 0xE0) {
			insertString[byteCounter++] = charBufferPattern[i % pattern_len];	i++;
			insertString[byteCounter++] = charBufferPattern[i % pattern_len];	i++;
			continue;
		}
		if ((first & 0xF8) == 0xF0) {
			insertString[byteCounter++] = charBufferPattern[i % pattern_len];	i++;
			insertString[byteCounter++] = charBufferPattern[i % pattern_len];	i++;
			insertString[byteCounter++] = charBufferPattern[i % pattern_len];	i++;
			continue;
		}
		if ((first & 0xFC) == 0xF8) {
			insertString[byteCounter++] = charBufferPattern[i % pattern_len];	i++;
			insertString[byteCounter++] = charBufferPattern[i % pattern_len];	i++;
			insertString[byteCounter++] = charBufferPattern[i % pattern_len];	i++;
			insertString[byteCounter++] = charBufferPattern[i % pattern_len];	i++;
			continue;
		}
		if ((first & 0xFE) == 0xFC) {
			insertString[byteCounter++] = charBufferPattern[i % pattern_len];	i++;
			insertString[byteCounter++] = charBufferPattern[i % pattern_len];	i++;
			insertString[byteCounter++] = charBufferPattern[i % pattern_len];	i++;
			insertString[byteCounter++] = charBufferPattern[i % pattern_len];	i++;
			insertString[byteCounter++] = charBufferPattern[i % pattern_len];	i++;
			continue;
		}
	}

	if (bCR) insertString[byteCounter++] = TEXT('\r');
	insertString[byteCounter++] = TEXT('\n');
	for (int i = 0; i < spc; i++) {
		insertString[byteCounter++] = TEXT(' ');
	}
	insertString[byteCounter++] = TEXT('\0');

	::SendMessage(curScint, SCI_BEGINUNDOACTION, 0, 0);
	if (delPatterns > 0) {
		::SendMessage(curScint, SCI_DELLINERIGHT, currentPos, currentPos);
	}
	::SendMessage(curScint, SCI_SETSEL, currentPos, currentPos);
	::SendMessage(curScint, SCI_REPLACESEL, 0, (LPARAM)insertString);
	::SendMessage(curScint, SCI_ENDUNDOACTION, 0, 0);

}

void ehfUnderlineText()
{
	unsigned char aux;
	int currentEdit;
	__int64 currentPos;
	int deleteChars = 0;

	::SendMessage(nppData._nppHandle, NPPM_GETCURRENTSCINTILLA, 0, (LPARAM)&currentEdit);
	HWND curScint = (currentEdit == 0) ? nppData._scintillaMainHandle : nppData._scintillaSecondHandle;

	currentPos = __int64(::SendMessage(curScint, SCI_GETCURRENTPOS, 0, 0));

	// Calculates the text size and the initial column to be filled in the next line
	// Lets find the beggining of the line.
	int indx = 1;
	int space_count = 0;
	int frase_length = 0;
	int frase_bytes = 0;
	int pos;
	int eol_ct = 0;
	int tab_conf = 4;	// EHF_TO_DO
	int utf8_qt = 0;
	bool bCR = false;

	// Return to beginning of the line.
	pos = currentPos;
	do {
		pos--;
		aux = (wchar_t)(::SendMessage(curScint, SCI_GETCHARAT, pos, pos));
	} while (aux != TEXT('\n'));

	indx = pos - 1;
	aux = (wchar_t)(::SendMessage(curScint, SCI_GETCHARAT, indx, indx));
	if (aux == TEXT('\r')) bCR = true;

	do {
		pos++;
		if (pos == currentPos) {
			break;
		}
		aux = (wchar_t)(::SendMessage(curScint, SCI_GETCHARAT, pos, pos));

		if (aux == TEXT(' ')) {
			if (frase_length == 0) {
				space_count++;
			} else {
				frase_length++; frase_bytes++;
			}
		}
		else if (aux == TEXT('\t')) {
			if (frase_length == 0) {
				space_count = space_count + (tab_conf - space_count % tab_conf);
			} else {
				frase_length = frase_length + (tab_conf - frase_length % tab_conf);
				frase_bytes++;
			}
		}
		else if (aux != TEXT('\n') && aux != TEXT('\r')) {
			frase_bytes++;
			if ((utf8_qt = is_UTF8(aux, utf8_qt)) == 0) {
				frase_length++;
			}
		}

	} while (pos != currentPos);



	// Checks for new characters to replace the underline pattern
	// A new pattern must be after the cursor
	pos = currentPos;
	aux = (wchar_t)(::SendMessage(curScint, SCI_GETCHARAT, pos, pos));
	if (aux != TEXT('\0') && aux != TEXT('\n') && aux != TEXT('\r') && aux != TEXT(' ') && aux != TEXT('\t')) {
		utf8_qt = 0;
		pattern_len = 0;
		pattern_chars = 0;
		while (aux != TEXT('\0') && aux != TEXT('\n') && aux != TEXT('\r') && aux != TEXT(' ') && aux != TEXT('\t')) {
			charBufferPattern[pattern_len++] = aux;
			pos++;
			if ((utf8_qt = is_UTF8(aux, utf8_qt)) == 0) {
				pattern_chars++;
			}
			aux = (wchar_t)(::SendMessage(curScint, SCI_GETCHARAT, pos, pos));
		}
		deleteChars = pattern_chars;
	}
	else if (frase_bytes == 0)
	{
		// If there is no text or if there is a space in the previous character, create a default line
		createPatternLine(curScint, currentPos, 0, 100 / pattern_chars,0, bCR);
		return;
	}

	// At this point:
	// <---- space_count ----><----- frase_length(in characters) ----->
	// Call pattern writing routine with all information obtained.
	createPatternLine(curScint, currentPos, space_count, frase_length / pattern_chars, deleteChars, bCR);

	return;
}
