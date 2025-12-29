// Fill out your copyright notice in the Description page of Project Settings.


#include "OutGameUi/RSGameInstance.h"
#include "MoviePlayer.h"
#include "Blueprint/UserWidget.h"

void URSGameInstance::Init()
{
	Super::Init();

	// 레벨 이동 시작/끝 이벤트에 내 함수를 등록
	FCoreUObjectDelegates::PreLoadMap.AddUObject(this, &URSGameInstance::BeginLoadingScreen);
	FCoreUObjectDelegates::PostLoadMapWithWorld.AddUObject(this, &URSGameInstance::EndLoadingScreen);
}

void URSGameInstance::BeginLoadingScreen(const FString& MapName)
{
	// 로딩 화면 설정 구조체
	FLoadingScreenAttributes LoadingScreen;

	// 로딩이 끝나면 자동으로 사라짐
	LoadingScreen.bAutoCompleteWhenLoadingCompletes = true;

	//위젯 블루프린트를 로딩 화면으로 사용
	if (LoadingScreenWidgetClass)
	{
		// 위젯 생성
		UUserWidget* Widget = CreateWidget<UUserWidget>(this, LoadingScreenWidgetClass);

		if (Widget)
		{
			// 위젯의 포인터를 가져와서 로딩창에 줌
			LoadingScreen.WidgetLoadingScreen = Widget->TakeWidget();
		}
	}
	else
	{
		// 위젯이 없으면 테스트용 기본 로딩창
		LoadingScreen.WidgetLoadingScreen = FLoadingScreenAttributes::NewTestLoadingScreenWidget();
	}

	// 3. 로딩 화면 시스템에 등록 및 시작
	GetMoviePlayer()->SetupLoadingScreen(LoadingScreen);
}

void URSGameInstance::EndLoadingScreen(UWorld* InLoadedWorld)
{
	// 로딩이 끝나면 할 일 
}