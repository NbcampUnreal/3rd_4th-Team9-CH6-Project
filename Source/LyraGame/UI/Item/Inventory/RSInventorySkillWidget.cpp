#include "RSInventorySkillWidget.h"

#include "AbilitySystemBlueprintLibrary.h"
#include "GameplayAbilitySpec.h"
#include "Abilities/GameplayAbility.h"
#include "AbilitySystem/LyraAbilitySystemComponent.h"
#include "Character/LyraPawnExtensionComponent.h"
#include "Components/Image.h"
#include "Data/RSUIData.h"
#include "UI/Item/RSSkillStatHoverWidget.h"

#include UE_INLINE_GENERATED_CPP_BY_NAME(RSInventorySkillWidget)

URSInventorySkillWidget::URSInventorySkillWidget(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
{
    
}

void URSInventorySkillWidget::NativeConstruct()
{
	Super::NativeConstruct();

	if (ULyraPawnExtensionComponent* PawnExtensionComponent = ULyraPawnExtensionComponent::FindPawnExtensionComponent(GetOwningPlayerPawn()))
	{
		PawnExtensionComponent->OnAbilitySystemInitialized_RegisterAndCall(FSimpleMulticastDelegate::FDelegate::CreateUObject(this, &ThisClass::OnAbilitySystemInitialized));
	}
}

void URSInventorySkillWidget::NativeDestruct()
{
	if (ULyraAbilitySystemComponent* LyraASC = Cast<ULyraAbilitySystemComponent>(UAbilitySystemBlueprintLibrary::GetAbilitySystemComponent(GetOwningPlayerPawn())))
	{
		LyraASC->AbilityChangedDelegate.Remove(AbilityDelegateHandle);
		AbilityDelegateHandle.Reset();
	}

	if (HoverWidget)
	{
		HoverWidget->RemoveFromParent();
		HoverWidget = nullptr;
	}
	
	Super::NativeDestruct();
}

void URSInventorySkillWidget::NativeOnMouseEnter(const FGeometry& InGeometry, const FPointerEvent& InMouseEvent)
{
	Super::NativeOnMouseEnter(InGeometry, InMouseEvent);

	if (FGameplayAbilitySpec* AbilitySpec = FindAbilitySpecFromHandle(CachedAbilitySpecHandle))
	{
		if (ULyraGameplayAbility* Ability = Cast<ULyraGameplayAbility>(AbilitySpec->Ability))
		{
			if (HoverWidget == nullptr)
			{
				TSubclassOf<URSSkillStatHoverWidget> HoverWidgetClass = URSUIData::Get().SkillStatHoverWidget;
				HoverWidget = CreateWidget<URSSkillStatHoverWidget>(GetOwningPlayer(), HoverWidgetClass);
			}

			if (HoverWidget)
			{
				HoverWidget->RefreshUI(Ability->Name, Ability->Description);
				HoverWidget->AddToViewport();
			}
		}
	}
}

FReply URSInventorySkillWidget::NativeOnMouseMove(const FGeometry& InGeometry, const FPointerEvent& InMouseEvent)
{
	FReply Reply = Super::NativeOnMouseMove(InGeometry, InMouseEvent);

	if (HoverWidget)
	{
		HoverWidget->SetPosition(InMouseEvent.GetScreenSpacePosition());
		return FReply::Handled();
	}
	
	return Reply;
}

void URSInventorySkillWidget::NativeOnMouseLeave(const FPointerEvent& InMouseEvent)
{
	Super::NativeOnMouseLeave(InMouseEvent);

	if (HoverWidget)
	{
		HoverWidget->RemoveFromParent();
		HoverWidget = nullptr;
	}
}

void URSInventorySkillWidget::InitializeUI()
{
	if (FGameplayAbilitySpec* AbilitySpec = FindAbilitySpecFromHandle(CachedAbilitySpecHandle))
	{
		if (ULyraGameplayAbility* Ability = Cast<ULyraGameplayAbility>(AbilitySpec->Ability))
		{
			Image_Icon->SetBrushFromTexture(Ability->Icon);
		}
	}
}

FGameplayAbilitySpec* URSInventorySkillWidget::FindAbilitySpecFromHandle(FGameplayAbilitySpecHandle AbilitySpecHandle)
{
	if (UAbilitySystemComponent* ASC = UAbilitySystemBlueprintLibrary::GetAbilitySystemComponent(GetOwningPlayerPawn()))
	{
		if (FGameplayAbilitySpec* AbilitySpec = ASC->FindAbilitySpecFromHandle(AbilitySpecHandle))
		{
			return AbilitySpec;
		}
	}
	return nullptr;
}

void URSInventorySkillWidget::OnAbilitySystemInitialized()
{
	if (ULyraAbilitySystemComponent* LyraASC = Cast<ULyraAbilitySystemComponent>(UAbilitySystemBlueprintLibrary::GetAbilitySystemComponent(GetOwningPlayerPawn())))
	{
		for (const FGameplayAbilitySpec& AbilitySpec : LyraASC->GetActivatableAbilities())
		{
			if (AbilitySpec.Ability->AbilityTags.HasTagExact(SkillSlotTag))
			{
				CachedAbilitySpecHandle = AbilitySpec.Handle;
				InitializeUI();
				break;
			}
		}
		
		AbilityDelegateHandle = LyraASC->AbilityChangedDelegate.AddUObject(this, &ThisClass::OnAbilityChanged);
	}
}

void URSInventorySkillWidget::OnAbilityChanged(FGameplayAbilitySpecHandle AbilitySpecHandle, bool bGiven)
{
	FGameplayAbilitySpec* AbilitySpec = FindAbilitySpecFromHandle(AbilitySpecHandle);
	if (AbilitySpec && AbilitySpec->Ability->AbilityTags.HasTagExact(SkillSlotTag))
	{
		CachedAbilitySpecHandle = bGiven ? AbilitySpec->Handle : FGameplayAbilitySpecHandle();
		InitializeUI();
	}
}
