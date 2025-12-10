#pragma once

#include "RSDefine.h"
#include "GameplayTagContainer.h"
#include "Blueprint/UserWidget.h"
#include "GameFramework/GameplayMessageSubsystem.h"
#include "RSEquipmentSlotsWidget.generated.h"

class UCommonVisibilitySwitcher;
class URSItemInstance;
class URSItemSlotWidget;
class URSEquipmentSlotWeaponWidget;
class URSEquipmentSlotSingleWidget;
class URSEquipManagerComponent;
class URSEquipmentManagerComponent;

USTRUCT(BlueprintType)
struct FEquipmentInitializeMessage
{
	GENERATED_BODY()

public:
	UPROPERTY(BlueprintReadWrite)
	TObjectPtr<URSEquipmentManagerComponent> EquipmentManager;

	UPROPERTY(BlueprintReadWrite)
	TObjectPtr<URSEquipManagerComponent> EquipManager;
};

UCLASS()
class URSEquipmentSlotsWidget : public UUserWidget
{
	GENERATED_BODY()
	
public:
	URSEquipmentSlotsWidget(const FObjectInitializer& ObjectInitializer = FObjectInitializer::Get());

protected:
	virtual void NativeOnInitialized() override;
	virtual void NativeConstruct() override;
	virtual void NativeDestruct() override;
	
private:
	void ConstructUI(FGameplayTag Channel, const FEquipmentInitializeMessage& Message);
	void DestructUI();
	
	void OnEquipmentEntryChanged(EEquipmentSlotType EquipmentSlotType, URSItemInstance* ItemInstance, int32 ItemCount);
	void OnEquipStateChanged(EEquipState PrevEquipState, EEquipState NewEquipState);

public:
	UPROPERTY(EditAnywhere, meta=(Categories="Message"))
	FGameplayTag MessageChannelTag;
	
private:
	UPROPERTY()
	TArray<TObjectPtr<URSEquipmentSlotWeaponWidget>> WeaponSlotWidgets;

	UPROPERTY()
	TArray<TObjectPtr<URSEquipmentSlotSingleWidget>> ArmorSlotWidgets;

	UPROPERTY()
	TArray<TObjectPtr<URSEquipmentSlotSingleWidget>> UtilitySlotWidgets;
	
	UPROPERTY()
	TObjectPtr<URSEquipmentManagerComponent> EquipmentManager;
	
	UPROPERTY()
	TObjectPtr<URSEquipManagerComponent> EquipManager;

protected:
	UPROPERTY(meta=(BindWidget))
	TObjectPtr<URSEquipmentSlotWeaponWidget> Widget_Weapon_Primary;
	
	UPROPERTY(meta=(BindWidget))
	TObjectPtr<URSEquipmentSlotWeaponWidget> Widget_Weapon_Secondary;

	UPROPERTY(meta=(BindWidget))
	TObjectPtr<URSEquipmentSlotSingleWidget> Widget_Armor_Head;

	UPROPERTY(meta=(BindWidget))
	TObjectPtr<URSEquipmentSlotSingleWidget> Widget_Armor_Chest;

	UPROPERTY(meta=(BindWidget))
	TObjectPtr<URSEquipmentSlotSingleWidget> Widget_Armor_Legs;

	UPROPERTY(meta=(BindWidget))
	TObjectPtr<URSEquipmentSlotSingleWidget> Widget_Armor_Hand;

	UPROPERTY(meta=(BindWidget))
	TObjectPtr<URSEquipmentSlotSingleWidget> Widget_Armor_Foot;

	UPROPERTY(meta=(BindWidget))
	TObjectPtr<URSEquipmentSlotSingleWidget> Widget_Utility_Primary;

	UPROPERTY(meta=(BindWidget))
	TObjectPtr<URSEquipmentSlotSingleWidget> Widget_Utility_Secondary;

	UPROPERTY(meta=(BindWidget))
	TObjectPtr<URSEquipmentSlotSingleWidget> Widget_Utility_Tertiary;

	UPROPERTY(meta=(BindWidget))
	TObjectPtr<URSEquipmentSlotSingleWidget> Widget_Utility_Quaternary;

	UPROPERTY(meta=(BindWidget))
	TObjectPtr<UCommonVisibilitySwitcher> Switcher_Weapon_Primary;

	UPROPERTY(meta=(BindWidget))
	TObjectPtr<UCommonVisibilitySwitcher> Switcher_Weapon_Secondary;

private:
	FDelegateHandle EntryChangedDelegateHandle;
	FDelegateHandle EquipStateChangedDelegateHandle;
	FGameplayMessageListenerHandle MessageListenerHandle;
};
