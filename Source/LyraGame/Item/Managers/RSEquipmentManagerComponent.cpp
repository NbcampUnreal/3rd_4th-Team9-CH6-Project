#include "RSEquipmentManagerComponent.h"

#include "RSEquipManagerComponent.h"
//#include "RSInventoryManagerComponent.h"
//#include "RSItemManagerComponent.h"
#include "Character/LyraCharacter.h"
#include "Data/RSItemData.h"
#include "Engine/ActorChannel.h"
#include "Item/RSItemInstance.h"
#include "Item/Fragments/RSItemFragment_Equipable_Armor.h"
#include "Item/Fragments/RSItemFragment_Equipable_Weapon.h"
#include "Net/UnrealNetwork.h"
#include "Player/LyraPlayerController.h"
#include "Player/LyraPlayerState.h"

#include UE_INLINE_GENERATED_CPP_BY_NAME(RSEquipmentManagerComponent)

void FRSEquipmentEntry::Init(URSItemInstance* InItemInstance, int32 InItemCount)
{
	check(InItemInstance && InItemCount > 0);

	URSEquipManagerComponent* EquipManager = EquipmentManager->GetEquipManager();
	if (EquipManager == nullptr)
		return;

	const URSItemFragment_Equipable* EquippableFragment = InItemInstance->FindFragmentByClass<URSItemFragment_Equipable>();
	if (EquippableFragment == nullptr)
		return;

	if (ItemInstance)
	{
		EquipManager->Unequip(EquipmentSlotType);
	}

	ItemInstance = InItemInstance;

	const URSItemTemplate& ItemTemplate = URSItemData::Get().FindItemTemplateByID(ItemInstance->GetItemTemplateID());
	ItemCount = FMath::Clamp(InItemCount, 1, ItemTemplate.MaxStackCount);

	if (EquippableFragment->EquipmentType == EEquipmentType::Armor || EquipmentManager->IsSameEquipState(EquipmentSlotType, EquipManager->GetCurrentEquipState()))
	{
		EquipManager->Equip(EquipmentSlotType, ItemInstance);
	}
}

URSItemInstance* FRSEquipmentEntry::Reset()
{
	URSEquipManagerComponent* EquipManager = EquipmentManager->GetEquipManager();
	if (EquipManager == nullptr)
		return nullptr;

	if (ItemInstance)
	{
		EquipManager->Unequip(EquipmentSlotType);
	}

	URSItemInstance* RemovedItemInstance = ItemInstance;
	ItemInstance = nullptr;
	ItemCount = 0;

	if (EquipmentManager->IsAllEmpty(EquipManager->GetCurrentEquipState()))
	{
		EquipManager->ChangeEquipState(EEquipState::Unarmed);
	}

	return RemovedItemInstance;
}

bool FRSEquipmentList::NetDeltaSerialize(FNetDeltaSerializeInfo& DeltaParams)
{
	return FFastArraySerializer::FastArrayDeltaSerialize<FRSEquipmentEntry, FRSEquipmentList>(Entries, DeltaParams, *this);
}

void FRSEquipmentList::PostReplicatedAdd(const TArrayView<int32> AddedIndices, int32 FinalSize)
{
	for (int32 AddedIndex : AddedIndices)
	{
		FRSEquipmentEntry& Entry = Entries[AddedIndex];
		Entry.EquipmentManager = EquipmentManager;
		Entry.EquipmentSlotType = (EEquipmentSlotType)AddedIndex;

		if (Entry.GetItemInstance())
		{
			BroadcastChangedMessage((EEquipmentSlotType)AddedIndex, Entry.GetItemInstance(), Entry.GetItemCount());
		}
	}
}

void FRSEquipmentList::PostReplicatedChange(const TArrayView<int32> ChangedIndices, int32 FinalSize)
{
	for (int32 ChangedIndex : ChangedIndices)
	{
		const FRSEquipmentEntry& Entry = Entries[ChangedIndex];
		BroadcastChangedMessage((EEquipmentSlotType)ChangedIndex, Entry.GetItemInstance(), Entry.GetItemCount());
	}
}

void FRSEquipmentList::BroadcastChangedMessage(EEquipmentSlotType EquipmentSlotType, URSItemInstance* ItemInstance, int32 ItemCount)
{
	if (EquipmentManager->OnEquipmentEntryChanged.IsBound())
	{
		EquipmentManager->OnEquipmentEntryChanged.Broadcast(EquipmentSlotType, ItemInstance, ItemCount);
	}
}

