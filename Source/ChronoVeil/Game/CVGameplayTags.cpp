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
	// ── InitState.* (GameFrameworkComponentManager 단계 구분) ──
	AddTag(GameplayTags.InitState_Spawned, "InitState.Spawned", "1: Spawned and can be extended");
	AddTag(GameplayTags.InitState_DataAvailable, "InitState.DataAvailable", "2: Required data loaded/replicated");
	AddTag(GameplayTags.InitState_DataInitialized, "InitState.DataInitialized", "3: Data initialized but not gameplay-ready");
	AddTag(GameplayTags.InitState_GameplayReady, "InitState.GameplayReady", "4: Fully ready for gameplay");

	// ── Enhanced Input ──
	AddTag(GameplayTags.InputTag_Move, "InputTag.Move", "");
	AddTag(GameplayTags.InputTag_Look_Mouse, "InputTag.Look.Mouse", "");

	// TODO: AddTag(... InputTag.Weapon.Fire / Reload / ADS ...)
	// TODO: AddTag(... Input.Layer.KBM / Drone / ADS ...)
}
