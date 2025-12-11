#include "RSSkillStatHoverEntryWidget.h"

#include "Components/RichTextBlock.h"
#include "Components/TextBlock.h"

#include UE_INLINE_GENERATED_CPP_BY_NAME(RSSkillStatHoverEntryWidget)

URSSkillStatHoverEntryWidget::URSSkillStatHoverEntryWidget(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
{
    
}

void URSSkillStatHoverEntryWidget::RefreshUI(FText Name, FText Description)
{
	Text_Name->SetText(Name);
	Text_Description->SetText(Description);

	PlayAnimationForward(Animation_FadeIn);
}
