#include "Game/CVGameplayTags.h"
#include "GameplayTagsManager.h"

FCVGameplayTags FCVGameplayTags::GameplayTags;

void FCVGameplayTags::InitializeNativeTags()
{
	UGameplayTagsManager& Manager = UGameplayTagsManager::Get();
	AddAllTags(Manager);
}

void FCVGameplayTags::AddTag(FGameplayTag& OutTag, const ANSICHAR* TagName, const ANSICHAR* TagComment)
{
	OutTag = UGameplayTagsManager::Get().AddNativeGameplayTag(FName(TagName), FString(TEXT("(Native) ")) + FString(TagComment));
}

void FCVGameplayTags::AddAllTags(UGameplayTagsManager& Manager)
{
	// InitState.* (GameFrameworkComponentManager 단계 구분) : 주의! 현재 우리가 만드는 프로젝트에서는 사용안하고 있음. 팀장인 내가 Experience적용하려다가 프레임워크가 너무 커질 것 같아서 해당 내용은 테스트 삼아 정의만했음.
	AddTag(GameplayTags.InitState_Spawned, "InitState.Spawned", "1: Spawned and can be extended");
	AddTag(GameplayTags.InitState_DataAvailable, "InitState.DataAvailable", "2: Required data loaded/replicated");
	AddTag(GameplayTags.InitState_DataInitialized, "InitState.DataInitialized", "3: Data initialized but not gameplay-ready");
	AddTag(GameplayTags.InitState_GameplayReady, "InitState.GameplayReady", "4: Fully ready for gameplay");

	// Enhanced Input
	AddTag(GameplayTags.InputTag_Native_Move, "InputTag.Native.Move", "");
	AddTag(GameplayTags.InputTag_Native_Look_Mouse, "InputTag.Native.Look.Mouse", "");

	// TODO: AddTag(... InputTag.Weapon.Fire / Reload / ADS ...)
	// TODO: AddTag(... Input.Layer.KBM / Drone / ADS ...)
}
