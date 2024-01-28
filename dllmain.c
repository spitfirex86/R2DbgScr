#include "framework.h"
#include "mod.h"


void fn_vAttachHooks( void )
{
	FHK_M_lCreateHook(&GAM_fn_WndProc, MOD_WndProc);
}

void fn_vDetachHooks( void )
{
	FHK_M_lDestroyHook(&GAM_fn_WndProc, MOD_WndProc);
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
