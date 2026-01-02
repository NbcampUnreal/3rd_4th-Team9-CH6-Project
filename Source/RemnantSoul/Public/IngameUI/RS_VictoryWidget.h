// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "RS_VictoryWidget.generated.h"

DECLARE_DYNAMIC_MULTICAST_DELEGATE(FOnVictoryWidgetFinished);

UCLASS()
class REMNANTSOUL_API URS_VictoryWidget : public UUserWidget
{
	GENERATED_BODY()

public:
	UFUNCTION(BlueprintCallable)
	void PlayVictorySequence();

	UFUNCTION(BlueprintCallable)
	bool CanPlay() const { return bCanPlay; }

	//승리 위젯을 불러올 어빌리티에 위젯 애니메이션이 종료되었음을 알리는 로직
	UPROPERTY(BlueprintAssignable)
	FOnVictoryWidgetFinished OnVictoryWidgetFinished;
	
protected:

	virtual void NativeConstruct() override;

	UPROPERTY(meta = (BindWidgetAnim), Transient)
	UWidgetAnimation* FadeInAnim;

	UPROPERTY(meta = (BindWidgetAnim), Transient)
	UWidgetAnimation* FadeOutAnim;

	UPROPERTY(meta = (BindWidgetAnim), Transient)
	float HoldTime = 3.0f;

private:

	UFUNCTION()
	void OnFadeInFinished();
	
	UFUNCTION()
	void OnFadeOutFinished();

	void StartFadeOut();


	FTimerHandle HoldTimer;
	
	bool bIsPlaying = false;

	bool bCanPlay = true;
};
