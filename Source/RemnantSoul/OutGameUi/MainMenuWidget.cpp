// Fill out your copyright notice in the Description page of Project Settings.


#include "OutGameUi/MainMenuWidget.h"
#include "Components/Button.h"
#include "Components/WidgetSwitcher.h"
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

	//환경설정 버튼 연결
	if (SettingsButton)
	{
		SettingsButton->OnClicked.AddDynamic(this, &UMainMenuWidget::OnSettingsClicked);
	}

	// 뒤로가는 버튼 연결
	if (BackButton)
	{
		BackButton->OnClicked.AddDynamic(this, &UMainMenuWidget::OnBackClicked);
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

void UMainMenuWidget::OnSettingsClicked()
{
	if (MenuSwitcher)
	{
		// SetActiveWidgetIndex(1): 스위처의 두 번째(Index 1)을 보여줘라!
		MenuSwitcher->SetActiveWidgetIndex(1);
	}
}

void UMainMenuWidget::OnBackClicked()
{
	if (MenuSwitcher)
	{
		// SetActiveWidgetIndex(0): 스위처의 첫 번째(Index 0)을 보여줘라!
		MenuSwitcher->SetActiveWidgetIndex(0);
	}
}