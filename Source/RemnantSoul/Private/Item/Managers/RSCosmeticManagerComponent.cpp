#include "Item/Managers/RSCosmeticManagerComponent.h"

#include "Character/RSCharacter.h"
#include "Item/RSItemInstance.h"
#include "Item/RSWeaponActor.h"
#include "Item/RSItemTemplate.h"
#include "Item/RSItemTemplate_Weapon.h"
#include "Item/Fragments/RSItemFragment_WeaponCosmetic.h"
#include "GameFramework/Character.h"
#include "Components/SkeletalMeshComponent.h"
#include "Engine/World.h"

#include UE_INLINE_GENERATED_CPP_BY_NAME(RSCosmeticManagerComponent)

URSCosmeticManagerComponent::URSCosmeticManagerComponent()
{
	PrimaryComponentTick.bCanEverTick = false;
}

void URSCosmeticManagerComponent::BeginPlay()
{
	Super::BeginPlay();

	CacheOwnerCharacter();
}

void URSCosmeticManagerComponent::CacheOwnerCharacter()
{
	if (CachedCharacter.IsValid())
	{
		return;
	}

	AActor* Owner = GetOwner();
	if (!Owner)
	{
		return;
	}

	if (ARSCharacter* RSChar = Cast<ARSCharacter>(Owner))
	{
		CachedCharacter = RSChar;
	}
	else
	{
		// 필요하다면 ACharacter로 완화할 수도 있음
		if (ACharacter* Char = Cast<ACharacter>(Owner))
		{
			CachedCharacter = Cast<ARSCharacter>(Char); // RSCharacter만 허용한다는 가정
		}
	}
}

// YKJ Annotation : 
void URSCosmeticManagerComponent::ApplyWeaponFromItem(URSItemInstance* ItemInstance)
{
	// Owner 캐시 시도
	if (!CachedCharacter.IsValid())
	{
		CacheOwnerCharacter();
	}

	// 아이템이 없다면 현재 무기 제거
	if (!ItemInstance)
	{
		ClearWeapon();
		return;
	}

	const URSItemTemplate* ItemTemplate = ItemInstance->GetTemplate();
	if (!ItemTemplate)
	{
		ClearWeapon();
		return;
	}

	// 무기 전용 템플릿이 있다면 캐스팅 (필수는 아님 — 그냥 WeaponCosmetic Fragment만 있어도 동작 가능)
	const URSItemTemplate_Weapon* WeaponTemplate = Cast<URSItemTemplate_Weapon>(ItemTemplate);

	// WeaponCosmetic Fragment 가져오기 (템플릿 타입과 무관하게 이 Fragment만 있으면 됨)
	const URSItemFragment_WeaponCosmetic* CosFrag = ItemTemplate->FindFragment<URSItemFragment_WeaponCosmetic>();
	if (!CosFrag)
	{
		// 무기 코스메틱 정보가 없다면 외형 제거
		ClearWeapon();
		return;
	}

	// 이전 무기 제거
	DestroyCurrentWeaponActor();

	// 새 무기 스폰 & Attach
	SpawnAndAttachWeaponActor(ItemTemplate, CosFrag);
}

void URSCosmeticManagerComponent::ClearWeapon()
{
	DestroyCurrentWeaponActor();
}

void URSCosmeticManagerComponent::DestroyCurrentWeaponActor()
{
	if (!CurrentWeaponActor)
	{
		return;
	}

	if (bDestroyWeaponOnUnequip)
	{
		CurrentWeaponActor->Destroy();
	}
	else
	{
		CurrentWeaponActor->SetActorHiddenInGame(true);
		CurrentWeaponActor->SetActorEnableCollision(false);
		CurrentWeaponActor->SetActorTickEnabled(false);
	}

	CurrentWeaponActor = nullptr;
}

void URSCosmeticManagerComponent::SpawnAndAttachWeaponActor(
	const URSItemTemplate* ItemTemplate,
	const URSItemFragment_WeaponCosmetic* CosFrag)
{
	if (!CachedCharacter.IsValid() || !CosFrag)
	{
		return;
	}

	ARSCharacter* RSChar = CachedCharacter.Get();
	USkeletalMeshComponent* CharacterMesh = RSChar->GetMesh();
	if (!CharacterMesh)
	{
		return;
	}

	UWorld* World = GetWorld();
	if (!World)
	{
		return;
	}

	// 스폰할 클래스 결정
	TSubclassOf<ARSWeaponActor> WeaponClass = CosFrag->WeaponActorClass;
	if (!WeaponClass)
	{
		// WeaponActorClass가 비어 있으면 굳이 스폰하지 않는다. 필요하다면 기본 무기 클래스를 지정할 수도 있움.
		// (원한다면 여기서 RSCharacter 내부 Weapon 컴포넌트를 사용하는 fallback을 넣을 수도 있음)
		return;
	}

	const FTransform SpawnTransform = CharacterMesh->GetSocketTransform(
		CosFrag->AttachSocketName.IsNone() ? DefaultWeaponAttachSocketName : CosFrag->AttachSocketName
	);

	FActorSpawnParameters SpawnParams;
	SpawnParams.Owner = RSChar;
	SpawnParams.Instigator = RSChar;
	SpawnParams.SpawnCollisionHandlingOverride = SpawnCollisionHandlingMethod;

	ARSWeaponActor* NewWeapon = World->SpawnActor<ARSWeaponActor>(
		WeaponClass,
		SpawnTransform,
		SpawnParams
	);

	if (!NewWeapon)
	{
		return;
	}

	// Attach
	const FName SocketName = CosFrag->AttachSocketName.IsNone()
		? DefaultWeaponAttachSocketName
		: CosFrag->AttachSocketName;

	FAttachmentTransformRules AttachRules(EAttachmentRule::SnapToTarget, true);
	NewWeapon->AttachToComponent(CharacterMesh, AttachRules, SocketName);

	if (CosFrag->WeaponMesh)
	{
		// YKJ Annotation : WeaponActor는 Mesh 컴포넌트를 보장한다. (정책)
		if (USkeletalMeshComponent* WeaponMeshComp = NewWeapon->GetMesh())
		{
			WeaponMeshComp->SetSkeletalMesh(CosFrag->WeaponMesh);
		}
	}

	// (선택사항임.) 무기별 AnimLayer는 AnimBP 세팅이나 Layer Interface를 통해 별도 처리 (상하체 분리도 신경써야할듯.)
	// - 여기서는 단순히 CosFrag->WeaponAnimLayer를 캐릭터 AnimInstance에 전달하는 방식으로 확장 가능한데 고민중.
	//   (예 : RSChar->OnWeaponAnimLayerChanged.Broadcast(CosFrag->WeaponAnimLayer);)

	CurrentWeaponActor = NewWeapon;
}
