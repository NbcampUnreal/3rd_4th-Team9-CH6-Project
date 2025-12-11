#include "RSNumberPopWidgetDefault.h"

#include "LyraLogChannels.h"
#include "Animation/UMGSequencePlayer.h"
#include "Components/TextBlock.h"

#include UE_INLINE_GENERATED_CPP_BY_NAME(RSNumberPopWidgetDefault)

URSNumberPopWidgetDefault::URSNumberPopWidgetDefault(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
{
    
}

void URSNumberPopWidgetDefault::InitializeUI(int32 InNumberToDisplay, FLinearColor InColor)
{
	Super::InitializeUI(InNumberToDisplay, InColor);
	
	Text_Number->SetColorAndOpacity(InColor);
	Text_Number->SetText(FText::AsNumber(InNumberToDisplay));
	
	GetWorld()->GetTimerManager().SetTimerForNextTick([this]()
	{
		PlayAnimation(Animation_NumberPop);
	});
}
