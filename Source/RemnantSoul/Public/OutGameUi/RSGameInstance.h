// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Engine/GameInstance.h"
#include "RSGameInstance.generated.h"

/**
 * 
 */
UCLASS()
class REMNANTSOUL_API URSGameInstance : public UGameInstance
{
	GENERATED_BODY()

public:
	virtual void Init() override;

protected:

	UFUNCTION()
	void BeginLoadingScreen(const FString& MapName); //레벨 이동 시작시 호출

	UFUNCTION()
	void EndLoadingScreen(UWorld* InLoadedWorld);

public:
	// 에디터에서 로딩 화면으로 쓸 위젯 블루프린트를 넣어줄 변수
	UPROPERTY(EditDefaultsOnly, Category = "Loading")
	TSubclassOf<class UUserWidget> LoadingScreenWidgetClass;

};
