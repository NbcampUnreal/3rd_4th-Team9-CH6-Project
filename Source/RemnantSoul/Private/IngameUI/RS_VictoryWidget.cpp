// Fill out your copyright notice in the Description page of Project Settings.


#include "IngameUI/RS_VictoryWidget.h"
#include "TimerManager.h"
#include "Blueprint/UserWidget.h"
#include "Kismet/GameplayStatics.h"
#include "Components/AudioComponent.h"


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
	bIsFadingOut = false;
	
	SetVisibility(ESlateVisibility::Visible);


	//사운드 생성
	if (VictorySound)
	{
		VictoryAudioComponent =
			UGameplayStatics::SpawnSound2D(
				this,
				VictorySound,
				0.75f,
				1.f,
				0.f,
				nullptr,
				true // bIsUISound
			);
	}

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

	if (bIsFadingOut)
	{
		return;
	}
	
	// HoldTime만큼 대기 후 FadeOut
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
	if (bIsFadingOut)
	{
		return;
	}

	bIsFadingOut = true;

	//페이드 아웃 시 사운드 종료
	StopVictorySound(false);
	
	

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

void URS_VictoryWidget::ForceFadeOut()
{
	// Ability Cancel 대응
	if (!bIsPlaying || bIsFadingOut)
	{
		return;
	}
	
	StopVictorySound(true);
	StartFadeOut();
}


void URS_VictoryWidget::OnFadeOutFinished()
{
	bIsPlaying = false;
	bCanPlay = true;

	StopVictorySound(true);
	
	OnVictoryWidgetFinished.Broadcast(); 
	RemoveFromParent();
}


FReply URS_VictoryWidget::NativeOnMouseButtonDown(
	const FGeometry& InGeometry,
	const FPointerEvent& InMouseEvent
)
{
	// Hold 중 클릭 → 즉시 종료
	if (bIsPlaying && !bIsFadingOut &&
		GetWorld()->GetTimerManager().IsTimerActive(HoldTimer))
	{
		StartFadeOut();
		return FReply::Handled();
	}

	return Super::NativeOnMouseButtonDown(InGeometry, InMouseEvent);
}

//실제 사운드를 종료하는 함수
void URS_VictoryWidget::StopVictorySound(bool bImmediate)
{
	if (!VictoryAudioComponent)
	{
		return;
	}

	if (bImmediate)
	{
		VictoryAudioComponent->Stop();
	}
	else
	{
		VictoryAudioComponent->FadeOut(2.0f, 0.f);
	}

	VictoryAudioComponent = nullptr;

}