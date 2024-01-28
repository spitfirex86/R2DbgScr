#pragma once
#include "framework.h"


#define SC_Script(name) AI_tdstNodeInterpret name[] =

#define SC_Value_(val,type) {.uParam.pvValue=(val),0,1,(BYTE)(type)}
#define SC_Value2_(val,type,uType) {.uParam.uType=(val),0,1,(BYTE)(type)}

#define SC_Func_(func) SC_Value2_((func),AI_E_ti_Function,ulValue)
#define SC_Proc_(proc) SC_Value2_((proc),AI_E_ti_Procedure,ulValue)
#define SC_Operator_(oper) SC_Value2_((oper), AI_E_ti_Operator,ulValue)
#define SC_Args_(...) __VA_ARGS__

#define SC_Func(func) SC_Func_(func), SC_Args_
#define SC_Proc(proc) SC_Proc_(proc), SC_Args_
#define SC_Operator(oper) SC_Operator_(oper), SC_Args_

#define SC_DsgVar(idx) SC_Value2_((idx),AI_E_ti_DsgVarRef,ucValue)
#define SC_Perso(ref) SC_Value2_((ref),AI_E_ti_PersoRef,hActor)
#define SC_PersoFromSpo(ref) SC_Perso((ref)->hLinkedObject.p_stActor)
#define SC_SuperObj(ref) SC_Value2_((ref),AI_E_ti_SuperObjectRef,hSuperObject)

#define SC_Int(val) SC_Value2_((val),AI_E_ti_Constant,lValue)
#define SC_Real(val) SC_Value2_(val,AI_E_ti_Real,xValue)
#define SC_String(val) SC_Value2_((val),AI_E_ti_String,szString)

#define SC_VectorFromVal(x,y,z) SC_Value_(NULL,AI_E_ti_Vector),SC_Real(x),SC_Real(y),SC_Real(z)

#define SC_Vector(stVec) SC_VectorFromVal((stVec).x,(stVec).y,(stVec).z)

#define SC_EndScript() {(void*)(NULL),0,0,(BYTE)AI_E_ti_EndTree}


/****************************************************************************
 * functions
 ****************************************************************************/

typedef enum tdeAIFunc
{
	ACT_CibleLaPlusProcheAvecAngles_ = 113,
	ACT_GetPersoSighting_ = 163,
}
tdeAIFunc;

#define ACT_CibleLaPlusProcheAvecAngles(lCustBit,xWeightX,xWeightXY,xWeightGlobal,VectorDirection,xCosXoY,xSinYoZ)\
	SC_Func_(ACT_CibleLaPlusProcheAvecAngles_),SC_Int(lCustBit),SC_Real(xWeightX),SC_Real(xWeightXY),SC_Real(xWeightGlobal),VectorDirection,SC_Real(xCosXoY),SC_Real(xSinYoZ)
#define ACT_GetPersoSighting()\
	SC_Func_(ACT_GetPersoSighting_)


/****************************************************************************
 * procedures
 ****************************************************************************/

typedef enum tdeAIProc
{
	Cam_ChangeLinearSpeed_ = 455,
	Cam_ChangeLinearIncreaseSpeed_ = 456,
	Cam_ChangeLinearDecreaseSpeed_ = 457,
	Cam_ChangeAngularSpeed_ = 458,
	Cam_ChangeAngularIncreaseSpeed_ = 459,
	Cam_ChangeAngularDecreaseSpeed_ = 460,
	Cam_ChangeTargetSpeed_ = 461,
	Cam_ChangeTgtPerso_ = 467,
	Cam_SetFlagNoVisibility_ = 476,
	Cam_SetFlagNoDynSpeed_ = 480,
	Cam_SetFlagNoObstacle_ = 489,
	Cam_ForcePosition_ = 496,
	Cam_Reset_ = 498,
}
tdeAIProc;

#define Cam_ChangeLinearSpeed(lTemp,lMode,xValue)\
	SC_Proc_(Cam_ChangeLinearSpeed_),SC_Int(lTemp),SC_Int(lMode),SC_Real(xValue)
#define Cam_ChangeLinearIncreaseSpeed(lTemp,lMode,xValue)\
	SC_Proc_(Cam_ChangeLinearIncreaseSpeed_),SC_Int(lTemp),SC_Int(lMode),SC_Real(xValue)
#define Cam_ChangeLinearDecreaseSpeed(lTemp,lMode,xValue)\
	SC_Proc_(Cam_ChangeLinearDecreaseSpeed_),SC_Int(lTemp),SC_Int(lMode),SC_Real(xValue)
#define Cam_ChangeAngularSpeed(lTemp,lMode,xValue)\
	SC_Proc_(Cam_ChangeAngularSpeed_),SC_Int(lTemp),SC_Int(lMode),SC_Real(xValue)
#define Cam_ChangeAngularIncreaseSpeed(lTemp,lMode,xValue)\
	SC_Proc_(Cam_ChangeAngularIncreaseSpeed_),SC_Int(lTemp),SC_Int(lMode),SC_Real(xValue)
#define Cam_ChangeAngularDecreaseSpeed(lTemp,lMode,xValue)\
	SC_Proc_(Cam_ChangeAngularDecreaseSpeed_),SC_Int(lTemp),SC_Int(lMode),SC_Real(xValue)
#define Cam_ChangeTargetSpeed(lTemp,lMode,xValue)\
	SC_Proc_(Cam_ChangeTargetSpeed_),SC_Int(lTemp),SC_Int(lMode),SC_Real(xValue)
#define Cam_ChangeTgtPerso(lTemp,lMode,Perso)\
	SC_Proc_(Cam_ChangeTgtPerso_),SC_Int(lTemp),SC_Int(lMode),Perso
#define Cam_SetFlagNoVisibility(lTemp,lValue)\
	SC_Proc_(Cam_SetFlagNoVisibility_),SC_Int(lTemp),SC_Int(lValue)
#define Cam_SetFlagNoDynSpeed(lTemp,lValue)\
	SC_Proc_(Cam_SetFlagNoDynSpeed_),SC_Int(lTemp),SC_Int(lValue)
#define Cam_SetFlagNoObstacle(lTemp,lValue)\
	SC_Proc_(Cam_SetFlagNoObstacle_),SC_Int(lTemp),SC_Int(lValue)
#define Cam_ForcePosition(lTemp,lMode,Vector)\
	SC_Proc_(Cam_ForcePosition_),SC_Int(lTemp),SC_Int(lMode),Vector
#define Cam_Reset()\
	SC_Proc_(Cam_Reset_)


#define SC_Ultra(Perso,Proc)\
	SC_Operator_(24),Perso,Proc
