// Copyright Epic Games, Inc. All Rights Reserved.
/*===========================================================================
	Generated code exported from UnrealHeaderTool.
	DO NOT modify this manually! Edit the corresponding .h files instead!
===========================================================================*/

#include "UObject/ObjectMacros.h"
#include "UObject/ScriptMacros.h"

PRAGMA_DISABLE_DEPRECATION_WARNINGS
#ifdef ARENABATTLE_ABGameMode_generated_h
#error "ABGameMode.generated.h already included, missing '#pragma once' in ABGameMode.h"
#endif
#define ARENABATTLE_ABGameMode_generated_h

#define Client_Source_ArenaBattle_ABGameMode_h_15_SPARSE_DATA
#define Client_Source_ArenaBattle_ABGameMode_h_15_RPC_WRAPPERS
#define Client_Source_ArenaBattle_ABGameMode_h_15_RPC_WRAPPERS_NO_PURE_DECLS
#define Client_Source_ArenaBattle_ABGameMode_h_15_INCLASS_NO_PURE_DECLS \
private: \
	static void StaticRegisterNativesAABGameMode(); \
	friend struct Z_Construct_UClass_AABGameMode_Statics; \
public: \
	DECLARE_CLASS(AABGameMode, AGameModeBase, COMPILED_IN_FLAGS(0 | CLASS_Transient | CLASS_Config), CASTCLASS_None, TEXT("/Script/ArenaBattle"), NO_API) \
	DECLARE_SERIALIZER(AABGameMode)


#define Client_Source_ArenaBattle_ABGameMode_h_15_INCLASS \
private: \
	static void StaticRegisterNativesAABGameMode(); \
	friend struct Z_Construct_UClass_AABGameMode_Statics; \
public: \
	DECLARE_CLASS(AABGameMode, AGameModeBase, COMPILED_IN_FLAGS(0 | CLASS_Transient | CLASS_Config), CASTCLASS_None, TEXT("/Script/ArenaBattle"), NO_API) \
	DECLARE_SERIALIZER(AABGameMode)


#define Client_Source_ArenaBattle_ABGameMode_h_15_STANDARD_CONSTRUCTORS \
	/** Standard constructor, called after all reflected properties have been initialized */ \
	NO_API AABGameMode(const FObjectInitializer& ObjectInitializer); \
	DEFINE_DEFAULT_OBJECT_INITIALIZER_CONSTRUCTOR_CALL(AABGameMode) \
	DECLARE_VTABLE_PTR_HELPER_CTOR(NO_API, AABGameMode); \
DEFINE_VTABLE_PTR_HELPER_CTOR_CALLER(AABGameMode); \
private: \
	/** Private move- and copy-constructors, should never be used */ \
	NO_API AABGameMode(AABGameMode&&); \
	NO_API AABGameMode(const AABGameMode&); \
public:


#define Client_Source_ArenaBattle_ABGameMode_h_15_ENHANCED_CONSTRUCTORS \
private: \
	/** Private move- and copy-constructors, should never be used */ \
	NO_API AABGameMode(AABGameMode&&); \
	NO_API AABGameMode(const AABGameMode&); \
public: \
	DECLARE_VTABLE_PTR_HELPER_CTOR(NO_API, AABGameMode); \
DEFINE_VTABLE_PTR_HELPER_CTOR_CALLER(AABGameMode); \
	DEFINE_DEFAULT_CONSTRUCTOR_CALL(AABGameMode)


#define Client_Source_ArenaBattle_ABGameMode_h_15_PRIVATE_PROPERTY_OFFSET
#define Client_Source_ArenaBattle_ABGameMode_h_12_PROLOG
#define Client_Source_ArenaBattle_ABGameMode_h_15_GENERATED_BODY_LEGACY \
PRAGMA_DISABLE_DEPRECATION_WARNINGS \
public: \
	Client_Source_ArenaBattle_ABGameMode_h_15_PRIVATE_PROPERTY_OFFSET \
	Client_Source_ArenaBattle_ABGameMode_h_15_SPARSE_DATA \
	Client_Source_ArenaBattle_ABGameMode_h_15_RPC_WRAPPERS \
	Client_Source_ArenaBattle_ABGameMode_h_15_INCLASS \
	Client_Source_ArenaBattle_ABGameMode_h_15_STANDARD_CONSTRUCTORS \
public: \
PRAGMA_ENABLE_DEPRECATION_WARNINGS


#define Client_Source_ArenaBattle_ABGameMode_h_15_GENERATED_BODY \
PRAGMA_DISABLE_DEPRECATION_WARNINGS \
public: \
	Client_Source_ArenaBattle_ABGameMode_h_15_PRIVATE_PROPERTY_OFFSET \
	Client_Source_ArenaBattle_ABGameMode_h_15_SPARSE_DATA \
	Client_Source_ArenaBattle_ABGameMode_h_15_RPC_WRAPPERS_NO_PURE_DECLS \
	Client_Source_ArenaBattle_ABGameMode_h_15_INCLASS_NO_PURE_DECLS \
	Client_Source_ArenaBattle_ABGameMode_h_15_ENHANCED_CONSTRUCTORS \
private: \
PRAGMA_ENABLE_DEPRECATION_WARNINGS


template<> ARENABATTLE_API UClass* StaticClass<class AABGameMode>();

#undef CURRENT_FILE_ID
#define CURRENT_FILE_ID Client_Source_ArenaBattle_ABGameMode_h


PRAGMA_ENABLE_DEPRECATION_WARNINGS
