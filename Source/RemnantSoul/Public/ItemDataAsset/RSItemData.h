// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Engine/DataAsset.h"
#include "RSItemData.generated.h"


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
    
        // 아이템 고유 ID (검색/저장용)
        UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Item|ID")
        FName ItemID;
    
        /* =========================
         * 표시 정보 (UI)
         * ========================= */
    
        // 인벤토리/툴팁에 표시될 이름
        UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Item|Display")
        FText DisplayName;
    
        // 아이콘 (SoftObject 권장)
        UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Item|Display")
        TSoftObjectPtr<UTexture2D> Icon;
    
        // 설명 텍스트
        UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Item|Display", meta=(MultiLine=true))
        FText Description;
    
        /* =========================
         * 분류 / 인벤토리 규칙
         * ========================= */
    
        // 아이템 분류
        UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Item|Category")
        EItemCategory Category = EItemCategory::Material;
    
        // 최대 스택 수
        UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Item|Stack", meta=(ClampMin="1"))
        int32 MaxStack = 1;
	
};

