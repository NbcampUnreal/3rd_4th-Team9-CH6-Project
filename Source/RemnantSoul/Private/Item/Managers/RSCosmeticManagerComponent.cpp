//#include "Item/Managers/RSCosmeticManagerComponent.h"
//
//#include "Character/RSCharacter.h"
//#include "Item/RSItemInstance.h"
//#include "Item/RSWeaponActor.h"
//#include "Item/RSItemTemplate.h"
//#include "Item/RSItemTemplate_Weapon.h"
//#include "Item/Fragments/RSItemFragment_WeaponCosmetic.h"
//#include "GameFramework/Character.h"
//#include "Components/SkeletalMeshComponent.h"
//#include "Engine/World.h"
//
//#include UE_INLINE_GENERATED_CPP_BY_NAME(RSCosmeticManagerComponent)
//
//URSCosmeticManagerComponent::URSCosmeticManagerComponent()
//{
//	PrimaryComponentTick.bCanEverTick = false;
//}
//
//void URSCosmeticManagerComponent::BeginPlay()
//{
//	Super::BeginPlay();
//
//	CacheOwnerCharacter();
//}
//
//void URSCosmeticManagerComponent::CacheOwnerCharacter()
//{
//	if (CachedCharacter.IsValid())
//	{
//		return;
//	}
//
//	AActor* Owner = GetOwner();
//	if (!Owner)
//	{
//		return;
//	}
//
//	if (ARSCharacter* RSChar = Cast<ARSCharacter>(Owner))
//	{
//		CachedCharacter = RSChar;
//	}
//	else
//	{
//		// 필요하다면 ACharacter로 완화할 수도 있음
//		if (ACharacter* Char = Cast<ACharacter>(Owner))
//		{
//			CachedCharacter = Cast<ARSCharacter>(Char); // RSCharacter만 허용한다는 가정
//		}
//	}
//}
//
//// YKJ Annotation : 
//void URSCosmeticManagerComponent::ApplyWeaponFromItem(URSItemInstance* ItemInstance)
//{
//	UE_LOG(LogTemp, Warning, TEXT("[Cos] ApplyWeaponFromItem Item=%s"), *GetNameSafe(ItemInstance));
//	const URSItemTemplate* T = ItemInstance ? ItemInstance->GetTemplate() : nullptr;
//	UE_LOG(LogTemp, Warning, TEXT("[Cos] Template=%s"), *GetNameSafe(T));
//
//
//	// Owner 캐시 시도
//	if (!CachedCharacter.IsValid())
//	{
//		CacheOwnerCharacter();
//	}
//
//	// 아이템이 없다면 현재 무기 제거
//	if (!ItemInstance)
//	{
//		ClearWeapon();
//		return;
//	}
//
//	const URSItemTemplate* ItemTemplate = ItemInstance->GetTemplate();
//	if (!ItemTemplate)
//	{
//		ClearWeapon();
//		return;
//	}
//
//	// 무기 전용 템플릿이 있다면 캐스팅 (필수는 아님 — 그냥 WeaponCosmetic Fragment만 있어도 동작 가능)
//	const URSItemTemplate_Weapon* WeaponTemplate = Cast<URSItemTemplate_Weapon>(ItemTemplate);
//
//	// WeaponCosmetic Fragment 가져오기 (템플릿 타입과 무관하게 이 Fragment만 있으면 됨)
//	const URSItemFragment_WeaponCosmetic* CosFrag = ItemTemplate->FindFragment<URSItemFragment_WeaponCosmetic>();
//	if (!CosFrag)
//	{
//		// 무기 코스메틱 정보가 없다면 외형 제거
//		ClearWeapon();
//		return;
//	}
//
//	// 이전 무기 제거
//	DestroyCurrentWeaponActor();
//
//	// 새 무기 스폰 & Attach
//	SpawnAndAttachWeaponActor(ItemTemplate, CosFrag);
//}
//
//void URSCosmeticManagerComponent::ClearWeapon()
//{
//	DestroyCurrentWeaponActor();
//}
//
//void URSCosmeticManagerComponent::DestroyCurrentWeaponActor()
//{
//	if (!CurrentWeaponActor)
//	{
//		return;
//	}
//
//	if (bDestroyWeaponOnUnequip)
//	{
//		CurrentWeaponActor->Destroy();
//	}
//	else
//	{
//		CurrentWeaponActor->SetActorHiddenInGame(true);
//		CurrentWeaponActor->SetActorEnableCollision(false);
//		CurrentWeaponActor->SetActorTickEnabled(false);
//	}
//
//	CurrentWeaponActor = nullptr;
//}
//
//void URSCosmeticManagerComponent::SpawnAndAttachWeaponActor(
//	const URSItemTemplate* ItemTemplate,
//	const URSItemFragment_WeaponCosmetic* CosFrag)
//{
//	if (!CachedCharacter.IsValid() || !CosFrag)
//	{
//		return;
//	}
//
//	ARSCharacter* RSChar = CachedCharacter.Get();
//	USkeletalMeshComponent* CharacterMesh = RSChar->GetMesh();
//	if (!CharacterMesh)
//	{
//		return;
//	}
//
//	UWorld* World = GetWorld();
//	if (!World)
//	{
//		return;
//	}
//
//	// 스폰할 클래스 결정
//	TSubclassOf<ARSWeaponActor> WeaponClass = CosFrag->WeaponActorClass;
//	if (!WeaponClass)
//	{
//		// WeaponActorClass가 비어 있으면 굳이 스폰하지 않는다. 필요하다면 기본 무기 클래스를 지정할 수도 있움.
//		// (원한다면 여기서 RSCharacter 내부 Weapon 컴포넌트를 사용하는 fallback을 넣을 수도 있음)
//		return;
//	}
//
//	const FTransform SpawnTransform = CharacterMesh->GetSocketTransform(
//		CosFrag->AttachSocketName.IsNone() ? DefaultWeaponAttachSocketName : CosFrag->AttachSocketName
//	);
//
//	FActorSpawnParameters SpawnParams;
//	SpawnParams.Owner = RSChar;
//	SpawnParams.Instigator = RSChar;
//	SpawnParams.SpawnCollisionHandlingOverride = SpawnCollisionHandlingMethod;
//
//	ARSWeaponActor* NewWeapon = World->SpawnActor<ARSWeaponActor>(
//		WeaponClass,
//		SpawnTransform,
//		SpawnParams
//	);
//
//	if (!NewWeapon)
//	{
//		return;
//	}
//
//	// Attach
//	const FName SocketName = CosFrag->AttachSocketName.IsNone()
//		? DefaultWeaponAttachSocketName
//		: CosFrag->AttachSocketName;
//
//	FAttachmentTransformRules AttachRules(EAttachmentRule::SnapToTarget, true);
//	NewWeapon->AttachToComponent(CharacterMesh, AttachRules, SocketName);
//
//	if (CosFrag->WeaponMesh)
//	{
//		// YKJ Annotation : WeaponActor는 Mesh 컴포넌트를 보장한다. (정책)
//		if (USkeletalMeshComponent* WeaponMeshComp = NewWeapon->GetMesh())
//		{
//			WeaponMeshComp->SetSkeletalMesh(CosFrag->WeaponMesh);
//		}
//	}
//
//	// (선택사항임.) 무기별 AnimLayer는 AnimBP 세팅이나 Layer Interface를 통해 별도 처리 (상하체 분리도 신경써야할듯.)
//	// - 여기서는 단순히 CosFrag->WeaponAnimLayer를 캐릭터 AnimInstance에 전달하는 방식으로 확장 가능한데 고민중.
//	//   (예 : RSChar->OnWeaponAnimLayerChanged.Broadcast(CosFrag->WeaponAnimLayer);)
//
//	CurrentWeaponActor = NewWeapon;
//
//	UE_LOG(LogTemp, Warning,
//		TEXT("[Cosmetic] Weapon attached. Actor=%s Socket=%s Mesh=%s"),
//		*GetNameSafe(NewWeapon),
//		*SocketName.ToString(),
//		*GetNameSafe(CosFrag->WeaponMesh));
//}
//
//// 여기서 부터 아래까지 다 Holster내용
//void URSCosmeticManagerComponent::SyncWeaponCosmetics(URSItemInstance* MainItem, URSItemInstance* SubItem)
//{
//	if (!CachedCharacter.IsValid())
//	{
//		CacheOwnerCharacter();
//	}
//
//	// Main(손)
//	ApplyWeaponToSlot(/*bIsMain=*/true, MainItem);
//
//	// Sub(홀스터)
//	ApplyWeaponToSlot(/*bIsMain=*/false, SubItem);
//}
//
//void URSCosmeticManagerComponent::ClearAllWeapons()
//{
//	DestroyWeaponActor(true);
//	DestroyWeaponActor(false);
//}
//
//
//
//void URSCosmeticManagerComponent::ApplyWeaponToSlot(bool bIsMain, URSItemInstance* ItemInstance)
//{
//	TObjectPtr<ARSWeaponActor>& SlotActor = bIsMain ? MainWeaponActor : SubWeaponActor;
//
//	if (!CachedCharacter.IsValid())
//	{
//		CacheOwnerCharacter();
//	}
//	ARSCharacter* RSChar = CachedCharacter.Get();
//	if (!RSChar)
//	{
//		return;
//	}
//
//	USkeletalMeshComponent* CharMesh = RSChar->GetMesh();
//	if (!CharMesh)
//	{
//		return;
//	}
//
//	// 아이템 없음 -> 액터 정리
//	if (!ItemInstance)
//	{
//		DestroyWeaponActor(bIsMain);
//		return;
//	}
//
//	const URSItemTemplate* ItemTemplate = ItemInstance->GetTemplate();
//	const URSItemFragment_WeaponCosmetic* CosFrag = FindWeaponCosmeticFragment(ItemInstance);
//
//	// 코스메틱 정보 없으면 해당 슬롯은 비움
//	if (!ItemTemplate || !CosFrag || !CosFrag->WeaponActorClass)
//	{
//		DestroyWeaponActor(bIsMain);
//		return;
//	}
//
//	// 기존 액터가 있으면, "같은 클래스"면 재사용 / 아니면 교체
//	const bool bNeedRespawn = (!SlotActor) || (SlotActor->GetClass() != CosFrag->WeaponActorClass);
//	if (bNeedRespawn)
//	{
//		DestroyWeaponActor(bIsMain);
//		SlotActor = SpawnWeaponActor(ItemTemplate, CosFrag);
//		if (!SlotActor)
//		{
//			return;
//		}
//	}
//
//	// 소켓 결정
//	const FName SocketName = bIsMain ? ResolveEquippedSocket(CosFrag) : ResolveHolsterSocket(CosFrag);
//
//	// Attach
//	FAttachmentTransformRules AttachRules(EAttachmentRule::SnapToTarget, true);
//	SlotActor->AttachToComponent(CharMesh, AttachRules, SocketName);
//
//	const FTransform Rel = bIsMain ? CosFrag->EquippedRelativeTransform : CosFrag->HolsterRelativeTransform;
//	SlotActor->SetActorRelativeTransform(Rel);
//
//	// Mesh 세팅(옵션)
//	if (CosFrag->WeaponMesh)
//	{
//		if (USkeletalMeshComponent* WeaponMeshComp = SlotActor->GetMesh())
//		{
//			WeaponMeshComp->SetSkeletalMesh(CosFrag->WeaponMesh);
//		}
//	}
//
//	// 표시 상태
//	SlotActor->SetActorHiddenInGame(false);
//	SlotActor->SetActorEnableCollision(false);
//	SlotActor->SetActorTickEnabled(false);
//
//	UE_LOG(LogTemp, Verbose, TEXT("[Cos] %s slot updated. Item=%s Actor=%s Socket=%s"),
//		bIsMain ? TEXT("MAIN") : TEXT("SUB"),
//		*GetNameSafe(ItemInstance),
//		*GetNameSafe(SlotActor),
//		*SocketName.ToString());
//}
//
//ARSWeaponActor* URSCosmeticManagerComponent::SpawnWeaponActor(
//	const URSItemTemplate* ItemTemplate,
//	const URSItemFragment_WeaponCosmetic* CosFrag)
//{
//	if (!CachedCharacter.IsValid() || !CosFrag || !CosFrag->WeaponActorClass)
//	{
//		return nullptr;
//	}
//
//	ARSCharacter* RSChar = CachedCharacter.Get();
//	UWorld* World = GetWorld();
//	if (!RSChar || !World)
//	{
//		return nullptr;
//	}
//
//	USkeletalMeshComponent* CharMesh = RSChar->GetMesh();
//	if (!CharMesh)
//	{
//		return nullptr;
//	}
//
//	// 스폰 위치는 일단 손 소켓 기준(스폰 후 Attach로 이동하므로 크게 중요하지 않음)
//	const FTransform SpawnTransform = CharMesh->GetSocketTransform(DefaultEquippedSocketName);
//
//	FActorSpawnParameters Params;
//	Params.Owner = RSChar;
//	Params.Instigator = RSChar;
//	Params.SpawnCollisionHandlingOverride = SpawnCollisionHandlingMethod;
//
//	return World->SpawnActor<ARSWeaponActor>(CosFrag->WeaponActorClass, SpawnTransform, Params);
//}
//
//void URSCosmeticManagerComponent::DestroyWeaponActor(bool bIsMain)
//{
//	TObjectPtr<ARSWeaponActor>& SlotActor = bIsMain ? MainWeaponActor : SubWeaponActor;
//	if (!SlotActor)
//	{
//		return;
//	}
//
//	if (bDestroyWeaponOnUnequip)
//	{
//		SlotActor->Destroy();
//	}
//	else
//	{
//		SlotActor->DetachFromActor(FDetachmentTransformRules::KeepWorldTransform);
//		SlotActor->SetActorHiddenInGame(true);
//		SlotActor->SetActorEnableCollision(false);
//		SlotActor->SetActorTickEnabled(false);
//	}
//
//	SlotActor = nullptr;
//}
//
//FName URSCosmeticManagerComponent::ResolveEquippedSocket(const URSItemFragment_WeaponCosmetic* CosFrag) const
//{
//	if (CosFrag && !CosFrag->AttachSocketName.IsNone())
//	{
//		return CosFrag->AttachSocketName;
//	}
//	return DefaultEquippedSocketName;
//}
//
//FName URSCosmeticManagerComponent::ResolveHolsterSocket(const URSItemFragment_WeaponCosmetic* CosFrag) const
//{
//	if (CosFrag && !CosFrag->HolsterSocketName.IsNone())
//	{
//		return CosFrag->HolsterSocketName;
//	}
//	return DefaultHolsterSocketName;
//}
//
//

