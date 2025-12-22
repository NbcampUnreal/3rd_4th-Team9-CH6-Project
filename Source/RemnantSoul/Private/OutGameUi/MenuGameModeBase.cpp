// Fill out your copyright notice in the Description page of Project Settings.


#include "OutGameUi/MenuGameModeBase.h"
#include "Blueprint/UserWidget.h"
#include "Kismet/GameplayStatics.h"
#include "Sound/SoundBase.h"

AMenuGameModeBase::AMenuGameModeBase()
{
	DefaultPawnClass = nullptr;
}

void AMenuGameModeBase::BeginPlay()
{
	Super::BeginPlay();

	// 1. UI 위젯 생성 및 화면 부착
	if (MainMenuWidgetClass)
	{
		UUserWidget* MenuWidget = CreateWidget<UUserWidget>(GetWorld(), MainMenuWidgetClass);
		if (MenuWidget)
		{
			MenuWidget->AddToViewport();
		}
	}

	// 2. 마우스 커서 보이게 설정
	if (APlayerController* PC = UGameplayStatics::GetPlayerController(this, 0))
	{
		PC->bShowMouseCursor = true;

		// UI 입력 모드로 설정 (게임 조작 막기)
		FInputModeUIOnly InputMode;
		InputMode.SetLockMouseToViewportBehavior(EMouseLockMode::DoNotLock);
		PC->SetInputMode(InputMode);
	}

	if (MenuBGM)
	{
		UGameplayStatics::PlaySound2D(this, MenuBGM);
	}
}