URSEquipmentManagerComponent::URSEquipmentManagerComponent(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
	, EquipmentList(this)
{
	bWantsInitializeComponent = true;
	SetIsReplicatedByDefault(true);
}

void URSEquipmentManagerComponent::InitializeComponent()
{
	Super::InitializeComponent();

	if (GetOwner() && GetOwner()->HasAuthority())
	{
		TArray<FRSEquipmentEntry>& Entries = EquipmentList.Entries;
		Entries.SetNum((int32)EEquipmentSlotType::Count);

		for (int32 i = 0; i < Entries.Num(); i++)
		{
			FRSEquipmentEntry& Entry = Entries[i];
			Entry.EquipmentManager = this;
			Entry.EquipmentSlotType = (EEquipmentSlotType)i;
			EquipmentList.MarkItemDirty(Entry);
		}
	}
}

void URSEquipmentManagerComponent::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME(ThisClass, EquipmentList);
}

bool URSEquipmentManagerComponent::ReplicateSubobjects(UActorChannel* Channel, FOutBunch* Bunch, FReplicationFlags* RepFlags)
{
	bool bWroteSomething = Super::ReplicateSubobjects(Channel, Bunch, RepFlags);

	for (FRSEquipmentEntry& Entry : EquipmentList.Entries)
	{
		URSItemInstance* ItemInstance = Entry.ItemInstance;
		if (IsValid(ItemInstance))
		{
			bWroteSomething |= Channel->ReplicateSubobject(ItemInstance, *Bunch, *RepFlags);
		}
	}

	return bWroteSomething;
}

void URSEquipmentManagerComponent::ReadyForReplication()
{
	Super::ReadyForReplication();

	if (IsUsingRegisteredSubObjectList())
	{
		for (const FRSEquipmentEntry& Entry : EquipmentList.Entries)
		{
			URSItemInstance* ItemInstance = Entry.GetItemInstance();
			if (IsValid(ItemInstance))
			{
				AddReplicatedSubObject(ItemInstance);
			}
		}
	}
}

int32 URSEquipmentManagerComponent::CanAddEquipment(int32 ItemTemplateID, EItemRarity ItemRarity, int32 ItemCount, EEquipmentSlotType ToEquipmentSlotType) const
{
	if (ItemTemplateID <= 0 || ItemRarity == EItemRarity::Count || ItemCount <= 0)
		return 0;

	if (ToEquipmentSlotType == EEquipmentSlotType::Unarmed_LeftHand || ToEquipmentSlotType == EEquipmentSlotType::Unarmed_RightHand || ToEquipmentSlotType == EEquipmentSlotType::Count)
		return 0;

	const URSItemTemplate& ItemTemplate = URSItemData::Get().FindItemTemplateByID(ItemTemplateID);
	const URSItemFragment_Equipable* FromEquippableFragment = ItemTemplate.FindFragmentByClass<URSItemFragment_Equipable>();
	if (FromEquippableFragment == nullptr)
		return 0;

	const URSItemInstance* ToItemInstance = GetItemInstance(ToEquipmentSlotType);
	const int32 ToItemCount = GetItemCount(ToEquipmentSlotType);

	if (ToItemInstance)
	{
		const int32 ToTemplateID = ToItemInstance->GetItemTemplateID();
		if (ItemTemplateID != ToTemplateID)
			return 0;

		if (ItemRarity != ToItemInstance->GetItemRarity())
			return 0;

		if (ItemTemplate.MaxStackCount < 2)
			return 0;

		return FMath::Min(ItemCount + ToItemCount, ItemTemplate.MaxStackCount) - ToItemCount;
	}
	else
	{
		if (FromEquippableFragment->EquipmentType == EEquipmentType::Weapon)
		{
			const URSItemFragment_Equipable_Weapon* FromWeaponFragment = Cast<URSItemFragment_Equipable_Weapon>(FromEquippableFragment);
			EWeaponHandType FromWeaponHandType = FromWeaponFragment->WeaponHandType;

			if (IsSameWeaponHandType(ToEquipmentSlotType, FromWeaponHandType) == false)
				return 0;

			if (IsPrimaryWeaponSlot(ToEquipmentSlotType))
			{
				if (FromWeaponHandType == EWeaponHandType::LeftHand || FromWeaponHandType == EWeaponHandType::RightHand)
				{
					return (GetItemInstance(EEquipmentSlotType::Primary_TwoHand) == nullptr) ? ItemCount : 0;
				}
				else if (FromWeaponHandType == EWeaponHandType::TwoHand)
				{
					return (GetItemInstance(EEquipmentSlotType::Primary_LeftHand) == nullptr && GetItemInstance(EEquipmentSlotType::Primary_RightHand) == nullptr) ? ItemCount : 0;
				}
			}
			else if (IsSecondaryWeaponSlot(ToEquipmentSlotType))
			{
				if (FromWeaponHandType == EWeaponHandType::LeftHand || FromWeaponHandType == EWeaponHandType::RightHand)
				{
					return (GetItemInstance(EEquipmentSlotType::Secondary_TwoHand) == nullptr) ? ItemCount : 0;
				}
				else if (FromWeaponHandType == EWeaponHandType::TwoHand)
				{
					return (GetItemInstance(EEquipmentSlotType::Secondary_LeftHand) == nullptr && GetItemInstance(EEquipmentSlotType::Secondary_RightHand) == nullptr) ? ItemCount : 0;
				}
			}
		}
		else if (FromEquippableFragment->EquipmentType == EEquipmentType::Armor)
		{
			const URSItemFragment_Equipable_Armor* FromArmorFragment = Cast<URSItemFragment_Equipable_Armor>(FromEquippableFragment);
			return IsSameArmorType(ToEquipmentSlotType, FromArmorFragment->ArmorType) ? ItemCount : 0;
		}
		else if (FromEquippableFragment->EquipmentType == EEquipmentType::Utility)
		{
			return IsUtilitySlot(ToEquipmentSlotType) ? ItemCount : 0;
		}
	}

	return 0;
}