#include "Item/Managers/RSCosmeticManagerComponent.h"

#include "Character/RSCharacter.h"
#include "Components/SkeletalMeshComponent.h"
#include "Engine/World.h"
#include "Item/RSItemInstance.h"
#include "Item/RSItemTemplate.h"
#include "Item/RSWeaponActor.h"
#include "Item/Fragments/RSItemFragment_WeaponCosmetic.h"

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
	if (ARSCharacter* RSChar = Cast<ARSCharacter>(GetOwner()))
	{
		CachedCharacter = RSChar;
	}
}

const URSItemFragment_WeaponCosmetic* URSCosmeticManagerComponent::FindWeaponCosmeticFragment(const URSItemInstance* Item) const
{
	if (!Item)
	{
		return nullptr;
	}

	const URSItemTemplate* Template = Item->GetTemplate();
	if (!Template)
	{
		return nullptr;
	}

	return Template->FindFragment<URSItemFragment_WeaponCosmetic>();
}

void URSCosmeticManagerComponent::SyncWeaponCosmetics(URSItemInstance* MainItem, URSItemInstance* SubItem)
{
	ApplyWeaponVisual(true, MainItem);
	ApplyWeaponVisual(false, SubItem);
}

void URSCosmeticManagerComponent::ClearAllWeapons()
{
	DestroyWeaponActor(true);
	DestroyWeaponActor(false);
}

