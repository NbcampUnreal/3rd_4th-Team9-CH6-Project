//// Copyright Epic Games, Inc. All Rights Reserved.
//
//#include "RemnantSoul.h"
//#include "Modules/ModuleManager.h"
//#include "RSGameplayTags.h"
//
//class FRemnantSoulModule : public FDefaultGameModuleImpl
//{
//public:
//	virtual void StartupModule() override
//	{
//		FDefaultGameModuleImpl::StartupModule();
//
//		FRSGameplayTags::InitializeNativeTags();
//	}
//};
//
//IMPLEMENT_PRIMARY_GAME_MODULE( FDefaultGameModuleImpl, RemnantSoul, "RemnantSoul" );
//
//DEFINE_LOG_CATEGORY(LogRemnantSoul)


// RemnantSoul.cpp
#include "RemnantSoul.h"
#include "Modules/ModuleManager.h"
#include "RSGameplayTags.h"

class FRemnantSoulModule : public FDefaultGameModuleImpl
{
public:
	virtual void StartupModule() override
	{
		FDefaultGameModuleImpl::StartupModule();

		// Native GameplayTags 등록 (프로젝트 부팅 시 1회만 적용됨.)
		FRSGameplayTags::InitializeNativeTags();
	}
};

IMPLEMENT_PRIMARY_GAME_MODULE(FRemnantSoulModule, RemnantSoul, "RemnantSoul");
DEFINE_LOG_CATEGORY(LogRemnantSoul);
