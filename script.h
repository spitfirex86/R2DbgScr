#pragma once
#include "framework.h"

#define SCRIPT(name) AI_tdstNodeInterpret name[] =

#define VALUE(val,type) {(void*)(val),0,1,(BYTE)(type)}

#define _FUNC(func) VALUE((func),AI_E_ti_Function)
#define _PROC(proc) VALUE ((proc),AI_E_ti_Procedure)
#define _ARGS(...) __VA_ARGS__

#define FUNC(func) _FUNC((func)), _ARGS
#define PROC(proc) _PROC((proc)), _ARGS

#define DSGVAR(idx) VALUE((idx),AI_E_ti_DsgVarRef)
#define PERSO(ref) VALUE((ref),AI_E_ti_PersoRef)
#define VECTOR3(x,y,z) VALUE(NULL,AI_E_ti_Vector),\
	VALUE(((tduWrapFloat){(x)}).pv,AI_E_ti_Real),\
	VALUE(((tduWrapFloat){(y)}).pv,AI_E_ti_Real),\
	VALUE(((tduWrapFloat){(z)}).pv,AI_E_ti_Real)

#define INT(val) VALUE((val),AI_E_ti_Constant)
#define REAL(val) VALUE(((tduWrapFloat){(val)}).pv,AI_E_ti_Real)
#define STRING(val) VALUE((val),AI_E_ti_String)

#define END() {(void*)(NULL),0,0,(BYTE)AI_E_ti_Unknown}

typedef enum
{
	SF_GenerateObject = 66,
	SF_CibleLaPlusProcheavecAngles = 113,
	SF_Func_GetSPOCoordinates = 152,
	SF_TEXT_DisplayText = 158,
	SF_GetPersoSighting = 163,
} tdeScriptFunction;

typedef enum
{
	SP_SPO_SetCoordinates = 412,
	SP_TEXT_SuperImposed = 443,
} tdeScriptProc;

typedef union
{
	float f;
	void *pv;
} tduWrapFloat;
