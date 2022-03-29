// Copyright Epic Games, Inc. All Rights Reserved.
/*===========================================================================
	Generated code exported from UnrealHeaderTool.
	DO NOT modify this manually! Edit the corresponding .h files instead!
===========================================================================*/

#include "UObject/ObjectMacros.h"
#include "UObject/ScriptMacros.h"

PRAGMA_DISABLE_DEPRECATION_WARNINGS
class UPrimitiveComponent;
class AActor;
struct FHitResult;
#ifdef FINAL_PROJECT_MyItemBox_generated_h
#error "MyItemBox.generated.h already included, missing '#pragma once' in MyItemBox.h"
#endif
#define FINAL_PROJECT_MyItemBox_generated_h

#define Unreal_project_Source_Final_Project_MyItemBox_h_12_SPARSE_DATA
#define Unreal_project_Source_Final_Project_MyItemBox_h_12_RPC_WRAPPERS \
 \
	DECLARE_FUNCTION(execOnCharacterOverlap);


#define Unreal_project_Source_Final_Project_MyItemBox_h_12_RPC_WRAPPERS_NO_PURE_DECLS \
 \
	DECLARE_FUNCTION(execOnCharacterOverlap);


#define Unreal_project_Source_Final_Project_MyItemBox_h_12_INCLASS_NO_PURE_DECLS \
private: \
	static void StaticRegisterNativesAMyItemBox(); \
	friend struct Z_Construct_UClass_AMyItemBox_Statics; \
public: \
	DECLARE_CLASS(AMyItemBox, AActor, COMPILED_IN_FLAGS(0 | CLASS_Config), CASTCLASS_None, TEXT("/Script/Final_Project"), NO_API) \
	DECLARE_SERIALIZER(AMyItemBox)


#define Unreal_project_Source_Final_Project_MyItemBox_h_12_INCLASS \
private: \
	static void StaticRegisterNativesAMyItemBox(); \
	friend struct Z_Construct_UClass_AMyItemBox_Statics; \
public: \
	DECLARE_CLASS(AMyItemBox, AActor, COMPILED_IN_FLAGS(0 | CLASS_Config), CASTCLASS_None, TEXT("/Script/Final_Project"), NO_API) \
	DECLARE_SERIALIZER(AMyItemBox)


#define Unreal_project_Source_Final_Project_MyItemBox_h_12_STANDARD_CONSTRUCTORS \
	/** Standard constructor, called after all reflected properties have been initialized */ \
	NO_API AMyItemBox(const FObjectInitializer& ObjectInitializer); \
	DEFINE_DEFAULT_OBJECT_INITIALIZER_CONSTRUCTOR_CALL(AMyItemBox) \
	DECLARE_VTABLE_PTR_HELPER_CTOR(NO_API, AMyItemBox); \
DEFINE_VTABLE_PTR_HELPER_CTOR_CALLER(AMyItemBox); \
private: \
	/** Private move- and copy-constructors, should never be used */ \
	NO_API AMyItemBox(AMyItemBox&&); \
	NO_API AMyItemBox(const AMyItemBox&); \
public:


#define Unreal_project_Source_Final_Project_MyItemBox_h_12_ENHANCED_CONSTRUCTORS \
private: \
	/** Private move- and copy-constructors, should never be used */ \
	NO_API AMyItemBox(AMyItemBox&&); \
	NO_API AMyItemBox(const AMyItemBox&); \
public: \
	DECLARE_VTABLE_PTR_HELPER_CTOR(NO_API, AMyItemBox); \
DEFINE_VTABLE_PTR_HELPER_CTOR_CALLER(AMyItemBox); \
	DEFINE_DEFAULT_CONSTRUCTOR_CALL(AMyItemBox)


#define Unreal_project_Source_Final_Project_MyItemBox_h_12_PRIVATE_PROPERTY_OFFSET
#define Unreal_project_Source_Final_Project_MyItemBox_h_9_PROLOG
#define Unreal_project_Source_Final_Project_MyItemBox_h_12_GENERATED_BODY_LEGACY \
PRAGMA_DISABLE_DEPRECATION_WARNINGS \
public: \
	Unreal_project_Source_Final_Project_MyItemBox_h_12_PRIVATE_PROPERTY_OFFSET \
	Unreal_project_Source_Final_Project_MyItemBox_h_12_SPARSE_DATA \
	Unreal_project_Source_Final_Project_MyItemBox_h_12_RPC_WRAPPERS \
	Unreal_project_Source_Final_Project_MyItemBox_h_12_INCLASS \
	Unreal_project_Source_Final_Project_MyItemBox_h_12_STANDARD_CONSTRUCTORS \
public: \
PRAGMA_ENABLE_DEPRECATION_WARNINGS


#define Unreal_project_Source_Final_Project_MyItemBox_h_12_GENERATED_BODY \
PRAGMA_DISABLE_DEPRECATION_WARNINGS \
public: \
	Unreal_project_Source_Final_Project_MyItemBox_h_12_PRIVATE_PROPERTY_OFFSET \
	Unreal_project_Source_Final_Project_MyItemBox_h_12_SPARSE_DATA \
	Unreal_project_Source_Final_Project_MyItemBox_h_12_RPC_WRAPPERS_NO_PURE_DECLS \
	Unreal_project_Source_Final_Project_MyItemBox_h_12_INCLASS_NO_PURE_DECLS \
	Unreal_project_Source_Final_Project_MyItemBox_h_12_ENHANCED_CONSTRUCTORS \
private: \
PRAGMA_ENABLE_DEPRECATION_WARNINGS


template<> FINAL_PROJECT_API UClass* StaticClass<class AMyItemBox>();

#undef CURRENT_FILE_ID
#define CURRENT_FILE_ID Unreal_project_Source_Final_Project_MyItemBox_h


PRAGMA_ENABLE_DEPRECATION_WARNINGS
