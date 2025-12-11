//#include "RSChestBase.h"
//
//#include "Components/ArrowComponent.h"
//#include "Data/RSItemData.h"
//#include "Item/RSItemTemplate.h"
//#include "Item/Managers/RSInventoryManagerComponent.h"
//#include "Net/UnrealNetwork.h"
//
//#include UE_INLINE_GENERATED_CPP_BY_NAME(RSChestBase)
//
//ARSChestBase::ARSChestBase(const FObjectInitializer& ObjectInitializer)
//	: Super(ObjectInitializer)
//{
//	PrimaryActorTick.bCanEverTick = false;
//	PrimaryActorTick.bStartWithTickEnabled = false;
//	
//	ArrowComponent = CreateDefaultSubobject<UArrowComponent>(TEXT("ArrowComponent"));
//	SetRootComponent(ArrowComponent);
//	
//    MeshComponent = CreateDefaultSubobject<USkeletalMeshComponent>(TEXT("MeshComponent"));
//	MeshComponent->SetupAttachment(GetRootComponent());
//	MeshComponent->SetCollisionProfileName(TEXT("Interactable"));
//	MeshComponent->SetRelativeRotation(FRotator(0.f, -90.f, 0.f));
//	MeshComponent->SetCanEverAffectNavigation(true);
//	MeshComponent->VisibilityBasedAnimTickOption = EVisibilityBasedAnimTickOption::OnlyTickMontagesAndRefreshBonesWhenPlayingMontages;
//	
//	InventoryManager = CreateDefaultSubobject<URSInventoryManagerComponent>(TEXT("InventoryManager"));
//}
//
//void ARSChestBase::BeginPlay()
//{
//	Super::BeginPlay();
//
//	if (HasAuthority() == false)
//		return;
//
//	const TArray<TSubclassOf<URSItemTemplate>>& WeaponItemTemplateClasses = URSItemData::Get().GetWeaponItemTemplateClasses();
//	const TArray<TSubclassOf<URSItemTemplate>>& ArmorItemTemplateClasses = URSItemData::Get().GetArmorItemTemplateClasses();
//
//	bool bItemAdded = false;
//	
//	for (const FItemAddRule& ItemAddRule : ItemAddRules)
//	{
//		if (ItemAddRule.ItemAddType == EItemAddType::None)
//			continue;
//
//		if (FMath::RandRange(0.f, 100.f) > ItemAddRule.ItemAddTypeRate)
//			continue;
//
//		const TArray<TSubclassOf<URSItemTemplate>>* SelectedItemTemplateClasses = nullptr;
//		
//		switch (ItemAddRule.ItemAddType)
//		{
//		case EItemAddType::Weapon:	SelectedItemTemplateClasses = &WeaponItemTemplateClasses;				break;
//		case EItemAddType::Armor:	SelectedItemTemplateClasses = &ArmorItemTemplateClasses;				break;
//		case EItemAddType::Custom:	SelectedItemTemplateClasses = &ItemAddRule.CustomItemTemplateClasses;	break;
//		}
//
//		if (SelectedItemTemplateClasses)
//		{
//			int32 SelectedItemTemplateIndex = FMath::RandRange(0, SelectedItemTemplateClasses->Num() - 1);
//			TSubclassOf<URSItemTemplate> SelectedItemTemplateClass = (*SelectedItemTemplateClasses)[SelectedItemTemplateIndex];
//			
//			int32 SelectedItemRarityIndex = FMath::RandRange(0, ItemAddRule.ItemRarities.Num() - 1);
//			EItemRarity SelectedItemRarity = ItemAddRule.ItemRarities[SelectedItemRarityIndex];
//			
//			InventoryManager->TryAddItemByRarity(SelectedItemTemplateClass, SelectedItemRarity, 1);
//			bItemAdded = true;
//		}
//	}
//
//	if (bShouldFallback && FallbackItemTemplateClass && bItemAdded == false)
//	{
//		InventoryManager->TryAddItemByRarity(FallbackItemTemplateClass, FallbackItemItemRarity, 1);
//	}
//}
//
//void ARSChestBase::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
//{
//	Super::GetLifetimeReplicatedProps(OutLifetimeProps);
//
//	DOREPLIFETIME(ThisClass, ChestState);
//}
//
//FRSInteractionInfo ARSChestBase::GetPreInteractionInfo(const FRSInteractionQuery& InteractionQuery) const
//{
//	switch (ChestState)
//	{
//	case EChestState::Open:		return OpenedInteractionInfo;
//	case EChestState::Close:	return ClosedInteractionInfo;
//	default:					return FRSInteractionInfo();
//	}
//}
//
//void ARSChestBase::GetMeshComponents(TArray<UMeshComponent*>& OutMeshComponents) const
//{
//	OutMeshComponents.Add(MeshComponent);
//}
//
//void ARSChestBase::SetChestState(EChestState NewChestState)
//{
//	if (HasAuthority() == false || NewChestState == ChestState)
//		return;
//
//	ChestState = NewChestState;
//	OnRep_ChestState();
//}
//
//void ARSChestBase::OnRep_ChestState()
//{
//	if (UAnimInstance* AnimInstance = MeshComponent->GetAnimInstance())
//	{
//		UAnimMontage* SelectedMontage = nullptr;
//		
//		switch (ChestState)
//		{
//		case EChestState::Open:		SelectedMontage = OpenMontage;	break;
//		case EChestState::Close:	SelectedMontage = CloseMontage; break;
//		}
//
//		if (SelectedMontage)
//		{
//			AnimInstance->Montage_Play(SelectedMontage);
//		}
//	}
//}
