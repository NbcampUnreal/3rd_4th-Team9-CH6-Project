#pragma once

#include "RSItemFragment_Equipable.h"
#include "RSItemFragment_Equipable_Attachment.generated.h"

class ARSEquipmentBase;

USTRUCT(BlueprintType)
struct FRSWeaponAttachInfo
{
	GENERATED_BODY()

public:
	UPROPERTY(EditDefaultsOnly)
	TSubclassOf<ARSEquipmentBase> SpawnWeaponClass;

	UPROPERTY(EditDefaultsOnly)
	FName AttachSocket;

	UPROPERTY(EditDefaultsOnly)
	FTransform AttachTransform;
};

UCLASS(Abstract, Const)
class URSItemFragment_Equipable_Attachment : public URSItemFragment_Equipable
{
	GENERATED_BODY()
	
public:
	URSItemFragment_Equipable_Attachment(const FObjectInitializer& ObjectInitializer = FObjectInitializer::Get());

public:
	UPROPERTY(EditDefaultsOnly)
	EWeaponHandType WeaponHandType = EWeaponHandType::Count;

	UPROPERTY(EditDefaultsOnly)
	FRSWeaponAttachInfo WeaponAttachInfo;

	UPROPERTY(EditDefaultsOnly)
	TSoftObjectPtr<UAnimMontage> EquipMontage;

	UPROPERTY(EditDefaultsOnly)
	TSoftObjectPtr<UAnimMontage> FrontHitMontage;
	
	UPROPERTY(EditDefaultsOnly)
	TSoftObjectPtr<UAnimMontage> BackHitMontage;

	UPROPERTY(EditDefaultsOnly)
	TSoftObjectPtr<UAnimMontage> LeftHitMontage;

	UPROPERTY(EditDefaultsOnly)
	TSoftObjectPtr<UAnimMontage> RightHitMontage;

	UPROPERTY(EditDefaultsOnly)
	TSoftObjectPtr<UAnimMontage> BlockHitMontage;

	UPROPERTY(EditDefaultsOnly)
	TSoftObjectPtr<UAnimMontage> PocketWorldIdleMontage;
	
	UPROPERTY(EditDefaultsOnly)
	TSubclassOf<UAnimInstance> AnimInstanceClass;
};
