#include "RSArmorBase.h"

#include "System/LyraAssetManager.h"
#include "GameFramework/Character.h"

#include UE_INLINE_GENERATED_CPP_BY_NAME(RSArmorBase)

ARSArmorBase::ARSArmorBase(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
{
	PrimaryActorTick.bCanEverTick = false;
	PrimaryActorTick.bStartWithTickEnabled = false;

    ArmorMeshComponent = CreateDefaultSubobject<USkeletalMeshComponent>("ArmorMeshComponent");
	ArmorMeshComponent->SetCollisionEnabled(ECollisionEnabled::NoCollision);
	SetRootComponent(ArmorMeshComponent);

	// 추가 파츠용 (목 등)
	ExtraMeshComponent = CreateDefaultSubobject<USkeletalMeshComponent>(TEXT("ExtraMeshComponent"));
	ExtraMeshComponent->SetupAttachment(ArmorMeshComponent);
	ExtraMeshComponent->SetCollisionEnabled(ECollisionEnabled::NoCollision);
	ExtraMeshComponent->SetVisibility(false); // 기본은 꺼둔다
}

void ARSArmorBase::UpdateExtraMeshForHeadSlot()
{
	if (!ExtraMeshComponent)
	{
		return;
	}

	// Head 슬롯 여부는 SkinMaterialSlotName이 NAME_None인지로 판별
	const bool bIsHeadSlot = SkinMaterialSlotName.IsNone();

	if (bIsHeadSlot && bSecondaryAdditive && SecondaryArmorMesh.IsNull() == false)
	{
		USkeletalMesh* ExtraMesh = ULyraAssetManager::GetAssetByPath<USkeletalMesh>(SecondaryArmorMesh);
		if (ExtraMesh)
		{
			ExtraMeshComponent->SetSkeletalMesh(ExtraMesh);

			if (ACharacter* OwnerCharacter = Cast<ACharacter>(GetOwner()))
			{
				if (USkeletalMeshComponent* OwnerMesh = OwnerCharacter->GetMesh())
				{
					ExtraMeshComponent->SetLeaderPoseComponent(OwnerMesh);
				}
			}

			ExtraMeshComponent->SetVisibility(true);
			return;
		}
	}

	// 여기까지 왔다는 건: 헤드 슬롯이 아니거나 / additive가 꺼져있거나 / SecondaryMesh 없음
	ExtraMeshComponent->SetVisibility(false);
	ExtraMeshComponent->SetSkeletalMesh(nullptr);
}

void ARSArmorBase::InitializeActor(TSoftObjectPtr<USkeletalMesh> InDefaultArmorMesh, TSoftObjectPtr<USkeletalMesh> InSecondaryArmorMesh, FName InSkinMaterialSlotName, TSoftObjectPtr<UMaterialInterface> InSkinMaterial)
{
	DefaultArmorMesh = InDefaultArmorMesh;
	SecondaryArmorMesh = (InSecondaryArmorMesh.IsNull() == false ? InSecondaryArmorMesh : InDefaultArmorMesh);
	
	if (InSkinMaterialSlotName.IsNone() == false && InSkinMaterial.IsNull() == false)
	{
		SkinMaterialSlotName = InSkinMaterialSlotName;
		SkinMaterial = InSkinMaterial;
	}

	// 메인 ArmorMeshComponent에는 항상 기본메쉬를 세팅
	SetArmorMesh(DefaultArmorMesh);

	// ExtraMesh(목) 세팅은 공통 함수로 처리
	UpdateExtraMeshForHeadSlot();
}

void ARSArmorBase::SetPrimaryArmorMesh(USkeletalMesh* InPrimaryArmorMesh)
{
	PrimaryArmorMesh = InPrimaryArmorMesh;
	RefreshArmorMesh();
}

void ARSArmorBase::ShouldUseSecondaryMesh(bool bInShouldUseSecondary)
{
	bShouldUseSecondaryArmor = bInShouldUseSecondary;
	RefreshArmorMesh();
}

// 목 메쉬 따로 추가 모드 설정
void ARSArmorBase::SetSecondaryAdditive(bool bInSecondaryAdditive)
{
	bSecondaryAdditive = bInSecondaryAdditive;

	// 플래그 바꿨으면 ExtraMesh 다시 세팅
	UpdateExtraMeshForHeadSlot();
}


void ARSArmorBase::RefreshArmorMesh()
{
	TSoftObjectPtr<USkeletalMesh> ArmorMesh;
	if (bShouldUseSecondaryArmor)
	{
		ArmorMesh = SecondaryArmorMesh;
	}
	else
	{
		ArmorMesh = PrimaryArmorMesh ? PrimaryArmorMesh : DefaultArmorMesh;
	}

	SetArmorMesh(ArmorMesh);
}

void ARSArmorBase::SetArmorMesh(TSoftObjectPtr<USkeletalMesh> InArmorMesh)
{
	USkeletalMesh* LoadedArmorMesh = nullptr;
	if (InArmorMesh.IsNull() == false)
	{
		LoadedArmorMesh = ULyraAssetManager::GetAssetByPath<USkeletalMesh>(InArmorMesh);
	}
	
	ArmorMeshComponent->SetSkeletalMesh(LoadedArmorMesh);
	ArmorMeshComponent->EmptyOverrideMaterials();

	if (SkinMaterialSlotName.IsNone() == false && SkinMaterial.IsNull() == false)
	{
		UMaterialInterface* LoadedMaterial = ULyraAssetManager::GetAssetByPath<UMaterialInterface>(SkinMaterial);
		ArmorMeshComponent->SetMaterialByName(SkinMaterialSlotName, LoadedMaterial);
	}
}

void ARSArmorBase::AttachToCharacterAndBindPose(ACharacter* InCharacter)
{
	if (!InCharacter)
	{
		return;
	}

	USkeletalMeshComponent* OwnerMesh = InCharacter->GetMesh();
	if (!OwnerMesh)
	{
		return;
	}

	// 1) 이 갑옷 액터를 캐릭터 메쉬에 붙인다.
	const FAttachmentTransformRules AttachRules(EAttachmentRule::SnapToTarget, true);
	AttachToComponent(OwnerMesh, AttachRules);

	// 2) 메인 갑옷 메쉬가 캐릭터 포즈를 따라가도록 LeaderPose 설정
	ArmorMeshComponent->SetLeaderPoseComponent(OwnerMesh);

	// 3) ExtraMesh도 동일하게 캐릭터 포즈를 따라가도록 설정
	ExtraMeshComponent->SetLeaderPoseComponent(OwnerMesh);
}
