#pragma once


typedef struct tdstNameTableArray
{
	char **d_szNames;
	long lNbNames;
}
tdstNameTableArray;

typedef struct tdstNameTable
{
	char *szName;

	tdstNameTableArray stComport;
	tdstNameTableArray stReflex;
}
tdstNameTable;


void fn_vReadComportNames( void );

tdstNameTable * fn_p_stGetNameTableForActor( HIE_tdstEngineObject *p_stActor );
char * fn_szGetComportNameInTable( tdstNameTable *p_stTable, int lIdx );
char * fn_szGetReflexNameInTable( tdstNameTable *p_stTable, int lIdx );
