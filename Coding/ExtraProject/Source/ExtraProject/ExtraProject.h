// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

//#include "CoreMinimal.h"
#include "EngineMinimal.h"

DECLARE_LOG_CATEGORY_EXTERN(ExtraProject, Log, All);
#define MYLOG_CALLINFO (FString(__FUNCTION__) + TEXT("(") + FString::FromInt(__LINE__) + TEXT(")"))
#define MYLOG_S(Verbosity) UE_LOG(ExtraProject, Verbosity, TEXT("%s"), *MYLOG_CALLINFO)
#define MYLOG(Verbosity, Format, ...) UE_LOG(ExtraProject, Verbosity, TEXT("%s %s"), *MYLOG_CALLINFO, *FString::Printf(Format, ##__VA_ARGS__))

#define MYCHECK(Expr, ...) {if(!(Expr)) {MYLOG(Error, TEXT("ASSERTION : %s"), TEXT("'"#Expr"'")); return __VA_ARGS__;}}

