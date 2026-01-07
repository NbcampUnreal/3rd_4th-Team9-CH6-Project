// Fill out your copyright notice in the Description page of Project Settings.


#include "IngameUI/RS_VideoWidget.h"
#include "Animation/UMGSequencePlayer.h"
#include "MediaPlayer.h"
#include "MediaSource.h"

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

		// 생성 직후 자동 재생 (컷신용)
		if (DefaultMediaSource)
		{
			PlayVideo(DefaultMediaSource);
		}
	}
}

void URS_VideoWidget::NativeDestruct()
{
	if (MediaPlayer)
	{
		MediaPlayer->OnEndReached.RemoveAll(this);
		MediaPlayer->Close();
	}

	Super::NativeDestruct();
}

void URS_VideoWidget::PlayVideo(UMediaSource* InMediaSource)
{
	if (!MediaPlayer || !InMediaSource || bIsPlaying)
	{
		return;
	}
	
	if (bIsPlaying)
	{
		UE_LOG(LogTemp, Warning, TEXT("[VideoWidget] Already playing"));
		return;
	}
	
	bIsPlaying = true;
	bIsFadingOut = false;

	SetVisibility(ESlateVisibility::Visible);

	MediaPlayer->OpenSource(InMediaSource);
	MediaPlayer->Play();
}

void URS_VideoWidget::OnMediaEndReached()
{

	if (!bIsPlaying)
	{
		return;
	}
	
	bIsPlaying = false;

	// 위젯 종료 이벤트 브로드캐스트
	OnVideoWidgetFinished.Broadcast();

	// 위젯 제거
	RemoveFromParent();

}
