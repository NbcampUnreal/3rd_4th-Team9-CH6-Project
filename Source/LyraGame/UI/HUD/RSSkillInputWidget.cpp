#include "RSSkillInputWidget.h"

#include UE_INLINE_GENERATED_CPP_BY_NAME(RSSkillInputWidget)

URSSkillInputWidget::URSSkillInputWidget(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
{
    
}

void URSSkillInputWidget::NativeConstruct()
{
	Super::NativeConstruct();

	SetVisibility(ESlateVisibility::Collapsed);
	
	UGameplayMessageSubsystem& MessageSubsystem = UGameplayMessageSubsystem::Get(this);
	ListenerHandle = MessageSubsystem.RegisterListener(MessageChannelTag, this, &ThisClass::ConstructUI);
}

void URSSkillInputWidget::NativeDestruct()
{
	UGameplayMessageSubsystem& MessageSubsystem = UGameplayMessageSubsystem::Get(this);
	MessageSubsystem.UnregisterListener(ListenerHandle);
	
	Super::NativeDestruct();
}

void URSSkillInputWidget::ConstructUI(FGameplayTag Channel, const FSkillInputInitializeMessage& Message)
{
	Message.bShouldShow ? SetVisibility(ESlateVisibility::Visible) : SetVisibility(ESlateVisibility::Hidden);
}
