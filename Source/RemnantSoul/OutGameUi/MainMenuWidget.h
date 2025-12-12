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

	// 종료 버튼 (선택 사항)
	UPROPERTY(meta = (BindWidget))
	class UButton* QuitGameButton;

	// 게임 시작 함수
	UFUNCTION()
	void OnStartGameClicked();

	// 게임 종료 함수
	UFUNCTION()
	void OnQuitGameClicked();

public:
	// 이동할 레벨의 이름
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Level")
	FName LevelToLoad = FName("Lvl_ThirdPerson");
};
