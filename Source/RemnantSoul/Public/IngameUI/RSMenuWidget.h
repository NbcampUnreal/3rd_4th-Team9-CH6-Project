// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "RSMenuWidget.generated.h"

class UButton;

UCLASS()
class REMNANTSOUL_API URSMenuWidget : public UUserWidget
{
	GENERATED_BODY()
	
protected:
	virtual void NativeConstruct() override;

	UFUNCTION()
	void OnQuitClicked();

public:
	UPROPERTY(meta = (BindWidget))
	UButton* QuitButton;

};
