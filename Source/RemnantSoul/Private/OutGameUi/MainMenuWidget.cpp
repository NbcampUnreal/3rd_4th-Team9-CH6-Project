// Fill out your copyright notice in the Description page of Project Settings.


#include "OutGameUi/MainMenuWidget.h"
#include "Components/Button.h"
#include "Components/WidgetSwitcher.h"
#include "Kismet/GameplayStatics.h"
#include "Kismet/KismetSystemLibrary.h"
#include "GameFramework/GameUserSettings.h"
#include "DLSSLibrary.h"

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
		APlayerController* PC = GetOwningPlayer(); //플레이어 컨트롤러를 가져옴

		if (PC)
		{
			//메뉴 레벨 떠나기 전 게임 모드로 돌아가라고 명령
			FInputModeGameOnly GameInputMode;
			PC->SetInputMode(GameInputMode);

			PC->bShowMouseCursor = false; // 마우스 커서 숨김
		}

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

void UMainMenuWidget::SetGraphicsQuality(int32 QualityLevel)
{
	UGameUserSettings* UserSettings = GEngine->GetGameUserSettings();
	if (UserSettings)
	{
		UserSettings->SetOverallScalabilityLevel(QualityLevel);
		UserSettings->ApplySettings(false);
		UserSettings->SaveSettings();
	}
}

int32 UMainMenuWidget::GetCurrentQualityLevel() const
{
	UGameUserSettings* UserSettings = GEngine->GetGameUserSettings();
	if (UserSettings)
	{
		return UserSettings->GetOverallScalabilityLevel();
	}
	return 0;
}

void UMainMenuWidget::SetDisplayMode(int32 ModeIndex)
{
	UGameUserSettings* UserSettings = GEngine->GetGameUserSettings();
	if (UserSettings)
	{
		//0은 창모드 1은 전체화면
		EWindowMode::Type NewMode = (ModeIndex == 0) ? EWindowMode::Windowed : EWindowMode::Fullscreen;

		UserSettings->SetFullscreenMode(NewMode);

		//해상도도 같이 적용
		FIntPoint CurrentRes = UserSettings->GetScreenResolution();
		UserSettings->SetScreenResolution(CurrentRes);

		UserSettings->ApplySettings(false);
		UserSettings->SaveSettings();

	}
}

int32 UMainMenuWidget::GetCurrentDisplayMode() const
{
	UGameUserSettings* UserSettings = GEngine->GetGameUserSettings();
	if (UserSettings)
	{
		EWindowMode::Type CurrentMode = UserSettings->GetFullscreenMode();

		if (CurrentMode == EWindowMode::Fullscreen || CurrentMode == EWindowMode::WindowedFullscreen)
		{
			return 1;
		}
	}
	return 0;
}

void UMainMenuWidget::SetDLSSMode(int32 ModeIndex)
{
	UDLSSMode DLSSMode = UDLSSMode::Off;

	switch (ModeIndex)
	{
	case 0: DLSSMode = UDLSSMode::Off; break;
	case 1: DLSSMode = UDLSSMode::Performance; break;
	case 2: DLSSMode = UDLSSMode::Balanced; break;
	case 3: DLSSMode = UDLSSMode::Quality; break;
	case 4: DLSSMode = UDLSSMode::UltraPerformance; break;
	default: DLSSMode = UDLSSMode::Off; break;
	}

	UDLSSLibrary::SetDLSSMode(GetWorld(), DLSSMode);
}

int32 UMainMenuWidget::GetCurrentDLSSMode() const
{
	UDLSSMode CurrentMode = UDLSSLibrary::GetDLSSMode();


	switch (CurrentMode)
	{
	case UDLSSMode::Off:            return 0;
	case UDLSSMode::Performance:    return 1;
	case UDLSSMode::Balanced:       return 2;
	case UDLSSMode::Quality:        return 3;
	case UDLSSMode::UltraPerformance: return 4;
	default:                        return 0;
	}
}

void UMainMenuWidget::SetFrameGeneration(bool bEnable)
{
	if (bEnable)
	{
		GetOwningPlayer()->ConsoleCommand("r.Streamline.DLSSG.Enable 1");
	}
	else
	{
		GetOwningPlayer()->ConsoleCommand("r.Streamline.DLSSG.Enable 0");
	}
}

bool UMainMenuWidget::GetFrameGenerationState() const
{
	IConsoleVariable* CVar = IConsoleManager::Get().FindConsoleVariable(TEXT("r.Streamline.DLSSG.Enable"));
	if (CVar)
	{
		return CVar->GetInt() > 0;
	}
	return false;
}

bool UMainMenuWidget::IsDLSSSupported() const
{
	return UDLSSLibrary::IsDLSSSupported();
}

bool UMainMenuWidget::IsFrameGenSupported() const
{
	static const auto CVar = IConsoleManager::Get().FindConsoleVariable(TEXT("r.Streamline.DLSSG.Enable"));
	return (CVar != nullptr);
}

void UMainMenuWidget::SetRayTracingQuality(int32 QualityLevel)
{
	APlayerController* PC = GetOwningPlayer();
	if (!PC) return;

	//모든 설정 초기화
	PC->ConsoleCommand("r.Lumen.HardwareRayTracing 0");
	PC->ConsoleCommand("r.RayTracing.ForceAllRayTracingEffects 0");
	PC->ConsoleCommand("r.PathTracing 0");
	
	switch (QualityLevel)
	{
	case 0: //기본, 소프트웨어 루맨 상태 초기화 상태
		break;

	case 1: //중간, 하드웨어 루맨 상태
		PC->ConsoleCommand("r.Lumen.HardwareRayTracing 1");
		break;

	case 2: // 높음, 풀 레이 트레이싱
		PC->ConsoleCommand("r.Lumen.HardwareRayTracing 1");
		PC->ConsoleCommand("r.RayTracing.ForceAllRayTracingEffects 1");
		break;

	case 3: // 패스 트레이싱
		PC->ConsoleCommand("r.PathTracing 1");
		break; 

	}

}

int32 UMainMenuWidget::GetRayTracingQuality() const
{
	//현재 상태 역추적 후 단계 반환
	IConsoleManager& IConsole = IConsoleManager::Get();


	// 패스 트레이싱 켠 상태인가? 리턴 3
	static const auto CVarPath = IConsole.FindConsoleVariable(TEXT("r.PathTracing"));
	if (CVarPath && CVarPath->GetInt() > 0) return 3;

	//풀 RT 켠 상태인가? 리턴 2
	static const auto CVarFull = IConsole.FindConsoleVariable(TEXT("r.RayTracing.ForceAllRayTracingEffects"));
	if (CVarFull && CVarFull->GetInt() > 0) return 2;
	//하드웨어 루멘 켠 상태인가? 1
	static const auto CVarHW = IConsole.FindConsoleVariable(TEXT("r.Lumen.HardwareRayTracing"));
	if (CVarHW && CVarHW->GetInt() > 0) return 1;

	//전부 아니면 리턴 0 (기본 소프트웨어 상태)
	return 0;
}