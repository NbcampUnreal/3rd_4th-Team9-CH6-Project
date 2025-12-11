#pragma once

#include "Blueprint/UserWidget.h"
#include "RSCheatEntryWidget.generated.h"

class USizeBox;
class UImage;
class UButton;
class UCommonTextBlock;
class URSItemTemplate;

UENUM(BlueprintType)
enum class ERSCheatEntryType : uint8
{
	None,
	PrimaryWeapon,
	SecondaryWeapon,
	Armor,
	Utility,
	Animation
};

UCLASS()
class URSCheatEntryWidget : public UUserWidget
{
	GENERATED_BODY()
	
public:
	URSCheatEntryWidget(const FObjectInitializer& ObjectInitializer = FObjectInitializer::Get());

protected:
	virtual void NativeOnInitialized() override;
	
public:
	void InitializeUI(ERSCheatEntryType InCheatEntryType, TSubclassOf<URSItemTemplate> InItemTemplateClass, TSoftObjectPtr<UAnimMontage> InAnimMontage);

private:
	UFUNCTION()
	void OnButtonClicked();
	
public:
	UPROPERTY()
	ERSCheatEntryType CheatEntryType = ERSCheatEntryType::None;

	UPROPERTY()
	TSubclassOf<URSItemTemplate> ItemTemplateClass;

	UPROPERTY()
	TSoftObjectPtr<UAnimMontage> AnimMontage;

protected:
	UPROPERTY(meta=(BindWidget))
	TObjectPtr<USizeBox> SizeBox_Entry;
	
	UPROPERTY(meta=(BindWidget))
	TObjectPtr<UButton> Button_Entry;

	UPROPERTY(meta=(BindWidget))
	TObjectPtr<UImage> Image_Entry;
	
	UPROPERTY(meta=(BindWidget))
	TObjectPtr<UCommonTextBlock> Text_Entry;
};
