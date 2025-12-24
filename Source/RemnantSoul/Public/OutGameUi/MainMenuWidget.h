// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "MainMenuWidget.generated.h"

/**
 * 
 */
UCLASS()
class REMNANTSOUL_API UMainMenuWidget : public UUserWidget
{
	GENERATED_BODY()
	

protected:

	virtual void NativeConstruct() override;
	// 게임 시작 함수
	UFUNCTION(BlueprintCallable)
	void OnStartGameClicked();

	// 게임 종료 함수
	UFUNCTION(BlueprintCallable)
	void OnQuitGameClicked();


public:
	// 이동할 레벨의 이름
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Level")
	FName LevelToLoad = FName("Cave");


	//그래픽 설정 관련 함수

	UFUNCTION(BlueprintCallable, Category = "Settings")
	void SetGraphicsQuality(int32 QualityLevel);  // 그래픽 품질 변경 0~3까지

	UFUNCTION(BlueprintPure, Category = "Settings")
	int32 GetCurrentQualityLevel() const; // 현재 그래픽 상태


	//화면 모드 관련 함수

	UFUNCTION(BlueprintCallable, Category = "Settings")
	void SetDisplayMode(int32 ModeIndex);  //화면 모드 변경
		
	UFUNCTION(BlueprintPure, Category = "Settings")
	int32 GetCurrentDisplayMode() const; // 현재 창 모드


	// dlss 관련 함수

	UFUNCTION(BlueprintCallable, Category = "Settings|DLSS")
	void SetDLSSMode(int32 ModeIndex); //dlss 모드 설정

	UFUNCTION(BlueprintCallable, Category = "Settings|DLSS")
	int32 GetCurrentDLSSMode() const; //현재 dlss 모드 가져옴

	UFUNCTION(BlueprintCallable, Category = "Settings|DLSS")
	void SetFrameGeneration(bool bEnable); // 프레임 생성 온오프

	UFUNCTION(BlueprintCallable, Category = "Settings|DLSS")
	bool GetFrameGenerationState() const; // 프레임 생성 상태 가져오기

	UFUNCTION(BlueprintCallable, Category = "Settings|DLSS")
	bool IsDLSSSupported() const;  // dlss 지원 하드웨어 확인

	UFUNCTION(BlueprintCallable, Category = "Settings|DLSS")
	bool IsFrameGenSupported() const; // 프레임 생성 지원 하드웨어 확인


	// 레이 트레이싱 함수

	// 레이 트레이싱 품질 변경 (0: 끄기/SW, 1: 중간/HW, 2: 높음/Full RT, 3: PathTracing)
	UFUNCTION(BlueprintCallable, Category = "Settigns|RayTracing")
	void SetRayTracingQuality(int32 QualityLevel);

	UFUNCTION(BlueprintPure, Category = "Settigns|RayTracing")
	int32 GetRayTracingQuality() const;
};
