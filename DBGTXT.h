#pragma once
#include "framework.h"

#define TF_COLOR_RESET "/o0:"
#define TF_COLOR_RED "/o200:"
#define TF_COLOR_YELLOW "/o400:"

#define TF_RED(str) TF_COLOR_RED str TF_COLOR_RESET
#define TF_YELLOW(str) TF_COLOR_YELLOW str TF_COLOR_RESET


extern BOOL DBGTXT_g_bDrawFrame;
extern BOOL DBGTXT_g_bRightAlign;

void DBGTXT_vInit( void *pContext );

void DBGTXT_vSetPos( MTH_tdxReal x, MTH_tdxReal y );
void DBGTXT_vAddToPos( MTH_tdxReal x, MTH_tdxReal y );
void DBGTXT_vSetSize( MTH_tdxReal xSize );

void DBGTXT_vPrint( char *szText );
void DBGTXT_vPrintLine( char *szText );
void DBGTXT_vPrintFmtLine( const char *szFmt, ... );
