// Fill out your copyright notice in the Description page of Project Settings.


#include "OutGameUi/MainMenuWidget.h"
#include "Components/Button.h"
#include "Kismet/GameplayStatics.h"
#include "Kismet/KismetSystemLibrary.h"

void UMainMenuWidget::NativeConstruct()
{
	Super::NativeConstruct();

	// 시작 버튼 연결
	if (StartGameButton)
	{
		StartGameButton->OnClicked.AddDynamic(this, &UMainMenuWidget::OnStartGameClicked);
	}

	// 종료 버튼 연결
	if (QuitGameButton)
	{
		QuitGameButton->OnClicked.AddDynamic(this, &UMainMenuWidget::OnQuitGameClicked);
	}
}

void UMainMenuWidget::OnStartGameClicked()
{
	// 레벨 이동
	if (LevelToLoad != NAME_None)
	{
		UGameplayStatics::OpenLevel(this, LevelToLoad);
	}
}

void UMainMenuWidget::OnQuitGameClicked()
{
	// 게임 종료
	UKismetSystemLibrary::QuitGame(this, GetOwningPlayer(), EQuitPreference::Quit, true);
}