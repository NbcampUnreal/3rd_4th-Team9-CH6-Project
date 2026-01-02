// Fill out your copyright notice in the Description page of Project Settings.


#include "IngameUI/RS_VictoryWidget.h"
#include "TimerManager.h"
#include "Blueprint/UserWidget.h"


void URS_VictoryWidget::NativeConstruct()
{
	Super::NativeConstruct();

	// 기본은 숨김 상태
	SetVisibility(ESlateVisibility::Hidden);
}


void URS_VictoryWidget::PlayVictorySequence()
{
	if (!bCanPlay || bIsPlaying)
	{
		return;
	}

	bIsPlaying = true;
	bCanPlay = false;

	SetVisibility(ESlateVisibility::Visible);

	if (FadeInAnim)
	{
		PlayAnimation(FadeInAnim);

		FWidgetAnimationDynamicEvent FinishedEvent;
		FinishedEvent.BindDynamic(this, &URS_VictoryWidget::OnFadeInFinished);

		BindToAnimationFinished(FadeInAnim, FinishedEvent);
	}
	else
	{
		OnFadeInFinished();
	}
}


void URS_VictoryWidget::OnFadeInFinished()
{
	// 5초 대기 후 FadeOut
	if (HoldTime > 0.f)
	{
		GetWorld()->GetTimerManager().SetTimer(
			HoldTimer,
			this,
			&URS_VictoryWidget::StartFadeOut,
			HoldTime,
			false
		);
	}
	else
	{
		StartFadeOut();
	}
}


void URS_VictoryWidget::StartFadeOut()
{
	if (FadeOutAnim)
	{
		PlayAnimation(FadeOutAnim);

		FWidgetAnimationDynamicEvent FinishedEvent;
		FinishedEvent.BindDynamic(this, &URS_VictoryWidget::OnFadeOutFinished);

		BindToAnimationFinished(FadeOutAnim, FinishedEvent);
	}
	else
	{
		OnFadeOutFinished();
	}
}

void URS_VictoryWidget::OnFadeOutFinished()
{
	bIsPlaying = false;
	bCanPlay = true;

	OnVictoryWidgetFinished.Broadcast(); 
	RemoveFromParent();
}