void URSEquipmentManagerComponent::AddUnarmedEquipments(TSubclassOf<URSItemTemplate> LeftHandClass, TSubclassOf<URSItemTemplate> RightHandClass)
{
	check(HasAuthority());

	SetEquipment(EEquipmentSlotType::Unarmed_LeftHand, LeftHandClass, EItemRarity::Poor, 1);
	SetEquipment(EEquipmentSlotType::Unarmed_RightHand, RightHandClass, EItemRarity::Poor, 1);
}

void URSEquipmentManagerComponent::AddEquipment_Unsafe(EEquipmentSlotType EquipmentSlotType, URSItemInstance* ItemInstance, int32 ItemCount)
{
	check(HasAuthority());

	if (EquipmentSlotType == EEquipmentSlotType::Count || ItemCount <= 0)
		return;

	FRSEquipmentEntry& Entry = EquipmentList.Entries[(int32)EquipmentSlotType];

	if (Entry.GetItemInstance())
	{
		Entry.ItemCount += ItemCount;
		EquipmentList.MarkItemDirty(Entry);
	}
	else
	{
		if (ItemInstance == nullptr)
			return;

		Entry.Init(ItemInstance, ItemCount);

		if (IsUsingRegisteredSubObjectList() && IsReadyForReplication() && ItemInstance)
		{
			AddReplicatedSubObject(ItemInstance);
		}

		EquipmentList.MarkItemDirty(Entry);
	}
}

