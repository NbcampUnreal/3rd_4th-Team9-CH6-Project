// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "Animation/WidgetAnimation.h"
#include "Animation/WidgetAnimationState.h"
#include "RS_VideoWidget.generated.h"

class UMediaPlayer;
class UMediaSource;
class UWidgetAnimation;
class UUMGSequencePlayer;
class USoundBase;
class UAudioComponent;

DECLARE_DYNAMIC_MULTICAST_DELEGATE(FOnVideoWidgetFinished);

UCLASS()
class REMNANTSOUL_API URS_VideoWidget : public UUserWidget
{
	GENERATED_BODY()

public:
	UFUNCTION(BlueprintCallable)
	void PlayVideo(UMediaSource* InMediaSource);

	UPROPERTY(BlueprintAssignable)
	FOnVideoWidgetFinished OnVideoWidgetFinished;

protected:
	virtual void NativeConstruct() override;
	virtual void NativeDestruct() override;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Media")
	UMediaPlayer* MediaPlayer = nullptr;

	UPROPERTY(EditDefaultsOnly, Category = "Video")
	UMediaSource* DefaultMediaSource;
	
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Audio")
	USoundBase* VideoSound = nullptr;

	UPROPERTY(Transient)
	UAudioComponent* AudioComponent = nullptr;

	UPROPERTY(meta = (BindWidgetAnim), Transient)
	UWidgetAnimation* FadeOutAnim = nullptr;

	UPROPERTY(EditDefaultsOnly, Category = "Fade")
	float FadeOutDuration = 1.0f;

	bool bIsPlaying = false;
	bool bIsFadingOut = false;

	UFUNCTION()
	void OnMediaEndReached();

	void StartFadeOut();

	UFUNCTION()
	void HandleFadeOutFinished();

	FTimerHandle FadeOutTimer;

};
