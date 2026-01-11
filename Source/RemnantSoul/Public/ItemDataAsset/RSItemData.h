// Fill out your copyright notice in the Description page of Project Settings.

// RSItemData.h

#pragma once

#include "CoreMinimal.h"
#include "Engine/DataAsset.h"
#include "RSItemData.generated.h"

class UTexture2D;
class UGameplayEffect;
class USoundBase;

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
	
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category="Item|Use")
	TSubclassOf<UGameplayEffect> UseGameplayEffect;

	
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category="Item|Use")
	bool bConsumeOnUse = true;


	UFUNCTION(BlueprintCallable, Category="Item|Use")
	bool TryUse(AActor* User) const;
	
	
	/* =========================
 * 사운드 (아이템 사용 효과음)
 * ========================= */
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category="Item|Audio")
	TSoftObjectPtr<USoundBase> UseSFX;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category="Item|Audio")
	bool bPlayUseSFX2D = false;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category="Item|Audio", meta=(ClampMin="0.0"))
	float UseSFXVolume = 1.0f;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category="Item|Audio", meta=(ClampMin="0.0"))
	float UseSFXPitch = 1.0f;

private:
	void PlayUseSFX(AActor* User) const;
	
	bool ApplyGameplayEffectToUser(AActor* User) const;
};