void URSCosmeticManagerComponent::ApplyWeaponVisual(bool bMain, URSItemInstance* Item)
{
	if (!CachedCharacter.IsValid())
	{
		CacheOwnerCharacter();
	}

	ARSCharacter* C = CachedCharacter.Get();
	if (!C)
	{
		return;
	}

	USkeletalMeshComponent* CharMesh = C->GetMesh();
	if (!CharMesh)
	{
		return;
	}

	// 핵심 수정: TObjectPtr 참조로 받는다 (RAW*& 금지)
	TObjectPtr<ARSWeaponActor>& SlotActor = bMain ? MainWeaponActor : SubWeaponActor;

	// 슬롯이 비면 해당 비주얼 제거
	if (!Item)
	{
		DestroyWeaponActor(bMain);
		return;
	}

	const URSItemFragment_WeaponCosmetic* Frag = FindWeaponCosmeticFragment(Item);
	if (!Frag || !Frag->WeaponActorClass)
	{
		DestroyWeaponActor(bMain);
		return;
	}

	ARSWeaponActor* SlotActorRaw = SlotActor.Get();

	// 필요 시 리스폰
	const bool bNeedRespawn = (!SlotActorRaw) || (SlotActorRaw->GetClass() != Frag->WeaponActorClass);
	if (bNeedRespawn)
	{
		DestroyWeaponActor(bMain);
		SlotActor = SpawnWeaponActor(Frag->WeaponActorClass);
		SlotActorRaw = SlotActor.Get();

		if (!SlotActorRaw)
		{
			return;
		}
	}

	// 소켓 결정(Main=손, Sub=홀스터) + None 폴백
	const FName SocketName = bMain ? ResolveEquippedSocket(Frag) : ResolveHolsterSocket(Frag);

	// 소켓 존재 체크 (Attach 직전이 정답 위치)
	if (!CharMesh->DoesSocketExist(SocketName))
	{
		UE_LOG(LogTemp, Warning, TEXT("[Cos] Missing socket: %s (Owner=%s Mesh=%s)"),
			*SocketName.ToString(),
			*GetNameSafe(C),
			*GetNameSafe(CharMesh));

		// 정책 선택지:
		// 1) 안전하게 슬롯 비우기(권장: 잘못된 소켓으로 중앙에 붙는 버그 방지)
		DestroyWeaponActor(bMain);
		return;

		// 2) 또는 그냥 Attach 시도(비추천)
		// (소켓 없으면 root로 붙어서 "등 중앙" 같은 문제가 재발 가능)
	}

	SlotActorRaw->AttachToComponent(
		CharMesh,
		FAttachmentTransformRules(EAttachmentRule::SnapToTarget, true),
		SocketName
	);

	// Attach 후 오프셋 적용
	const FTransform Rel = bMain ? Frag->EquippedRelativeTransform : Frag->HolsterRelativeTransform;
	SlotActorRaw->SetActorRelativeTransform(Rel);

	// 안정화 정책
	SlotActorRaw->SetActorHiddenInGame(false);
	SlotActorRaw->SetActorEnableCollision(false);
	SlotActorRaw->SetActorTickEnabled(false);

	// 메시 적용(옵션)
	if (Frag->WeaponMesh)
	{
		if (USkeletalMeshComponent* WeaponMeshComp = SlotActorRaw->GetMesh())
		{
			WeaponMeshComp->SetSkeletalMesh(Frag->WeaponMesh);
		}
	}
}

