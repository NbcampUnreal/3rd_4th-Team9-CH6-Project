// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "MainMenuWidget.generated.h"

/**
 * 
 */
UCLASS()
class REMNANTSOUL_API UMainMenuWidget : public UUserWidget
{
	GENERATED_BODY()
	

protected:

	virtual void NativeConstruct() override;

	UPROPERTY(meta = (BindWidget))
		class UButton* StartGameButton;

	// 종료 버튼
	UPROPERTY(meta = (BindWidget))
	class UButton* QuitGameButton;

	// 환경설정버튼
	UPROPERTY(meta = (BindWidget))
	class UButton* SettingsButton;

	// 환경설정에서 시작 메뉴로 돌아가는 버튼
	UPROPERTY(meta = (BindWidget))
	class UButton* BackButton;

	// 화면 전환 장치
	UPROPERTY(meta = (BindWidget))
	class UWidgetSwitcher* MenuSwitcher;

	// 게임 시작 함수
	UFUNCTION()
	void OnStartGameClicked();

	// 게임 종료 함수
	UFUNCTION()
	void OnQuitGameClicked();

	// 환경설정 버튼 클릭 시 실행
	UFUNCTION()
	void OnSettingsClicked();

	// 뒤로 가기 버튼 클릭 시 실행
	UFUNCTION()
	void OnBackClicked();

public:
	// 이동할 레벨의 이름
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Level")
	FName LevelToLoad = FName("Cave");

	//그래픽 설정 관련 함수
	UFUNCTION(BlueprintCallable, Category = "Settings")
	void SetGraphicsQuality(int32 QualityLevel);  // 그래픽 품질 변경 0~3까지

	UFUNCTION(BlueprintPure, Category = "Settings")
	int32 GetCurrentQualityLevel() const; // 현재 그래픽 상태

};
