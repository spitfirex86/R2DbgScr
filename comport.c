#include "framework.h"
#include "comport.h"


char const g_szComportNamesPath[] = ".\\Mods\\ComportNames.tbl";


tdstNameTable *g_d_stNameTable = NULL;
long g_lNbNameTable = 0;


tdstNameTable * fn_p_stGetNameTable( char *szName )
{
	for ( int i = 0; i < g_lNbNameTable; ++i )
	{
		tdstNameTable *pTable = &g_d_stNameTable[i];
		if ( !_stricmp(pTable->szName, szName) )
			return pTable;
	}

	return NULL;
}

tdstNameTable * fn_p_stGetOrCreateNameTable( char *szName )
{
	tdstNameTable *pTable = fn_p_stGetNameTable(szName);
	if ( !pTable )
	{
		tdstNameTable *pNew = realloc(g_d_stNameTable, sizeof(tdstNameTable) * (g_lNbNameTable + 1));
		if ( !pNew )
			return NULL;

		g_d_stNameTable = pNew;
		pTable = &g_d_stNameTable[g_lNbNameTable++];

		ZeroMemory(pTable, sizeof(tdstNameTable));
		pTable->szName = malloc(strlen(szName) + 1);
		strcpy(pTable->szName, szName);
	}

	return pTable;
}

tdstNameTable * fn_p_stGetNameTableForActor( HIE_tdstEngineObject *p_stActor )
{
	char * szModelName = HIE_fn_szGetModelTypeName(HIE_M_lActorGetModelType(p_stActor));
	return (szModelName)
		? fn_p_stGetNameTable(szModelName)
		: NULL;
}

char * fn_szGetComportNameInTable( tdstNameTable *p_stTable, int lIdx )
{
	if ( !p_stTable || lIdx >= p_stTable->stComport.lNbNames || lIdx < 0 )
		return NULL;

	return p_stTable->stComport.d_szNames[lIdx];
}

char * fn_szGetReflexNameInTable( tdstNameTable *p_stTable, int lIdx )
{
	if ( !p_stTable || lIdx >= p_stTable->stReflex.lNbNames || lIdx < 0 )
		return NULL;

	return p_stTable->stReflex.d_szNames[lIdx];
}


BOOL fn_bReadComportNamesSection( FILE *hFile, tdstNameTableArray *p_stArray )
{
	char szBuffer[256];

	while ( !feof(hFile) )
	{
		if ( fscanf(hFile, "%255s ", szBuffer) != 1 )
			return FALSE;

		if ( szBuffer[0] == '}' )
			return TRUE;

		char **pNew = realloc(p_stArray->d_szNames, sizeof(char *) * (p_stArray->lNbNames + 1));
		if ( !pNew )
			return FALSE;

		p_stArray->d_szNames = pNew;
		char **pName = &p_stArray->d_szNames[p_stArray->lNbNames++];

		*pName = malloc(strlen(szBuffer) + 1);
		strcpy(*pName, szBuffer);
	}

	return FALSE;
}

void fn_vReadComportNames( void )
{
	char szSection[32];
	char szBuffer[256];
	BOOL bError = FALSE;

	FILE *hFile = fopen(g_szComportNamesPath, "r");
	if ( !hFile )
		return;

	while ( !feof(hFile) && !bError )
	{
		if ( fscanf(hFile, "{ %31[^:]:%255[^\n]\n", szSection, szBuffer) != 2 )
		{
			bError = TRUE;
			break;
		}

		char *pTrailing = strchr(szBuffer, ' ');
		if ( pTrailing ) *pTrailing = '\0';

		if ( !_stricmp(szSection, "Comport") )
		{
			tdstNameTable *p_stTable = fn_p_stGetOrCreateNameTable(szBuffer);
			bError = !fn_bReadComportNamesSection(hFile, &p_stTable->stComport);
		}
		else if ( !_stricmp(szSection, "Reflex") )
		{
			tdstNameTable *p_stTable = fn_p_stGetOrCreateNameTable(szBuffer);
			bError = !fn_bReadComportNamesSection(hFile, &p_stTable->stReflex);
		}
		else bError = TRUE;
	}

	fclose(hFile);

	if ( bError )
	{
		MessageBoxA(NULL, "Error parsing ComportNames.tbl !", "R2DbgScr", MB_ICONEXCLAMATION | MB_OK);
	}
}
