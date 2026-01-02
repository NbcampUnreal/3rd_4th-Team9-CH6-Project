// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "Components/Button.h"
#include "RSGameOverWidget.generated.h"

/**
 * 
 */
UCLASS()
class REMNANTSOUL_API URSGameOverWidget : public UUserWidget
{
	GENERATED_BODY()
	
protected:
	virtual void NativeConstruct() override;

	UFUNCTION()
	void OnRestartClicked();

	UFUNCTION()
	void OnQuitClicked();

public:
	// 위젯 블루프린트에서 바인딩할 버튼 변수

	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UButton> RestartButton;

	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UButton> QuitButton;
};