ARSWeaponActor* URSCosmeticManagerComponent::SpawnWeaponActor(TSubclassOf<ARSWeaponActor> WeaponActorClass)
{
	if (!CachedCharacter.IsValid())
	{
		CacheOwnerCharacter();
	}

	ARSCharacter* C = CachedCharacter.Get();
	if (!C || !WeaponActorClass)
	{
		return nullptr;
	}

	UWorld* World = GetWorld();
	if (!World)
	{
		return nullptr;
	}

	FActorSpawnParameters Params;
	Params.Owner = C;
	Params.Instigator = C;
	Params.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AlwaysSpawn;

	return World->SpawnActor<ARSWeaponActor>(WeaponActorClass, FTransform::Identity, Params);
}

void URSCosmeticManagerComponent::DestroyWeaponActor(bool bMain)
{
	TObjectPtr<ARSWeaponActor>& SlotActor = bMain ? MainWeaponActor : SubWeaponActor;
	if (!SlotActor)
	{
		return;
	}

	if (bDestroyWeaponOnUnequip)
	{
		SlotActor->Destroy();
	}
	else
	{
		SlotActor->DetachFromActor(FDetachmentTransformRules::KeepWorldTransform);
		SlotActor->SetActorHiddenInGame(true);
		SlotActor->SetActorEnableCollision(false);
		SlotActor->SetActorTickEnabled(false);
	}

	SlotActor = nullptr;
}

FName URSCosmeticManagerComponent::ResolveEquippedSocket(const URSItemFragment_WeaponCosmetic* CosFrag) const
{
	if (CosFrag && !CosFrag->AttachSocketName.IsNone())
	{
		return CosFrag->AttachSocketName;
	}
	return DefaultEquippedSocketName;
}

FName URSCosmeticManagerComponent::ResolveHolsterSocket(const URSItemFragment_WeaponCosmetic* CosFrag) const
{
	if (CosFrag && !CosFrag->HolsterSocketName.IsNone())
	{
		return CosFrag->HolsterSocketName;
	}
	return DefaultHolsterSocketName; // spine_05_Socket_Holster
}