void URSEquipmentManagerComponent::SetEquipment(EEquipmentSlotType EquipmentSlotType, TSubclassOf<URSItemTemplate> ItemTemplateClass, EItemRarity ItemRarity, int32 ItemCount)
{
	check(HasAuthority());

	if (EquipmentSlotType == EEquipmentSlotType::Count || ItemTemplateClass == nullptr || ItemRarity == EItemRarity::Count || ItemCount <= 0)
		return;

	const int32 ItemTemplateID = URSItemData::Get().FindItemTemplateIDByClass(ItemTemplateClass);
	const URSItemTemplate& ItemTemplate = URSItemData::Get().FindItemTemplateByID(ItemTemplateID);

	ItemCount = FMath::Clamp(ItemCount, 1, ItemTemplate.MaxStackCount);

	const URSItemFragment_Equipable* EquippableFragment = ItemTemplate.FindFragmentByClass<URSItemFragment_Equipable>();
	if (EquippableFragment == nullptr)
		return;

	FRSEquipmentEntry& Entry = EquipmentList.Entries[(int32)EquipmentSlotType];
	Entry.Reset();

	if (EquippableFragment->EquipmentType == EEquipmentType::Weapon)
	{
		const URSItemFragment_Equipable_Weapon* WeaponFragment = Cast<URSItemFragment_Equipable_Weapon>(EquippableFragment);
		EWeaponHandType WeaponHandType = WeaponFragment->WeaponHandType;

		if (IsPrimaryWeaponSlot(EquipmentSlotType))
		{
			if (WeaponHandType == EWeaponHandType::LeftHand || WeaponHandType == EWeaponHandType::RightHand)
			{
				RemoveEquipment_Unsafe(EEquipmentSlotType::Primary_TwoHand, 1);
			}
			else if (WeaponHandType == EWeaponHandType::TwoHand)
			{
				RemoveEquipment_Unsafe(EEquipmentSlotType::Primary_LeftHand, 1);
				RemoveEquipment_Unsafe(EEquipmentSlotType::Primary_RightHand, 1);
			}
		}
		else if (IsSecondaryWeaponSlot(EquipmentSlotType))
		{
			if (WeaponHandType == EWeaponHandType::LeftHand || WeaponHandType == EWeaponHandType::RightHand)
			{
				RemoveEquipment_Unsafe(EEquipmentSlotType::Secondary_TwoHand, 1);
			}
			else if (WeaponHandType == EWeaponHandType::TwoHand)
			{
				RemoveEquipment_Unsafe(EEquipmentSlotType::Secondary_LeftHand, 1);
				RemoveEquipment_Unsafe(EEquipmentSlotType::Secondary_RightHand, 1);
			}
		}
	}

	URSItemInstance* AddedItemInstance = NewObject<URSItemInstance>();
	AddedItemInstance->Init(ItemTemplateID, ItemRarity);
	Entry.Init(AddedItemInstance, ItemCount);

	if (IsUsingRegisteredSubObjectList() && IsReadyForReplication() && AddedItemInstance)
	{
		AddReplicatedSubObject(AddedItemInstance);
	}

	EquipmentList.MarkItemDirty(Entry);

	if (IsWeaponSlot(EquipmentSlotType))
	{
		if (URSEquipManagerComponent* EquipManager = GetEquipManager())
		{
			EWeaponSlotType WeaponSlotType = URSEquipManagerComponent::ConvertToWeaponSlotType(EquipmentSlotType);
			EEquipState EquipState = URSEquipManagerComponent::ConvertToEquipState(WeaponSlotType);
			if (EquipManager->GetCurrentEquipState() != EquipState)
			{
				EquipManager->ChangeEquipState(EquipState);
			}
		}
	}
}

URSItemInstance* URSEquipmentManagerComponent::RemoveEquipment_Unsafe(EEquipmentSlotType EquipmentSlotType, int32 ItemCount)
{
	check(HasAuthority());

	FRSEquipmentEntry& Entry = EquipmentList.Entries[(int32)EquipmentSlotType];
	URSItemInstance* ItemInstance = Entry.GetItemInstance();

	Entry.ItemCount -= ItemCount;
	if (Entry.GetItemCount() <= 0)
	{
		URSItemInstance* RemovedItemInstance = Entry.Reset();
		if (IsUsingRegisteredSubObjectList() && RemovedItemInstance)
		{
			RemoveReplicatedSubObject(RemovedItemInstance);
		}
	}

	EquipmentList.MarkItemDirty(Entry);
	return ItemInstance;
}

bool URSEquipmentManagerComponent::IsWeaponSlot(EEquipmentSlotType EquipmentSlotType)
{
	return (EEquipmentSlotType::Unarmed_LeftHand <= EquipmentSlotType && EquipmentSlotType <= EEquipmentSlotType::Secondary_TwoHand);
}

bool URSEquipmentManagerComponent::IsArmorSlot(EEquipmentSlotType EquipmentSlotType)
{
	return (EEquipmentSlotType::Helmet <= EquipmentSlotType && EquipmentSlotType <= EEquipmentSlotType::Foot);
}

bool URSEquipmentManagerComponent::IsUtilitySlot(EEquipmentSlotType EquipmentSlotType)
{
	return (EEquipmentSlotType::Utility_Primary <= EquipmentSlotType && EquipmentSlotType <= EEquipmentSlotType::Utility_Quaternary);
}

