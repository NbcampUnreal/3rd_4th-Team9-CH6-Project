// Fill out your copyright notice in the Description page of Project Settings.


#include "IngameUI/RS_VideoWidget.h"
#include "MediaPlayer.h"
#include "MediaSource.h"
#include "Components/AudioComponent.h"
#include "Sound/SoundBase.h"
#include "Kismet/GameplayStatics.h"
#include "TimerManager.h"

void URS_VideoWidget::NativeConstruct()
{
	Super::NativeConstruct();

	SetVisibility(ESlateVisibility::Hidden);

	if (MediaPlayer)
	{
		MediaPlayer->OnEndReached.AddDynamic(
			this,
			&URS_VideoWidget::OnMediaEndReached
		);
	}

	if (!AudioComponent)
	{
		AudioComponent = NewObject<UAudioComponent>(this);

		if (AudioComponent)
		{
			AudioComponent->bAutoActivate = false;
			AudioComponent->bAutoDestroy = false;

			if (UWorld* World = GetWorld())
			{
				AudioComponent->RegisterComponentWithWorld(World);
			}
			// World가 없어도 생성은 유지됨 (Play 시점에 다시 등록 가능)
		}
	}

	if (DefaultMediaSource)
	{
		PlayVideo(DefaultMediaSource);
	}
}

void URS_VideoWidget::NativeDestruct()
{
	if (MediaPlayer)
	{
		MediaPlayer->OnEndReached.RemoveAll(this);
		MediaPlayer->Close();
	}
	
	if (AudioComponent)
	{
		AudioComponent->Stop();
		AudioComponent->DestroyComponent();
		AudioComponent = nullptr;
	}

	Super::NativeDestruct();
}


void URS_VideoWidget::PlayVideo(UMediaSource* InMediaSource)
{
	if (!MediaPlayer || !InMediaSource || bIsPlaying)
	{
		return;
	}

	bIsPlaying = true;
	bIsFadingOut = false;

	SetVisibility(ESlateVisibility::Visible);

	MediaPlayer->OpenSource(InMediaSource);
	MediaPlayer->Play();

	if (VideoSound && AudioComponent)
	{
		if (!AudioComponent->IsRegistered())
		{
			if (UWorld* World = GetWorld())
			{
				AudioComponent->RegisterComponentWithWorld(World);
			}
		}

		AudioComponent->SetSound(VideoSound);
		AudioComponent->Play();
	}
}

void URS_VideoWidget::OnMediaEndReached()
{
	if (!bIsPlaying || bIsFadingOut)
	{
		return;
	}

	StartFadeOut();
}

void URS_VideoWidget::StartFadeOut()
{
	if (bIsFadingOut)
	{
		return;
	}

	bIsFadingOut = true;
	bIsPlaying = false;

	/* 오디오 페이드 */
	if (AudioComponent && AudioComponent->IsPlaying())
	{
		AudioComponent->FadeOut(FadeOutDuration, 0.f);
	}

	/* 비주얼 페이드 */
	if (FadeOutAnim)
	{
		PlayAnimation(FadeOutAnim);

		const float FadeOutLength = FadeOutAnim->GetEndTime();

		GetWorld()->GetTimerManager().SetTimer(
			FadeOutTimer,
			this,
			&URS_VideoWidget::HandleFadeOutFinished,
			FadeOutLength,
			false
		);

		return;
	}

	/* 애니메이션 없으면 즉시 종료 */
	HandleFadeOutFinished();
}

void URS_VideoWidget::HandleFadeOutFinished()
{
	if (MediaPlayer)
	{
		MediaPlayer->Close();
	}

	OnVideoWidgetFinished.Broadcast();
	RemoveFromParent();
}

