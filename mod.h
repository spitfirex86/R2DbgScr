#pragma once
#include "framework.h"

extern WNDPROC R2_WndProc;

LRESULT CALLBACK MOD_WndProc( HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam );
void MOD_Main( void );
