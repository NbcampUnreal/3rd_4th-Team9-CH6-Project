#pragma once

#include "RSArmorBase.generated.h"

UCLASS(BlueprintType, Blueprintable)
class ARSArmorBase : public AActor
{
	GENERATED_BODY()
	
public:
	ARSArmorBase(const FObjectInitializer& ObjectInitializer = FObjectInitializer::Get());

public:
	void InitializeActor(TSoftObjectPtr<USkeletalMesh> InDefaultArmorMesh, TSoftObjectPtr<USkeletalMesh> InSecondaryArmorMesh, FName InSkinMaterialSlotName, TSoftObjectPtr<UMaterialInterface> InSkinMaterial);

public:
	void SetPrimaryArmorMesh(USkeletalMesh* InPrimaryArmorMesh);
	void ShouldUseSecondaryMesh(bool bInShouldUseSecondary);

	// 헤드 슬롯에서 "목 메쉬 따로 추가" 모드인지 여부
	void SetSecondaryAdditive(bool bInSecondaryAdditive);

	// 애니메이션이 파츠별로 안움직여서 확인차 만든 함수
	UFUNCTION(BlueprintCallable, Category = "Armor")
	void AttachToCharacterAndBindPose(ACharacter* InCharacter);

private:
	void RefreshArmorMesh();
	void SetArmorMesh(TSoftObjectPtr<USkeletalMesh> InArmorMesh);



	// 목 메쉬, 혹은 Extra 추가
	void UpdateExtraMeshForHeadSlot();
	
public:
	USkeletalMeshComponent* GetMeshComponent() const { return ArmorMeshComponent; }
	
protected:
	UPROPERTY(VisibleAnywhere)
	TObjectPtr<USkeletalMeshComponent> ArmorMeshComponent;

	// 목/추가 파츠 렌더용 (헤드 슬롯에서만 사용)
	UPROPERTY(VisibleAnywhere)
	TObjectPtr<USkeletalMeshComponent> ExtraMeshComponent;

protected:
	UPROPERTY()
	TSoftObjectPtr<USkeletalMesh> DefaultArmorMesh;
	
	UPROPERTY()
	TSoftObjectPtr<USkeletalMesh> PrimaryArmorMesh;

	UPROPERTY()
	TSoftObjectPtr<USkeletalMesh> SecondaryArmorMesh;

protected:
	UPROPERTY()
	FName SkinMaterialSlotName;

	UPROPERTY()
	TSoftObjectPtr<UMaterialInterface> SkinMaterial;
	
private:
	bool bShouldUseSecondaryArmor = false;

	// true이면: SecondaryArmorMesh를 대체가 아니라 "추가 Mesh"로 사용 (헤드 슬롯)
	bool bSecondaryAdditive = false;
};
