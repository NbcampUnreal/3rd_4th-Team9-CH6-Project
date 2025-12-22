// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/GameModeBase.h"
#include "MenuGameModeBase.generated.h"

/**
 * 
 */
UCLASS()
class REMNANTSOUL_API AMenuGameModeBase : public AGameModeBase
{
	GENERATED_BODY()
	

public:
	AMenuGameModeBase();

	virtual void BeginPlay() override;

protected:
	// 화면에 띄울 위젯 클래스
	UPROPERTY(EditDefaultsOnly, Category = "UI")
	TSubclassOf<class UUserWidget> MainMenuWidgetClass;

	//bgm넣을 변수
	UPROPERTY(EditDefaultsOnly, Category = "Audio")
	class USoundBase* MenuBGM;
};
