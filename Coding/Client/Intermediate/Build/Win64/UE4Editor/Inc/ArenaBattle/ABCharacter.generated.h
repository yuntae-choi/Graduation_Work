// Copyright Epic Games, Inc. All Rights Reserved.
/*===========================================================================
	Generated code exported from UnrealHeaderTool.
	DO NOT modify this manually! Edit the corresponding .h files instead!
===========================================================================*/

#include "UObject/ObjectMacros.h"
#include "UObject/ScriptMacros.h"

PRAGMA_DISABLE_DEPRECATION_WARNINGS
#ifdef ARENABATTLE_ABCharacter_generated_h
#error "ABCharacter.generated.h already included, missing '#pragma once' in ABCharacter.h"
#endif
#define ARENABATTLE_ABCharacter_generated_h

#define Client_Source_ArenaBattle_ABCharacter_h_12_SPARSE_DATA
#define Client_Source_ArenaBattle_ABCharacter_h_12_RPC_WRAPPERS
#define Client_Source_ArenaBattle_ABCharacter_h_12_RPC_WRAPPERS_NO_PURE_DECLS
#define Client_Source_ArenaBattle_ABCharacter_h_12_INCLASS_NO_PURE_DECLS \
private: \
	static void StaticRegisterNativesAABCharacter(); \
	friend struct Z_Construct_UClass_AABCharacter_Statics; \
public: \
	DECLARE_CLASS(AABCharacter, ACharacter, COMPILED_IN_FLAGS(0 | CLASS_Config), CASTCLASS_None, TEXT("/Script/ArenaBattle"), NO_API) \
	DECLARE_SERIALIZER(AABCharacter)


#define Client_Source_ArenaBattle_ABCharacter_h_12_INCLASS \
private: \
	static void StaticRegisterNativesAABCharacter(); \
	friend struct Z_Construct_UClass_AABCharacter_Statics; \
public: \
	DECLARE_CLASS(AABCharacter, ACharacter, COMPILED_IN_FLAGS(0 | CLASS_Config), CASTCLASS_None, TEXT("/Script/ArenaBattle"), NO_API) \
	DECLARE_SERIALIZER(AABCharacter)


#define Client_Source_ArenaBattle_ABCharacter_h_12_STANDARD_CONSTRUCTORS \
	/** Standard constructor, called after all reflected properties have been initialized */ \
	NO_API AABCharacter(const FObjectInitializer& ObjectInitializer); \
	DEFINE_DEFAULT_OBJECT_INITIALIZER_CONSTRUCTOR_CALL(AABCharacter) \
	DECLARE_VTABLE_PTR_HELPER_CTOR(NO_API, AABCharacter); \
DEFINE_VTABLE_PTR_HELPER_CTOR_CALLER(AABCharacter); \
private: \
	/** Private move- and copy-constructors, should never be used */ \
	NO_API AABCharacter(AABCharacter&&); \
	NO_API AABCharacter(const AABCharacter&); \
public:


#define Client_Source_ArenaBattle_ABCharacter_h_12_ENHANCED_CONSTRUCTORS \
private: \
	/** Private move- and copy-constructors, should never be used */ \
	NO_API AABCharacter(AABCharacter&&); \
	NO_API AABCharacter(const AABCharacter&); \
public: \
	DECLARE_VTABLE_PTR_HELPER_CTOR(NO_API, AABCharacter); \
DEFINE_VTABLE_PTR_HELPER_CTOR_CALLER(AABCharacter); \
	DEFINE_DEFAULT_CONSTRUCTOR_CALL(AABCharacter)


#define Client_Source_ArenaBattle_ABCharacter_h_12_PRIVATE_PROPERTY_OFFSET
#define Client_Source_ArenaBattle_ABCharacter_h_9_PROLOG
#define Client_Source_ArenaBattle_ABCharacter_h_12_GENERATED_BODY_LEGACY \
PRAGMA_DISABLE_DEPRECATION_WARNINGS \
public: \
	Client_Source_ArenaBattle_ABCharacter_h_12_PRIVATE_PROPERTY_OFFSET \
	Client_Source_ArenaBattle_ABCharacter_h_12_SPARSE_DATA \
	Client_Source_ArenaBattle_ABCharacter_h_12_RPC_WRAPPERS \
	Client_Source_ArenaBattle_ABCharacter_h_12_INCLASS \
	Client_Source_ArenaBattle_ABCharacter_h_12_STANDARD_CONSTRUCTORS \
public: \
PRAGMA_ENABLE_DEPRECATION_WARNINGS


#define Client_Source_ArenaBattle_ABCharacter_h_12_GENERATED_BODY \
PRAGMA_DISABLE_DEPRECATION_WARNINGS \
public: \
	Client_Source_ArenaBattle_ABCharacter_h_12_PRIVATE_PROPERTY_OFFSET \
	Client_Source_ArenaBattle_ABCharacter_h_12_SPARSE_DATA \
	Client_Source_ArenaBattle_ABCharacter_h_12_RPC_WRAPPERS_NO_PURE_DECLS \
	Client_Source_ArenaBattle_ABCharacter_h_12_INCLASS_NO_PURE_DECLS \
	Client_Source_ArenaBattle_ABCharacter_h_12_ENHANCED_CONSTRUCTORS \
private: \
PRAGMA_ENABLE_DEPRECATION_WARNINGS


template<> ARENABATTLE_API UClass* StaticClass<class AABCharacter>();

#undef CURRENT_FILE_ID
#define CURRENT_FILE_ID Client_Source_ArenaBattle_ABCharacter_h


PRAGMA_ENABLE_DEPRECATION_WARNINGS
