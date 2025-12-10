#pragma once

#include "RSDefine.h"
#include "GameplayTagContainer.h"
#include "RSUIData.generated.h"

class UImage;
class URSSkillStatHoverWidget;
class URSInventoryValidWidget;
class URSItemDragWidget;
class URSItemHoverWidget;
class URSEquipmentEntryWidget;
class URSInventorySlotWidget;
class URSInventoryEntryWidget;

USTRUCT(BlueprintType)
struct FRSItemRarityInfoEntry
{
	GENERATED_BODY()

public:
	UPROPERTY(EditDefaultsOnly, meta = (HideAlphaChannel))
	FColor Color = FColor::Black;

	UPROPERTY(EditDefaultsOnly)
	TObjectPtr<UTexture2D> EntryTexture;

	UPROPERTY(EditDefaultsOnly)
	TObjectPtr<UTexture2D> HoverTexture;
};

USTRUCT(BlueprintType)
struct FRSUIInfo
{
	GENERATED_BODY()

public:
	UPROPERTY(EditDefaultsOnly)
	TObjectPtr<UTexture2D> Icon;

	UPROPERTY(EditDefaultsOnly)
	FText Title;

	UPROPERTY(EditDefaultsOnly)
	FText Content;
};

UCLASS(BlueprintType, Const, meta = (DisplayName = "RS UI Data"))
class URSUIData : public UPrimaryDataAsset
{
	GENERATED_BODY()

public:
	static const URSUIData& Get();

public:
	UTexture2D* GetEntryRarityTexture(EItemRarity ItemRarity) const;
	UTexture2D* GetHoverRarityTexture(EItemRarity ItemRarity) const;
	FColor GetRarityColor(EItemRarity ItemRarity) const;

	const FRSUIInfo& GetTagUIInfo(FGameplayTag Tag) const;

public:
	UPROPERTY(EditDefaultsOnly)
	FIntPoint UnitInventorySlotSize = FIntPoint::ZeroValue;

public:
	UPROPERTY(EditDefaultsOnly)
	TSubclassOf<URSItemDragWidget> DragWidgetClass;

	UPROPERTY(EditDefaultsOnly)
	TSubclassOf<URSItemHoverWidget> ItemHoverWidgetClass;

	UPROPERTY(EditDefaultsOnly)
	TSubclassOf<URSSkillStatHoverWidget> SkillStatHoverWidget;

	UPROPERTY(EditDefaultsOnly)
	TSubclassOf<URSEquipmentEntryWidget> EquipmentEntryWidgetClass;

	UPROPERTY(EditDefaultsOnly)
	TSubclassOf<URSInventorySlotWidget> InventorySlotWidgetClass;

	UPROPERTY(EditDefaultsOnly)
	TSubclassOf<URSInventoryEntryWidget> InventoryEntryWidgetClass;

	UPROPERTY(EditDefaultsOnly)
	TSubclassOf<URSInventoryValidWidget> InventoryValidWidgetClass;

private:
	UPROPERTY(EditDefaultsOnly, meta = (ArraySizeEnum = "EItemRarity"))
	FRSItemRarityInfoEntry RarityInfoEntries[(int32)EItemRarity::Count];

	UPROPERTY(EditDefaultsOnly, meta = (DisplayName = "Tag UI Infos"))
	TMap<FGameplayTag, FRSUIInfo> TagUIInfos;
};
