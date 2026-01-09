// Fill out your copyright notice in the Description page of Project Settings.

// RSRuneNode.h

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "Interface/Interactable.h"
#include "Interface/RSItemSource.h"
#include "RSRuneNode.generated.h"

class UBoxComponent;
class UStaticMeshComponent;
class URSPuzzleNodeComponent;

UCLASS()
class REMNANTSOUL_API ARSRuneNode : public AActor, public IInteractable
{
	GENERATED_BODY()

public:
	ARSRuneNode();

	// IInteractable
	virtual bool CanInteract_Implementation(AActor* Interactor) const override;
	virtual void Interact_Implementation(AActor* Interactor) override;
	virtual URSItemData* GetItemData_Implementation() const;

protected:
	virtual void BeginPlay() override;

private:
	UPROPERTY(VisibleAnywhere, Category="Components")
	TObjectPtr<USceneComponent> Root = nullptr;

	UPROPERTY(VisibleAnywhere, Category="Components")
	TObjectPtr<UStaticMeshComponent> Visual = nullptr;

	UPROPERTY(VisibleAnywhere, Category="Components")
	TObjectPtr<UBoxComponent> Trigger = nullptr;

	UPROPERTY(VisibleAnywhere, Category="Components")
	TObjectPtr<URSPuzzleNodeComponent> PuzzleNode = nullptr;

private:
	// 액터 이름에 Top/Mid/Bot 들어가면 자동으로 그 노드로 동작.
	// 예) BP_RuneNode_Top_01, RuneNode_Mid, RuneNode_Bot 등
	void ApplyHardcodedTagsFromName();

	// 퍼즐 정의(정답 포함) 하드코딩으로 생성해서 StartPuzzle에 넣는다.
	const class URSPuzzleDefinition* GetOrCreateHardcodedDefinition() const;

	void UpdateVisualByLocalState() const;

private:
	// 하드코딩 파라미터
	static constexpr int32 MaxState_Hardcoded = 6;      // 룬 종류(0~5)
	static constexpr int32 Target_Top = 2;
	static constexpr int32 Target_Mid = 4;
	static constexpr int32 Target_Bot = 1;

	// 캐시(월드마다 하나 만들고 재사용)
	mutable TObjectPtr<URSPuzzleDefinition> CachedDefinition = nullptr;
};