bool URSEquipmentManagerComponent::IsSameEquipState(EEquipmentSlotType EquipmentSlotType, EEquipState WeaponEquipState)
{
	return (((EquipmentSlotType == EEquipmentSlotType::Unarmed_LeftHand || EquipmentSlotType == EEquipmentSlotType::Unarmed_RightHand) && WeaponEquipState == EEquipState::Unarmed) ||
		((EquipmentSlotType == EEquipmentSlotType::Primary_LeftHand || EquipmentSlotType == EEquipmentSlotType::Primary_RightHand || EquipmentSlotType == EEquipmentSlotType::Primary_TwoHand) && WeaponEquipState == EEquipState::Weapon_Primary) ||
		((EquipmentSlotType == EEquipmentSlotType::Secondary_LeftHand || EquipmentSlotType == EEquipmentSlotType::Secondary_RightHand || EquipmentSlotType == EEquipmentSlotType::Secondary_TwoHand) && WeaponEquipState == EEquipState::Weapon_Secondary) ||
		(EquipmentSlotType == EEquipmentSlotType::Utility_Primary && WeaponEquipState == EEquipState::Utility_Primary) ||
		(EquipmentSlotType == EEquipmentSlotType::Utility_Secondary && WeaponEquipState == EEquipState::Utility_Secondary) ||
		(EquipmentSlotType == EEquipmentSlotType::Utility_Tertiary && WeaponEquipState == EEquipState::Utility_Tertiary) ||
		(EquipmentSlotType == EEquipmentSlotType::Utility_Quaternary && WeaponEquipState == EEquipState::Utility_Quaternary));
}

bool URSEquipmentManagerComponent::IsSameWeaponHandType(EEquipmentSlotType EquipmentSlotType, EWeaponHandType WeaponHandType)
{
	return (((EquipmentSlotType == EEquipmentSlotType::Primary_LeftHand || EquipmentSlotType == EEquipmentSlotType::Secondary_LeftHand) && WeaponHandType == EWeaponHandType::LeftHand) ||
		((EquipmentSlotType == EEquipmentSlotType::Primary_RightHand || EquipmentSlotType == EEquipmentSlotType::Secondary_RightHand) && WeaponHandType == EWeaponHandType::RightHand) ||
		((EquipmentSlotType == EEquipmentSlotType::Primary_TwoHand || EquipmentSlotType == EEquipmentSlotType::Secondary_TwoHand) && WeaponHandType == EWeaponHandType::TwoHand));
}

bool URSEquipmentManagerComponent::IsSameArmorType(EEquipmentSlotType EquipmentSlotType, EArmorType ArmorType)
{
	return ((EquipmentSlotType == EEquipmentSlotType::Helmet && ArmorType == EArmorType::Helmet) || (EquipmentSlotType == EEquipmentSlotType::Chest && ArmorType == EArmorType::Chest) ||
		(EquipmentSlotType == EEquipmentSlotType::Legs && ArmorType == EArmorType::Legs) || (EquipmentSlotType == EEquipmentSlotType::Hands && ArmorType == EArmorType::Hands) ||
		(EquipmentSlotType == EEquipmentSlotType::Foot && ArmorType == EArmorType::Foot));
}

bool URSEquipmentManagerComponent::IsPrimaryWeaponSlot(EEquipmentSlotType EquipmentSlotType)
{
	return (EEquipmentSlotType::Primary_LeftHand <= EquipmentSlotType && EquipmentSlotType <= EEquipmentSlotType::Primary_TwoHand);
}

bool URSEquipmentManagerComponent::IsSecondaryWeaponSlot(EEquipmentSlotType EquipmentSlotType)
{
	return (EEquipmentSlotType::Secondary_LeftHand <= EquipmentSlotType && EquipmentSlotType <= EEquipmentSlotType::Secondary_TwoHand);
}

