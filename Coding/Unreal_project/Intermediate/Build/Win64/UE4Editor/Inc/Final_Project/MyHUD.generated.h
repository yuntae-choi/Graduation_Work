// Copyright Epic Games, Inc. All Rights Reserved.
/*===========================================================================
	Generated code exported from UnrealHeaderTool.
	DO NOT modify this manually! Edit the corresponding .h files instead!
===========================================================================*/

#include "UObject/ObjectMacros.h"
#include "UObject/ScriptMacros.h"

PRAGMA_DISABLE_DEPRECATION_WARNINGS
#ifdef FINAL_PROJECT_MyHUD_generated_h
#error "MyHUD.generated.h already included, missing '#pragma once' in MyHUD.h"
#endif
#define FINAL_PROJECT_MyHUD_generated_h

#define Unreal_project_Source_Final_Project_MyHUD_h_16_SPARSE_DATA
#define Unreal_project_Source_Final_Project_MyHUD_h_16_RPC_WRAPPERS
#define Unreal_project_Source_Final_Project_MyHUD_h_16_RPC_WRAPPERS_NO_PURE_DECLS
#define Unreal_project_Source_Final_Project_MyHUD_h_16_INCLASS_NO_PURE_DECLS \
private: \
	static void StaticRegisterNativesAMyHUD(); \
	friend struct Z_Construct_UClass_AMyHUD_Statics; \
public: \
	DECLARE_CLASS(AMyHUD, AHUD, COMPILED_IN_FLAGS(0 | CLASS_Transient | CLASS_Config), CASTCLASS_None, TEXT("/Script/Final_Project"), NO_API) \
	DECLARE_SERIALIZER(AMyHUD)


#define Unreal_project_Source_Final_Project_MyHUD_h_16_INCLASS \
private: \
	static void StaticRegisterNativesAMyHUD(); \
	friend struct Z_Construct_UClass_AMyHUD_Statics; \
public: \
	DECLARE_CLASS(AMyHUD, AHUD, COMPILED_IN_FLAGS(0 | CLASS_Transient | CLASS_Config), CASTCLASS_None, TEXT("/Script/Final_Project"), NO_API) \
	DECLARE_SERIALIZER(AMyHUD)


#define Unreal_project_Source_Final_Project_MyHUD_h_16_STANDARD_CONSTRUCTORS \
	/** Standard constructor, called after all reflected properties have been initialized */ \
	NO_API AMyHUD(const FObjectInitializer& ObjectInitializer = FObjectInitializer::Get()); \
	DEFINE_DEFAULT_OBJECT_INITIALIZER_CONSTRUCTOR_CALL(AMyHUD) \
	DECLARE_VTABLE_PTR_HELPER_CTOR(NO_API, AMyHUD); \
DEFINE_VTABLE_PTR_HELPER_CTOR_CALLER(AMyHUD); \
private: \
	/** Private move- and copy-constructors, should never be used */ \
	NO_API AMyHUD(AMyHUD&&); \
	NO_API AMyHUD(const AMyHUD&); \
public:


#define Unreal_project_Source_Final_Project_MyHUD_h_16_ENHANCED_CONSTRUCTORS \
	/** Standard constructor, called after all reflected properties have been initialized */ \
	NO_API AMyHUD(const FObjectInitializer& ObjectInitializer = FObjectInitializer::Get()) : Super(ObjectInitializer) { }; \
private: \
	/** Private move- and copy-constructors, should never be used */ \
	NO_API AMyHUD(AMyHUD&&); \
	NO_API AMyHUD(const AMyHUD&); \
public: \
	DECLARE_VTABLE_PTR_HELPER_CTOR(NO_API, AMyHUD); \
DEFINE_VTABLE_PTR_HELPER_CTOR_CALLER(AMyHUD); \
	DEFINE_DEFAULT_OBJECT_INITIALIZER_CONSTRUCTOR_CALL(AMyHUD)


#define Unreal_project_Source_Final_Project_MyHUD_h_16_PRIVATE_PROPERTY_OFFSET \
	FORCEINLINE static uint32 __PPO__CrosshairTexture() { return STRUCT_OFFSET(AMyHUD, CrosshairTexture); }


#define Unreal_project_Source_Final_Project_MyHUD_h_13_PROLOG
#define Unreal_project_Source_Final_Project_MyHUD_h_16_GENERATED_BODY_LEGACY \
PRAGMA_DISABLE_DEPRECATION_WARNINGS \
public: \
	Unreal_project_Source_Final_Project_MyHUD_h_16_PRIVATE_PROPERTY_OFFSET \
	Unreal_project_Source_Final_Project_MyHUD_h_16_SPARSE_DATA \
	Unreal_project_Source_Final_Project_MyHUD_h_16_RPC_WRAPPERS \
	Unreal_project_Source_Final_Project_MyHUD_h_16_INCLASS \
	Unreal_project_Source_Final_Project_MyHUD_h_16_STANDARD_CONSTRUCTORS \
public: \
PRAGMA_ENABLE_DEPRECATION_WARNINGS


#define Unreal_project_Source_Final_Project_MyHUD_h_16_GENERATED_BODY \
PRAGMA_DISABLE_DEPRECATION_WARNINGS \
public: \
	Unreal_project_Source_Final_Project_MyHUD_h_16_PRIVATE_PROPERTY_OFFSET \
	Unreal_project_Source_Final_Project_MyHUD_h_16_SPARSE_DATA \
	Unreal_project_Source_Final_Project_MyHUD_h_16_RPC_WRAPPERS_NO_PURE_DECLS \
	Unreal_project_Source_Final_Project_MyHUD_h_16_INCLASS_NO_PURE_DECLS \
	Unreal_project_Source_Final_Project_MyHUD_h_16_ENHANCED_CONSTRUCTORS \
private: \
PRAGMA_ENABLE_DEPRECATION_WARNINGS


template<> FINAL_PROJECT_API UClass* StaticClass<class AMyHUD>();

#undef CURRENT_FILE_ID
#define CURRENT_FILE_ID Unreal_project_Source_Final_Project_MyHUD_h


PRAGMA_ENABLE_DEPRECATION_WARNINGS
