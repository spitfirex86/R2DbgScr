#include "framework.h"
#include "mod.h"

void fn_vAttachHooks( void )
{
	DetourTransactionBegin();
	DetourUpdateThread(GetCurrentThread());

	DetourAttach((PVOID *)&R2_WndProc, (PVOID)MOD_WndProc);
	DetourAttach((PVOID *)&JFFTXT_vAffiche, (PVOID)MOD_JFFTXT_vAffiche);

	DetourTransactionCommit();
}

void fn_vDetachHooks( void )
{
	DetourTransactionBegin();
	DetourUpdateThread(GetCurrentThread());

	DetourDetach((PVOID *)&R2_WndProc, (PVOID)MOD_WndProc);
	DetourDetach((PVOID *)&JFFTXT_vAffiche, (PVOID)MOD_JFFTXT_vAffiche);

	DetourTransactionCommit();
}

BOOL APIENTRY DllMain( HMODULE hModule, DWORD dwReason, LPVOID lpReserved )
{
	switch ( dwReason )
	{
	case DLL_PROCESS_ATTACH:
		fn_vAttachHooks();
		break;

	case DLL_PROCESS_DETACH:
		fn_vDetachHooks();
		break;
	}
	return TRUE;
}