const URSItemInstance* URSEquipmentManagerComponent::FindPairItemInstance(const URSItemInstance* InBaseItemInstance, EEquipmentSlotType& OutEquipmentSlotType) const
{
	if (InBaseItemInstance == nullptr)
		return nullptr;

	const URSItemFragment_Equipable* BaseEquippableFragment = InBaseItemInstance->FindFragmentByClass<URSItemFragment_Equipable>();
	if (BaseEquippableFragment == nullptr)
		return nullptr;

	URSItemInstance* SelectedItemInstance = nullptr;

	if (BaseEquippableFragment->EquipmentType == EEquipmentType::Weapon)
	{
		const URSItemFragment_Equipable_Weapon* BaseWeaponFragment = Cast<URSItemFragment_Equipable_Weapon>(BaseEquippableFragment);

		const TArray<FRSEquipmentEntry>& Entries = EquipmentList.GetAllEntries();
		for (int32 i = 0; i < (int32)EEquipmentSlotType::Count; i++)
		{
			if (IsWeaponSlot((EEquipmentSlotType)i) == false)
				continue;

			const FRSEquipmentEntry& Entry = Entries[i];
			if (URSItemInstance* ItemInstance = Entry.ItemInstance)
			{
				if (const URSItemFragment_Equipable_Weapon* EquippedWeaponFragment = ItemInstance->FindFragmentByClass<URSItemFragment_Equipable_Weapon>())
				{
					if ((BaseWeaponFragment->WeaponType == EquippedWeaponFragment->WeaponType) && (BaseWeaponFragment->WeaponHandType == EquippedWeaponFragment->WeaponHandType))
					{
						if (InBaseItemInstance == ItemInstance)
						{
							SelectedItemInstance = nullptr;
							break;
						}

						if (SelectedItemInstance == nullptr)
						{
							SelectedItemInstance = ItemInstance;
							OutEquipmentSlotType = (EEquipmentSlotType)i;
						}
					}
				}
			}
		}
	}
	else if (BaseEquippableFragment->EquipmentType == EEquipmentType::Armor)
	{
		const URSItemFragment_Equipable_Armor* BaseArmorFragment = Cast<URSItemFragment_Equipable_Armor>(BaseEquippableFragment);
		OutEquipmentSlotType = URSEquipManagerComponent::ConvertToEquipmentSlotType(BaseArmorFragment->ArmorType);
		SelectedItemInstance = GetItemInstance(OutEquipmentSlotType);
	}

	if (InBaseItemInstance == SelectedItemInstance)
	{
		SelectedItemInstance = nullptr;
	}

	return SelectedItemInstance;
}

bool URSEquipmentManagerComponent::IsAllEmpty(EEquipState EquipState) const
{
	if (EquipState == EEquipState::Count)
		return true;

	if (EquipState == EEquipState::Unarmed)
		return false;

	bool bAllEmpty = true;
	for (EEquipmentSlotType SlotType : URSEquipManagerComponent::GetEquipmentSlotsByEquipState(EquipState))
	{
		const FRSEquipmentEntry& Entry = EquipmentList.Entries[(int32)SlotType];
		if (Entry.ItemInstance)
		{
			bAllEmpty = false;
			break;
		}
	}
	return bAllEmpty;
}

ALyraCharacter* URSEquipmentManagerComponent::GetCharacter() const
{
	return Cast<ALyraCharacter>(GetOwner());
}

ALyraPlayerController* URSEquipmentManagerComponent::GetPlayerController() const
{
	if (ALyraCharacter* LyraCharacter = GetCharacter())
	{
		return LyraCharacter->GetLyraPlayerController();
	}
	return nullptr;
}

URSEquipManagerComponent* URSEquipmentManagerComponent::GetEquipManager() const
{
	URSEquipManagerComponent* EquipManager = nullptr;
	if (ALyraCharacter* Character = GetCharacter())
	{
		EquipManager = Character->FindComponentByClass<URSEquipManagerComponent>();
	}
	return EquipManager;
}

URSItemInstance* URSEquipmentManagerComponent::GetItemInstance(EEquipmentSlotType EquipmentSlotType) const
{
	if (EquipmentSlotType == EEquipmentSlotType::Count)
		return nullptr;

	const TArray<FRSEquipmentEntry>& Entries = EquipmentList.GetAllEntries();
	const FRSEquipmentEntry& Entry = Entries[(int32)EquipmentSlotType];

	return Entry.GetItemInstance();
}

int32 URSEquipmentManagerComponent::GetItemCount(EEquipmentSlotType EquipmentSlotType) const
{
	if (EquipmentSlotType == EEquipmentSlotType::Count)
		return 0;

	const TArray<FRSEquipmentEntry>& Entries = EquipmentList.GetAllEntries();
	const FRSEquipmentEntry& Entry = Entries[(int32)EquipmentSlotType];

	return Entry.GetItemCount();
}

const TArray<FRSEquipmentEntry>& URSEquipmentManagerComponent::GetAllEntries() const
{
	return EquipmentList.GetAllEntries();
}

void URSEquipmentManagerComponent::GetAllWeaponItemInstances(TArray<URSItemInstance*>& OutItemInstances) const
{
	OutItemInstances.Reset();

	const TArray<FRSEquipmentEntry>& Entries = EquipmentList.GetAllEntries();

	for (int32 i = 0; i < (int32)EEquipmentSlotType::Count; i++)
	{
		if (IsWeaponSlot((EEquipmentSlotType)i) == false)
			continue;

		const FRSEquipmentEntry& Entry = Entries[i];
		if (Entry.ItemInstance)
		{
			OutItemInstances.Add(Entry.ItemInstance);
		}
	}
}
