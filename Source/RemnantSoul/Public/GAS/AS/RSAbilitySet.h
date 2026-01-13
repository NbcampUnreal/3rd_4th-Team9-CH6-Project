#pragma once

#include "CoreMinimal.h"
//#include "UObject/Object.h"
#include "Engine/DataAsset.h"
#include "GameplayTagContainer.h"
#include "GameplayAbilitySpec.h"     
#include "GameplayEffectTypes.h"     
#include "RSAbilitySet.generated.h"

class UAbilitySystemComponent;
class UGameplayAbility;
class UGameplayEffect;
class UAttributeSet;

/**
 * ASC에 실제로 부여된 핸들들을 보관하는 구조체
 *
 * - AbilitySet을 ASC에 적용하면, 부여된 AbilitySpecHandle / GEHandle / AttributeSet을
 *   전부 여기에 기록해둔다.
 * - 나중에 TakeFromAbilitySystem()을 호출해서
 *   해당 세트만 깔끔하게 제거할 수 있다.
 */
USTRUCT(BlueprintType)
struct REMNANTSOUL_API FRSAbilitySet_GrantedHandles
{
	GENERATED_BODY()

public:
	/** 부여된 Ability들의 SpecHandle 목록 */
	UPROPERTY()
	TArray<FGameplayAbilitySpecHandle> AbilitySpecHandles;

	/** 부여된 GameplayEffect들의 ActiveHandle 목록 */
	UPROPERTY()
	TArray<FActiveGameplayEffectHandle> GameplayEffectHandles;

	/** 부여된 AttributeSet 인스턴스 목록 */
	UPROPERTY()
	TArray<TObjectPtr<UAttributeSet>> GrantedAttributeSets;

public:
	void AddAbilitySpecHandle(const FGameplayAbilitySpecHandle& Handle)
	{
		if (Handle.IsValid())
		{
			AbilitySpecHandles.Add(Handle);
		}
	}

	void AddGameplayEffectHandle(const FActiveGameplayEffectHandle& Handle)
	{
		if (Handle.IsValid())
		{
			GameplayEffectHandles.Add(Handle);
		}
	}

	void AddAttributeSet(UAttributeSet* Set)
	{
		if (Set)
		{
			GrantedAttributeSets.Add(Set);
		}
	}

	/**
	 * ASC에서 이 핸들들이 가리키는 Ability/GE/AttributeSet을 제거
	 * - Ability: ClearAbility
	 * - GameplayEffect: RemoveActiveGameplayEffect
	 * - AttributeSet: ASC->RemoveSpawnedAttribute
	 */
	void TakeFromAbilitySystem(UAbilitySystemComponent* ASC);
};

/**
 * 한 개 AbilitySet 안에 들어갈 "Ability 한 개"에 대한 정의
 */
USTRUCT(BlueprintType)
struct REMNANTSOUL_API FRSAbilitySet_GameplayAbility
{
	GENERATED_BODY()

public:
	/** 부여할 Ability 클래스 */
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "RS|AbilitySet")
	TSubclassOf<UGameplayAbility> Ability = nullptr;

	/** 부여할 레벨 (기본 1) */
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "RS|AbilitySet")
	int32 AbilityLevel = 1;

	/**
	 * 이 Ability의 입력/역할 태그
	 * - 예: InputTag.Ability.Attack, InputTag.Ability.Skill
	 * - HeroComponent → InputConfig → 이벤트 태그와 맞춰 쓰거나,
	 *   Ability 내부에서 ActivationOwnedTags/AbilityTags로 활용 가능
	 */
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "RS|AbilitySet", meta = (Categories = "InputTag"))
	FGameplayTag InputTag;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "RS|Ability", meta = (Categories = "Event"))
	FGameplayTag EventTag;
};

/**
 * 한번에 부여/회수할 GameplayAbility / GameplayEffect / AttributeSet 세트
 *
 * - 사용처:
 *   - RSItemFragment_AbilitySet에서 무기별 AbilitySet 지정
 *   - PawnData에서 기본 AbilitySet 지정
 *   - Boss Phase / 클래스 전환 시 AbilitySet 교체에도 재사용 가능
 */
UCLASS(BlueprintType, Const)
class REMNANTSOUL_API URSAbilitySet : public UDataAsset
{
	GENERATED_BODY()

public:
	URSAbilitySet(const FObjectInitializer& ObjectInitializer = FObjectInitializer::Get());

public:
	/**
	 * AbilitySet 적용:
	 * - GrantedAbilities에 있는 Ability를 ASC에 전부 GiveAbility
	 * - 추후 확장: GrantedEffects / GrantedAttributes도 여기서 적용
	 *
	 * @param ASC: 대상 AbilitySystemComponent
	 * @param OutGrantedHandles: (선택) 부여된 핸들들을 저장할 구조체
	 * @param SourceObject: AbilitySpec.SourceObject로 설정할 값 (무기 액터/템플릿 등)
	 */
	void GiveToAbilitySystem(
		UAbilitySystemComponent* ASC,
		FRSAbilitySet_GrantedHandles* OutGrantedHandles = nullptr,
		UObject* SourceObject = nullptr
	) const;

	/** 추후 필요하면 여기서 "이 AbilitySet이 어떤 역할인지"를 설명하는 태그를 추가할 수도 있음 */
	// UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category="RS|AbilitySet")
	// FGameplayTagContainer SetTags;

public:
	/** 이 AbilitySet이 ASC에 적용될 때 함께 부여할 Ability 목록 */
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "RS|AbilitySet")
	TArray<FRSAbilitySet_GameplayAbility> GrantedAbilities;
	// v1에서는 Ability만 사용. 나중에 원하면 아래 두 개를 확장해서 사용할 수 있다.

	/** 이 AbilitySet이 적용될 때 즉시 줄 GameplayEffect 목록 (버프/패시브 등) */
	// UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category="RS|AbilitySet")
	// TArray<TSubclassOf<UGameplayEffect>> GrantedEffects;

	/** 이 AbilitySet이 적용될 때 함께 추가할 AttributeSet 목록 */
	// UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category="RS|AbilitySet")
	// TArray<TSubclassOf<UAttributeSet>> GrantedAttributeSets;
};
