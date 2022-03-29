// Copyright Epic Games, Inc. All Rights Reserved.
/*===========================================================================
	Generated code exported from UnrealHeaderTool.
	DO NOT modify this manually! Edit the corresponding .h files instead!
===========================================================================*/

#include "UObject/ObjectMacros.h"
#include "UObject/ScriptMacros.h"

PRAGMA_DISABLE_DEPRECATION_WARNINGS
class UAnimMontage;
#ifdef FINAL_PROJECT_MyCharacter_generated_h
#error "MyCharacter.generated.h already included, missing '#pragma once' in MyCharacter.h"
#endif
#define FINAL_PROJECT_MyCharacter_generated_h

#define Unreal_project_Source_Final_Project_MyCharacter_h_13_SPARSE_DATA
#define Unreal_project_Source_Final_Project_MyCharacter_h_13_RPC_WRAPPERS \
 \
	DECLARE_FUNCTION(execOnAttackMontageEnded);


#define Unreal_project_Source_Final_Project_MyCharacter_h_13_RPC_WRAPPERS_NO_PURE_DECLS \
 \
	DECLARE_FUNCTION(execOnAttackMontageEnded);


#define Unreal_project_Source_Final_Project_MyCharacter_h_13_INCLASS_NO_PURE_DECLS \
private: \
	static void StaticRegisterNativesAMyCharacter(); \
	friend struct Z_Construct_UClass_AMyCharacter_Statics; \
public: \
	DECLARE_CLASS(AMyCharacter, ACharacter, COMPILED_IN_FLAGS(0 | CLASS_Config), CASTCLASS_None, TEXT("/Script/Final_Project"), NO_API) \
	DECLARE_SERIALIZER(AMyCharacter)


#define Unreal_project_Source_Final_Project_MyCharacter_h_13_INCLASS \
private: \
	static void StaticRegisterNativesAMyCharacter(); \
	friend struct Z_Construct_UClass_AMyCharacter_Statics; \
public: \
	DECLARE_CLASS(AMyCharacter, ACharacter, COMPILED_IN_FLAGS(0 | CLASS_Config), CASTCLASS_None, TEXT("/Script/Final_Project"), NO_API) \
	DECLARE_SERIALIZER(AMyCharacter)


#define Unreal_project_Source_Final_Project_MyCharacter_h_13_STANDARD_CONSTRUCTORS \
	/** Standard constructor, called after all reflected properties have been initialized */ \
	NO_API AMyCharacter(const FObjectInitializer& ObjectInitializer); \
	DEFINE_DEFAULT_OBJECT_INITIALIZER_CONSTRUCTOR_CALL(AMyCharacter) \
	DECLARE_VTABLE_PTR_HELPER_CTOR(NO_API, AMyCharacter); \
DEFINE_VTABLE_PTR_HELPER_CTOR_CALLER(AMyCharacter); \
private: \
	/** Private move- and copy-constructors, should never be used */ \
	NO_API AMyCharacter(AMyCharacter&&); \
	NO_API AMyCharacter(const AMyCharacter&); \
public:


#define Unreal_project_Source_Final_Project_MyCharacter_h_13_ENHANCED_CONSTRUCTORS \
private: \
	/** Private move- and copy-constructors, should never be used */ \
	NO_API AMyCharacter(AMyCharacter&&); \
	NO_API AMyCharacter(const AMyCharacter&); \
public: \
	DECLARE_VTABLE_PTR_HELPER_CTOR(NO_API, AMyCharacter); \
DEFINE_VTABLE_PTR_HELPER_CTOR_CALLER(AMyCharacter); \
	DEFINE_DEFAULT_CONSTRUCTOR_CALL(AMyCharacter)


#define Unreal_project_Source_Final_Project_MyCharacter_h_13_PRIVATE_PROPERTY_OFFSET \
	FORCEINLINE static uint32 __PPO__ProjectileClass() { return STRUCT_OFFSET(AMyCharacter, ProjectileClass); } \
	FORCEINLINE static uint32 __PPO__IsAttacking() { return STRUCT_OFFSET(AMyCharacter, IsAttacking); } \
	FORCEINLINE static uint32 __PPO__MyAnim() { return STRUCT_OFFSET(AMyCharacter, MyAnim); } \
	FORCEINLINE static uint32 __PPO__IsDead() { return STRUCT_OFFSET(AMyCharacter, IsDead); } \
	FORCEINLINE static uint32 __PPO__bear() { return STRUCT_OFFSET(AMyCharacter, bear); } \
	FORCEINLINE static uint32 __PPO__snowman() { return STRUCT_OFFSET(AMyCharacter, snowman); } \
	FORCEINLINE static uint32 __PPO__bearAnim() { return STRUCT_OFFSET(AMyCharacter, bearAnim); } \
	FORCEINLINE static uint32 __PPO__snowmanAnim() { return STRUCT_OFFSET(AMyCharacter, snowmanAnim); }


#define Unreal_project_Source_Final_Project_MyCharacter_h_10_PROLOG
#define Unreal_project_Source_Final_Project_MyCharacter_h_13_GENERATED_BODY_LEGACY \
PRAGMA_DISABLE_DEPRECATION_WARNINGS \
public: \
	Unreal_project_Source_Final_Project_MyCharacter_h_13_PRIVATE_PROPERTY_OFFSET \
	Unreal_project_Source_Final_Project_MyCharacter_h_13_SPARSE_DATA \
	Unreal_project_Source_Final_Project_MyCharacter_h_13_RPC_WRAPPERS \
	Unreal_project_Source_Final_Project_MyCharacter_h_13_INCLASS \
	Unreal_project_Source_Final_Project_MyCharacter_h_13_STANDARD_CONSTRUCTORS \
public: \
PRAGMA_ENABLE_DEPRECATION_WARNINGS


#define Unreal_project_Source_Final_Project_MyCharacter_h_13_GENERATED_BODY \
PRAGMA_DISABLE_DEPRECATION_WARNINGS \
public: \
	Unreal_project_Source_Final_Project_MyCharacter_h_13_PRIVATE_PROPERTY_OFFSET \
	Unreal_project_Source_Final_Project_MyCharacter_h_13_SPARSE_DATA \
	Unreal_project_Source_Final_Project_MyCharacter_h_13_RPC_WRAPPERS_NO_PURE_DECLS \
	Unreal_project_Source_Final_Project_MyCharacter_h_13_INCLASS_NO_PURE_DECLS \
	Unreal_project_Source_Final_Project_MyCharacter_h_13_ENHANCED_CONSTRUCTORS \
private: \
PRAGMA_ENABLE_DEPRECATION_WARNINGS


template<> FINAL_PROJECT_API UClass* StaticClass<class AMyCharacter>();

#undef CURRENT_FILE_ID
#define CURRENT_FILE_ID Unreal_project_Source_Final_Project_MyCharacter_h


PRAGMA_ENABLE_DEPRECATION_WARNINGS
