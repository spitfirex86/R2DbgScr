#pragma once
#include "framework.h"

extern WNDPROC R2_WndProc;
extern HWND *R2_hWnd;

LRESULT CALLBACK MOD_WndProc( HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam );
void MOD_Main( void );
