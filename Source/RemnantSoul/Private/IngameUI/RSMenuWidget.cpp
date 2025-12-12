// Fill out your copyright notice in the Description page of Project Settings.


#include "IngameUI/RSMenuWidget.h"
#include "Components/Button.h"
#include "Kismet/KismetSystemLibrary.h"

void URSMenuWidget::NativeConstruct()
{
	Super::NativeConstruct();

	if (QuitButton)
	{
		QuitButton->OnClicked.AddDynamic(this, &URSMenuWidget::OnQuitClicked);
	}
}

void URSMenuWidget::OnQuitClicked()
{
	// 단순 종료 (에디터에서도 돌아가도록 함)
	UWorld* World = GetWorld();
	APlayerController* PC = World ? World->GetFirstPlayerController() : nullptr;

	UKismetSystemLibrary::QuitGame(
		World,
		PC,
		EQuitPreference::Quit,
		true
	);
}