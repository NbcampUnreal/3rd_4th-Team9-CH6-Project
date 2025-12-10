#include "RSClassEntryWidget.h"

#include "RSClassSelectionWidget.h"
#include "Components/Image.h"
#include "Components/TextBlock.h"
#include "Components/VerticalBox.h"
#include "RSClassSkillEntryWidget.h"
#include "AbilitySystem/LyraAbilitySet.h"
#include "Components/Button.h"
#include "Data/RSClassData.h"
#include "Player/LyraPlayerState.h"

#include UE_INLINE_GENERATED_CPP_BY_NAME(RSClassEntryWidget)

URSClassEntryWidget::URSClassEntryWidget(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
{
    
}

void URSClassEntryWidget::InitializeUI(URSClassSelectionWidget* OwnerWidget, ECharacterClassType ClassType)
{
	CachedClassType = ClassType;
	CachedOwnerWidget = OwnerWidget;

	const FRSClassInfoEntry& ClassEntry = URSClassData::Get().GetClassInfoEntry(ClassType);
	
	Text_ClassName->SetText(ClassEntry.ClassName);
	VerticalBox_SkillElements->ClearChildren();
	
	if (ULyraAbilitySet* AbilitySet = ClassEntry.ClassAbilitySet)
	{
		const TArray<FLyraAbilitySet_GameplayAbility>& AbilitySetAbilities = AbilitySet->GetGrantedGameplayAbilities();
		for (int i = 0; i < 2; i++)
		{
			if (AbilitySetAbilities.IsValidIndex(i))
			{
				const FLyraAbilitySet_GameplayAbility& AbilitySetAbility = AbilitySetAbilities[i];
				URSClassSkillEntryWidget* SkillEntryWidget = CreateWidget<URSClassSkillEntryWidget>(this, SkillEntryWidgetClass);
				SkillEntryWidget->InitializeUI(AbilitySetAbility.Ability);
				VerticalBox_SkillElements->AddChild(SkillEntryWidget);
			}
		}
	}

	Button_Class->OnClicked.AddUniqueDynamic(this, &ThisClass::OnButtonClicked);
}

void URSClassEntryWidget::OnButtonClicked()
{
	if (ALyraPlayerState* LyraPlayerState = Cast<ALyraPlayerState>(GetOwningPlayerState()))
	{
		LyraPlayerState->Server_SelectClass(CachedClassType);
	}

	if (URSClassSelectionWidget* ClassSelectionWidget = CachedOwnerWidget.Get())
	{
		ClassSelectionWidget->DeactivateWidget();
		ClassSelectionWidget = nullptr;
	}
}
