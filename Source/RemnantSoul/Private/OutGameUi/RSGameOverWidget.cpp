// Fill out your copyright notice in the Description page of Project Settings.


#include "OutGameUi/RSGameOverWidget.h"
#include "Components/Button.h"
#include "Kismet/GameplayStatics.h"
#include "Kismet/KismetSystemLibrary.h"

void URSGameOverWidget::NativeConstruct()
{
	Super::NativeConstruct();

	if (RestartButton)
	{
		RestartButton->OnClicked.AddDynamic(this, &URSGameOverWidget::OnRestartClicked);
	}
	if (QuitButton)
	{
		QuitButton->OnClicked.AddDynamic(this, &URSGameOverWidget::OnQuitClicked);
	}
}

void URSGameOverWidget::OnRestartClicked()
{
	APlayerController* PC = GetOwningPlayer();
	if (PC)
	{
		// ★ 핵심: 입력 모드를 '게임 전용'으로 되돌림
		PC->SetInputMode(FInputModeGameOnly());

		// 마우스 커서 숨기기
		PC->bShowMouseCursor = false;
	}

	// 현재 레벨의 이름을 가져와서 다시 로드 (재시작)
	FString CurrentLevelName = UGameplayStatics::GetCurrentLevelName(this);
	UGameplayStatics::OpenLevel(this, FName(*CurrentLevelName));
}

void URSGameOverWidget::OnQuitClicked()
{
	// 게임 종료
	UKismetSystemLibrary::QuitGame(this, GetOwningPlayer(), EQuitPreference::Quit, true);
}