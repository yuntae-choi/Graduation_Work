// Copyright Epic Games, Inc. All Rights Reserved.
/*===========================================================================
	Generated code exported from UnrealHeaderTool.
	DO NOT modify this manually! Edit the corresponding .h files instead!
===========================================================================*/

#include "UObject/ObjectMacros.h"
#include "UObject/ScriptMacros.h"

PRAGMA_DISABLE_DEPRECATION_WARNINGS
#ifdef FINAL_PROJECT_MyAnimInstance_generated_h
#error "MyAnimInstance.generated.h already included, missing '#pragma once' in MyAnimInstance.h"
#endif
#define FINAL_PROJECT_MyAnimInstance_generated_h

#define Unreal_project_Source_Final_Project_MyAnimInstance_h_15_SPARSE_DATA
#define Unreal_project_Source_Final_Project_MyAnimInstance_h_15_RPC_WRAPPERS
#define Unreal_project_Source_Final_Project_MyAnimInstance_h_15_RPC_WRAPPERS_NO_PURE_DECLS
#define Unreal_project_Source_Final_Project_MyAnimInstance_h_15_INCLASS_NO_PURE_DECLS \
private: \
	static void StaticRegisterNativesUMyAnimInstance(); \
	friend struct Z_Construct_UClass_UMyAnimInstance_Statics; \
public: \
	DECLARE_CLASS(UMyAnimInstance, UAnimInstance, COMPILED_IN_FLAGS(0 | CLASS_Transient), CASTCLASS_None, TEXT("/Script/Final_Project"), NO_API) \
	DECLARE_SERIALIZER(UMyAnimInstance)


#define Unreal_project_Source_Final_Project_MyAnimInstance_h_15_INCLASS \
private: \
	static void StaticRegisterNativesUMyAnimInstance(); \
	friend struct Z_Construct_UClass_UMyAnimInstance_Statics; \
public: \
	DECLARE_CLASS(UMyAnimInstance, UAnimInstance, COMPILED_IN_FLAGS(0 | CLASS_Transient), CASTCLASS_None, TEXT("/Script/Final_Project"), NO_API) \
	DECLARE_SERIALIZER(UMyAnimInstance)


#define Unreal_project_Source_Final_Project_MyAnimInstance_h_15_STANDARD_CONSTRUCTORS \
	/** Standard constructor, called after all reflected properties have been initialized */ \
	NO_API UMyAnimInstance(const FObjectInitializer& ObjectInitializer); \
	DEFINE_DEFAULT_OBJECT_INITIALIZER_CONSTRUCTOR_CALL(UMyAnimInstance) \
	DECLARE_VTABLE_PTR_HELPER_CTOR(NO_API, UMyAnimInstance); \
DEFINE_VTABLE_PTR_HELPER_CTOR_CALLER(UMyAnimInstance); \
private: \
	/** Private move- and copy-constructors, should never be used */ \
	NO_API UMyAnimInstance(UMyAnimInstance&&); \
	NO_API UMyAnimInstance(const UMyAnimInstance&); \
public:


#define Unreal_project_Source_Final_Project_MyAnimInstance_h_15_ENHANCED_CONSTRUCTORS \
private: \
	/** Private move- and copy-constructors, should never be used */ \
	NO_API UMyAnimInstance(UMyAnimInstance&&); \
	NO_API UMyAnimInstance(const UMyAnimInstance&); \
public: \
	DECLARE_VTABLE_PTR_HELPER_CTOR(NO_API, UMyAnimInstance); \
DEFINE_VTABLE_PTR_HELPER_CTOR_CALLER(UMyAnimInstance); \
	DEFINE_DEFAULT_CONSTRUCTOR_CALL(UMyAnimInstance)


#define Unreal_project_Source_Final_Project_MyAnimInstance_h_15_PRIVATE_PROPERTY_OFFSET \
	FORCEINLINE static uint32 __PPO__CurrentPawnSpeed() { return STRUCT_OFFSET(UMyAnimInstance, CurrentPawnSpeed); } \
	FORCEINLINE static uint32 __PPO__IsInAir() { return STRUCT_OFFSET(UMyAnimInstance, IsInAir); } \
	FORCEINLINE static uint32 __PPO__AttackMontage() { return STRUCT_OFFSET(UMyAnimInstance, AttackMontage); }


#define Unreal_project_Source_Final_Project_MyAnimInstance_h_12_PROLOG
#define Unreal_project_Source_Final_Project_MyAnimInstance_h_15_GENERATED_BODY_LEGACY \
PRAGMA_DISABLE_DEPRECATION_WARNINGS \
public: \
	Unreal_project_Source_Final_Project_MyAnimInstance_h_15_PRIVATE_PROPERTY_OFFSET \
	Unreal_project_Source_Final_Project_MyAnimInstance_h_15_SPARSE_DATA \
	Unreal_project_Source_Final_Project_MyAnimInstance_h_15_RPC_WRAPPERS \
	Unreal_project_Source_Final_Project_MyAnimInstance_h_15_INCLASS \
	Unreal_project_Source_Final_Project_MyAnimInstance_h_15_STANDARD_CONSTRUCTORS \
public: \
PRAGMA_ENABLE_DEPRECATION_WARNINGS


#define Unreal_project_Source_Final_Project_MyAnimInstance_h_15_GENERATED_BODY \
PRAGMA_DISABLE_DEPRECATION_WARNINGS \
public: \
	Unreal_project_Source_Final_Project_MyAnimInstance_h_15_PRIVATE_PROPERTY_OFFSET \
	Unreal_project_Source_Final_Project_MyAnimInstance_h_15_SPARSE_DATA \
	Unreal_project_Source_Final_Project_MyAnimInstance_h_15_RPC_WRAPPERS_NO_PURE_DECLS \
	Unreal_project_Source_Final_Project_MyAnimInstance_h_15_INCLASS_NO_PURE_DECLS \
	Unreal_project_Source_Final_Project_MyAnimInstance_h_15_ENHANCED_CONSTRUCTORS \
private: \
PRAGMA_ENABLE_DEPRECATION_WARNINGS


template<> FINAL_PROJECT_API UClass* StaticClass<class UMyAnimInstance>();

#undef CURRENT_FILE_ID
#define CURRENT_FILE_ID Unreal_project_Source_Final_Project_MyAnimInstance_h


PRAGMA_ENABLE_DEPRECATION_WARNINGS
