#include "framework.h"
#include "mod.h"

WNDPROC R2_WndProc = NULL;


void fn_vAttachHooks( void )
{
	R2_WndProc = GAM_fn_WndProc;

	DetourTransactionBegin();
	DetourUpdateThread(GetCurrentThread());

	DetourAttach((PVOID *)&R2_WndProc, (PVOID)MOD_WndProc);

	DetourTransactionCommit();
}

void fn_vDetachHooks( void )
{
	DetourTransactionBegin();
	DetourUpdateThread(GetCurrentThread());

	DetourDetach((PVOID *)&R2_WndProc, (PVOID)MOD_WndProc);

	DetourTransactionCommit();
}

BOOL APIENTRY DllMain( HMODULE hModule, DWORD dwReason, LPVOID lpReserved )
{
	switch ( dwReason )
	{
	case DLL_PROCESS_ATTACH:
		fn_vAttachHooks();
		MOD_Main();
		break;

	case DLL_PROCESS_DETACH:
		fn_vDetachHooks();
		break;
	}
	return TRUE;
}
