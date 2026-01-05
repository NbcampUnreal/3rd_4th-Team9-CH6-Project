// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "RS_VictoryWidget.generated.h"

DECLARE_DYNAMIC_MULTICAST_DELEGATE(FOnVictoryWidgetFinished);

class UWidgetAnimation;
class UAudioComponent;
class USoundBase;


UCLASS()
class REMNANTSOUL_API URS_VictoryWidget : public UUserWidget
{
	GENERATED_BODY()

public:

	//승리 위젯 출력 중 클릭 시 FadeOut으로 즉시 스킵
	virtual FReply NativeOnMouseButtonDown(const FGeometry& InGeometry,const FPointerEvent& InMouseEvent) override;
	
	UFUNCTION(BlueprintCallable)
	void PlayVictorySequence();

	UFUNCTION(BlueprintCallable)
	bool CanPlay() const { return bCanPlay; }

	void ForceFadeOut();

	//승리 위젯을 불러올 어빌리티에 위젯 애니메이션이 종료되었음을 알리는 로직
	UPROPERTY(BlueprintAssignable, Category = "Victory")
	FOnVictoryWidgetFinished OnVictoryWidgetFinished;
	
protected:

	virtual void NativeConstruct() override;

	UPROPERTY(meta = (BindWidgetAnim), Transient)
	UWidgetAnimation* FadeInAnim;

	UPROPERTY(meta = (BindWidgetAnim), Transient)
	UWidgetAnimation* FadeOutAnim;

	UPROPERTY(meta = (BindWidgetAnim), Transient)
	float HoldTime = 3.0f;

	//사운드 관련 함수
	UPROPERTY(EditDefaultsOnly, Category = "Audio")
	USoundBase* VictorySound;

	UPROPERTY(Transient)
	UAudioComponent* VictoryAudioComponent = nullptr;
	
	void StopVictorySound(bool bImmediate);

private:

	UFUNCTION()
	void OnFadeInFinished();
	
	UFUNCTION()
	void OnFadeOutFinished();

	void StartFadeOut();


	FTimerHandle HoldTimer;
	
	bool bIsPlaying = false;

	bool bCanPlay = true;

	bool bIsFadingOut = false;

};
