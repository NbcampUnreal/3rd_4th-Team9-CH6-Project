#pragma once

#include "CoreMinimal.h"
#include "AttributeSet.h"                 // FGameplayAttribute
#include "Item/Fragments/RSItemFragment.h"
#include "RSItemFragment_EquipStats.generated.h"

/**
 * 장착 시 Attribute에 적용할 Modifier 연산 타입
 */
UENUM(BlueprintType)
enum class ERSItemStatModifierOp : uint8
{
	/** Attribute += Magnitude */
	Additive		UMETA(DisplayName = "Additive"),

	/** Attribute *= Magnitude (예: 1.2 → +20%) */
	Multiplicative UMETA(DisplayName = "Multiplicative"),

	/** Attribute = Magnitude */
	Override		UMETA(DisplayName = "Override")
};

/**
 * "이 Attribute에 이 방식으로 얼마를 적용한다" 한 항목
 */
USTRUCT(BlueprintType)
struct REMNANTSOUL_API FRSItemEquipStatModifier
{
	GENERATED_BODY()

public:
	/** 어떤 Attribute에 적용할지 (예: URSAttributeSet_Character::GetAttackPowerAttribute()) */
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "RS|Item|EquipStats")
	FGameplayAttribute Attribute;

	/** 연산 타입 (Add / Mul / Override) */
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "RS|Item|EquipStats")
	ERSItemStatModifierOp Operation = ERSItemStatModifierOp::Additive;

	/** 크기 (Add일 때 +값, Mul일 때 배수, Override일 때 최종값) */
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "RS|Item|EquipStats")
	float Magnitude = 0.0f;

public:
	FRSItemEquipStatModifier() = default;

	FRSItemEquipStatModifier(FGameplayAttribute InAttribute, ERSItemStatModifierOp InOp, float InMagnitude)
		: Attribute(InAttribute)
		, Operation(InOp)
		, Magnitude(InMagnitude)
	{
	}

	/** 유효한 Modifier인지 (Attribute가 세팅되어 있는지) */
	bool IsValid() const
	{
		return Attribute.IsValid();
	}
};

/**
 * 아이템 장착 시 적용할 스탯 보정을 담는 Fragment
 *
 * - 예)
 *   Sword:
 *     - AttackPower += 20
 *     - AttackRange += 50
 *   HeavyArmor:
 *     - Defense += 30
 *     - Poise   += 50
 *
 * - 실제 ASC에 반영하는 코드는 RSEquipManagerComponent / RSEquipmentManagerComponent 쪽에서 처리한다.
 */
UCLASS(BlueprintType, EditInlineNew, DefaultToInstanced)
class REMNANTSOUL_API URSItemFragment_EquipStats : public URSItemFragment
{
	GENERATED_BODY()

public:
	URSItemFragment_EquipStats(const FObjectInitializer& ObjectInitializer = FObjectInitializer::Get());

	/** 이 아이템이 장착될 때 적용할 Modifier 목록 */
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "RS|Item|EquipStats")
	TArray<FRSItemEquipStatModifier> Modifiers;

	/** 공격 거리 보정값 (+면 증가, -면 감소) */
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "RS|Item|EquipStats")
	float AttackRangeBonus = 0.f;

	/** 공격력 보정값 */
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "RS|Item|EquipStats")
	float AttackDamageBonus = 0.f;

public:
	/** 이 Fragment가 가진 모든 Modifier를 OutModifiers에 추가 */
	UFUNCTION(BlueprintCallable, Category = "RS|Item|EquipStats")
	void CollectModifiers(TArray<FRSItemEquipStatModifier>& OutModifiers) const;

	/**
	 * 특정 Attribute에 대해 Additive/Mul/Override를 따로 합산해서 리턴하는 유틸
	 * - Manager에서 "장착된 모든 아이템"에 대한 합산을 할 때 편의를 위해 제공
	 */
	UFUNCTION(BlueprintCallable, Category = "RS|Item|EquipStats")
	void GetAccumulatedForAttribute(
		FGameplayAttribute Attribute,
		float& OutAdditive,
		float& OutMultiplicative,
		bool& bHasOverride,
		float& OutOverrideValue
	) const;

#if WITH_EDITOR
	virtual EDataValidationResult IsDataValid(FDataValidationContext& Context) const override;
#endif
};
