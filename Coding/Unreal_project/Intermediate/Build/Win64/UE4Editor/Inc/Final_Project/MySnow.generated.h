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
struct FVector;
struct FHitResult;
#ifdef FINAL_PROJECT_MySnow_generated_h
#error "MySnow.generated.h already included, missing '#pragma once' in MySnow.h"
#endif
#define FINAL_PROJECT_MySnow_generated_h

#define Unreal_project_Source_Final_Project_MySnow_h_14_SPARSE_DATA
#define Unreal_project_Source_Final_Project_MySnow_h_14_RPC_WRAPPERS \
 \
	DECLARE_FUNCTION(execOnHit);


#define Unreal_project_Source_Final_Project_MySnow_h_14_RPC_WRAPPERS_NO_PURE_DECLS \
 \
	DECLARE_FUNCTION(execOnHit);


#define Unreal_project_Source_Final_Project_MySnow_h_14_INCLASS_NO_PURE_DECLS \
private: \
	static void StaticRegisterNativesAMySnow(); \
	friend struct Z_Construct_UClass_AMySnow_Statics; \
public: \
	DECLARE_CLASS(AMySnow, AActor, COMPILED_IN_FLAGS(0 | CLASS_Config), CASTCLASS_None, TEXT("/Script/Final_Project"), NO_API) \
	DECLARE_SERIALIZER(AMySnow)


#define Unreal_project_Source_Final_Project_MySnow_h_14_INCLASS \
private: \
	static void StaticRegisterNativesAMySnow(); \
	friend struct Z_Construct_UClass_AMySnow_Statics; \
public: \
	DECLARE_CLASS(AMySnow, AActor, COMPILED_IN_FLAGS(0 | CLASS_Config), CASTCLASS_None, TEXT("/Script/Final_Project"), NO_API) \
	DECLARE_SERIALIZER(AMySnow)


#define Unreal_project_Source_Final_Project_MySnow_h_14_STANDARD_CONSTRUCTORS \
	/** Standard constructor, called after all reflected properties have been initialized */ \
	NO_API AMySnow(const FObjectInitializer& ObjectInitializer); \
	DEFINE_DEFAULT_OBJECT_INITIALIZER_CONSTRUCTOR_CALL(AMySnow) \
	DECLARE_VTABLE_PTR_HELPER_CTOR(NO_API, AMySnow); \
DEFINE_VTABLE_PTR_HELPER_CTOR_CALLER(AMySnow); \
private: \
	/** Private move- and copy-constructors, should never be used */ \
	NO_API AMySnow(AMySnow&&); \
	NO_API AMySnow(const AMySnow&); \
public:


#define Unreal_project_Source_Final_Project_MySnow_h_14_ENHANCED_CONSTRUCTORS \
private: \
	/** Private move- and copy-constructors, should never be used */ \
	NO_API AMySnow(AMySnow&&); \
	NO_API AMySnow(const AMySnow&); \
public: \
	DECLARE_VTABLE_PTR_HELPER_CTOR(NO_API, AMySnow); \
DEFINE_VTABLE_PTR_HELPER_CTOR_CALLER(AMySnow); \
	DEFINE_DEFAULT_CONSTRUCTOR_CALL(AMySnow)


#define Unreal_project_Source_Final_Project_MySnow_h_14_PRIVATE_PROPERTY_OFFSET
#define Unreal_project_Source_Final_Project_MySnow_h_11_PROLOG
#define Unreal_project_Source_Final_Project_MySnow_h_14_GENERATED_BODY_LEGACY \
PRAGMA_DISABLE_DEPRECATION_WARNINGS \
public: \
	Unreal_project_Source_Final_Project_MySnow_h_14_PRIVATE_PROPERTY_OFFSET \
	Unreal_project_Source_Final_Project_MySnow_h_14_SPARSE_DATA \
	Unreal_project_Source_Final_Project_MySnow_h_14_RPC_WRAPPERS \
	Unreal_project_Source_Final_Project_MySnow_h_14_INCLASS \
	Unreal_project_Source_Final_Project_MySnow_h_14_STANDARD_CONSTRUCTORS \
public: \
PRAGMA_ENABLE_DEPRECATION_WARNINGS


#define Unreal_project_Source_Final_Project_MySnow_h_14_GENERATED_BODY \
PRAGMA_DISABLE_DEPRECATION_WARNINGS \
public: \
	Unreal_project_Source_Final_Project_MySnow_h_14_PRIVATE_PROPERTY_OFFSET \
	Unreal_project_Source_Final_Project_MySnow_h_14_SPARSE_DATA \
	Unreal_project_Source_Final_Project_MySnow_h_14_RPC_WRAPPERS_NO_PURE_DECLS \
	Unreal_project_Source_Final_Project_MySnow_h_14_INCLASS_NO_PURE_DECLS \
	Unreal_project_Source_Final_Project_MySnow_h_14_ENHANCED_CONSTRUCTORS \
private: \
PRAGMA_ENABLE_DEPRECATION_WARNINGS


template<> FINAL_PROJECT_API UClass* StaticClass<class AMySnow>();

#undef CURRENT_FILE_ID
#define CURRENT_FILE_ID Unreal_project_Source_Final_Project_MySnow_h


PRAGMA_ENABLE_DEPRECATION_WARNINGS
