// Fill out your copyright notice in the Description page of Project Settings.

// RSItemData.h

#pragma once

#include "CoreMinimal.h"
#include "Engine/DataAsset.h"
#include "RSItemData.generated.h"

class UTexture2D;
class UGameplayEffect;

UENUM(BlueprintType)
enum class EItemCategory : uint8
{
	Potion      UMETA(DisplayName = "Potion"),
	Material    UMETA(DisplayName = "Material"),
	Equipment   UMETA(DisplayName = "Equipment"),
	Quest       UMETA(DisplayName = "Quest"),
	Key         UMETA(DisplayName = "Key"),
};

UCLASS()
class REMNANTSOUL_API URSItemData : public UDataAsset
{
	GENERATED_BODY()

public:
	/* =========================
	 * 식별 정보
	 * ========================= */
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Item|ID")
	FName ItemID;

	/* =========================
	 * 표시 정보 (UI)
	 * ========================= */
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Item|Display")
	FText DisplayName;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Item|Display")
	TSoftObjectPtr<UTexture2D> Icon;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Item|Display", meta=(MultiLine=true))
	FText Description;

	/* =========================
	 * 분류 / 인벤토리 규칙
	 * ========================= */
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Item|Category")
	EItemCategory Category = EItemCategory::Material;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Item|Stack", meta=(ClampMin="1"))
	int32 MaxStack = 1;

	/* =========================
	 * 사용(포션 등) 로직
	 * ========================= */

	// 사용 시 자기 자신에게 적용할 GameplayEffect (포션이면 보통 Heal GE)
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category="Item|Use")
	TSubclassOf<UGameplayEffect> UseGameplayEffect;

	// 사용 성공 시 소비할지 여부(포션=대부분 true)
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category="Item|Use")
	bool bConsumeOnUse = true;

	// 실제 사용 시도(성공/실패 반환)
	UFUNCTION(BlueprintCallable, Category="Item|Use")
	bool TryUse(AActor* User) const;

private:
	bool ApplyGameplayEffectToUser(AActor* User) const;
};
