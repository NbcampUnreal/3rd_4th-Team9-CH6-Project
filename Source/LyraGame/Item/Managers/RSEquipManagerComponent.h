//#pragma once
//
//#include "RSDefine.h"
//#include "AbilitySystem/LyraAbilitySet.h"
//#include "Components/PawnComponent.h"
//#include "Net/Serialization/FastArraySerializer.h"
//#include "RSEquipManagerComponent.generated.h"
//
//class ARSPocketWorldAttachment;
//class ALyraCharacter;
//class ALyraPlayerController;
//class ARSEquipmentBase;
//class URSItemInstance;
//class URSEquipManagerComponent;
//class URSEquipmentManagerComponent;
//
//DECLARE_MULTICAST_DELEGATE_TwoParams(FOnEquipStateChanged, EEquipState, EEquipState);
//
//USTRUCT(BlueprintType)
//struct FRSEquipEntry : public FFastArraySerializerItem
//{
//	GENERATED_BODY()
//	
//private:
//	void Init(URSItemInstance* InItemInstance);
//	
//	void Equip();
//	void Unequip();
//
//public:
//	URSItemInstance* GetItemInstance() const { return ItemInstance; }
//
//	void SetEquipmentActor(ARSEquipmentBase* InEquipmentActor) { SpawnedEquipmentActor = InEquipmentActor; }
//	ARSEquipmentBase* GetEquipmentActor() const { return SpawnedEquipmentActor; }
//	
//private:
//	friend struct FRSEquipList;
//	friend class URSEquipManagerComponent;
//	
//	UPROPERTY()
//	TObjectPtr<URSItemInstance> ItemInstance;
//
//private:
//	UPROPERTY(NotReplicated)
//	EEquipmentSlotType EquipmentSlotType = EEquipmentSlotType::Count;
//
//private:
//	UPROPERTY(NotReplicated)
//	TObjectPtr<ARSEquipmentBase> SpawnedEquipmentActor;
//
//	UPROPERTY(NotReplicated)
//	TObjectPtr<ARSEquipmentBase> SpawnedPocketWorldActor;
//
//private:
//	UPROPERTY(NotReplicated)
//	FLyraAbilitySet_GrantedHandles BaseAbilitySetHandles;
//
//	UPROPERTY(NotReplicated)
//	FActiveGameplayEffectHandle BaseStatHandle;
//
//public:
//	UPROPERTY(NotReplicated)
//	TObjectPtr<URSEquipManagerComponent> EquipManager;
//};
//
//USTRUCT(BlueprintType)
//struct FRSEquipList : public FFastArraySerializer
//{
//	GENERATED_BODY()
//
//public:
//	FRSEquipList() : EquipManager(nullptr) { }
//	FRSEquipList(URSEquipManagerComponent* InOwnerComponent) : EquipManager(InOwnerComponent) { }
//
//public:
//	bool NetDeltaSerialize(FNetDeltaSerializeInfo& DeltaParams);
//	void PostReplicatedAdd(const TArrayView<int32> AddedIndices, int32 FinalSize);
//	void PostReplicatedChange(const TArrayView<int32> ChangedIndices, int32 FinalSize);
//
//private:
//	void Equip(EEquipmentSlotType EquipmentSlotType, URSItemInstance* ItemInstance);
//	void Unequip(EEquipmentSlotType EquipmentSlotType);
//	
//public:
//	TArray<FRSEquipEntry>& GetAllEntries() { return Entries; }
//	
//private:
//	friend class URSEquipManagerComponent;
//
//	UPROPERTY()
//	TArray<FRSEquipEntry> Entries;
//	
//	UPROPERTY(NotReplicated)
//	TObjectPtr<URSEquipManagerComponent> EquipManager;
//};
//
//template<>
//struct TStructOpsTypeTraits<FRSEquipList> : public TStructOpsTypeTraitsBase2<FRSEquipList>
//{
//	enum
//	{
//		WithNetDeltaSerializer = true
//	};
//};
//
//UCLASS(BlueprintType)
//class URSEquipManagerComponent : public UPawnComponent
//{
//	GENERATED_BODY()
//	
//public:
//	URSEquipManagerComponent(const FObjectInitializer& ObjectInitializer = FObjectInitializer::Get());
//
//protected:
//	virtual void InitializeComponent() override;
//	virtual void UninitializeComponent() override;
//	
//	virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;
//	virtual bool ReplicateSubobjects(UActorChannel* Channel, FOutBunch* Bunch, FReplicationFlags* RepFlags) override;
//	virtual void ReadyForReplication() override;
//	
//public:
//	void Equip(EEquipmentSlotType EquipmentSlotType, URSItemInstance* ItemInstance);
//	void Unequip(EEquipmentSlotType EquipmentSlotType);
//
//	void EquipCurrentSlots();
//	void UnequipCurrentSlots();
//	
//public:
//	UFUNCTION(BlueprintCallable, BlueprintAuthorityOnly)
//	void ChangeEquipState(EEquipState NewEquipState);
//
//	UFUNCTION(BlueprintCallable)
//	bool CanChangeEquipState(EEquipState NewEquipState) const;
//
//private:
//	UFUNCTION()
//	void OnRep_CurrentEquipState(EEquipState PrevEquipState);
//
//	void BroadcastChangedMessage(EEquipState PrevEquipState, EEquipState NewEquipState);
//
//public:
//	ALyraCharacter* GetCharacter() const;
//	ALyraPlayerController* GetPlayerController() const;
//	
//	TArray<FRSEquipEntry>& GetAllEntries();
//	UAbilitySystemComponent* GetAbilitySystemComponent() const;
//	URSEquipmentManagerComponent* GetEquipmentManager() const;
//
//	static EEquipmentSlotType ConvertToEquipmentSlotType(EWeaponHandType WeaponHandType, EEquipState EquipState);
//	static EEquipmentSlotType ConvertToEquipmentSlotType(EWeaponHandType WeaponHandType, EWeaponSlotType WeaponSlotType);
//	static EEquipmentSlotType ConvertToEquipmentSlotType(EArmorType ArmorType);
//	static EEquipmentSlotType ConvertToEquipmentSlotType(EUtilitySlotType UtilitySlotType);
//	
//	static EWeaponSlotType ConvertToWeaponSlotType(EEquipmentSlotType EquipmentSlotType);
//	static EWeaponHandType ConvertToWeaponHandType(EEquipmentSlotType EquipmentSlotType);
//	static EArmorType ConvertToArmorType(EEquipmentSlotType EquipmentSlotType);
//	static EUtilitySlotType ConvertToUtilitySlotType(EEquipmentSlotType EquipmentSlotType);
//	static EEquipState ConvertToEquipState(EWeaponSlotType WeaponSlotType);
//	static EEquipState ConvertToEquipState(EUtilitySlotType UtilitySlotType);
//
//	static bool IsWeaponEquipState(EEquipState EquipState);
//	static bool IsUtilityEquipState(EEquipState EquipState);
//
//	static const TArray<EEquipmentSlotType>& GetEquipmentSlotsByEquipState(EEquipState EquipState);
//	
//	UFUNCTION(BlueprintCallable)
//	void ChangeShouldHiddenEquipments(bool bNewShouldHiddenEquipments);
//	
//	bool ShouldHiddenEquipments() const { return bShouldHiddenEquipments; }
//	EEquipState GetCurrentEquipState() const { return CurrentEquipState; }
//	
//	ARSEquipmentBase* GetFirstEquippedActor() const;
//	ARSEquipmentBase* GetEquippedActor(EWeaponHandType WeaponHandType) const;
//	void GetAllEquippedActors(TArray<ARSEquipmentBase*>& OutActors) const;
//
//	URSItemInstance* GetFirstEquippedItemInstance(bool bIgnoreArmor = true) const;
//	URSItemInstance* GetEquippedItemInstance(EArmorType ArmorType) const;
//	URSItemInstance* GetEquippedItemInstance(EWeaponHandType WeaponHandType) const;
//	URSItemInstance* GetEquippedItemInstance(EEquipmentSlotType EquipmentSlotType) const;
//
//public:
//	FOnEquipStateChanged OnEquipStateChanged;
//	
//private:
//	UPROPERTY(Replicated)
//	FRSEquipList EquipList;
//
//	UPROPERTY(ReplicatedUsing=OnRep_CurrentEquipState)
//	EEquipState CurrentEquipState = EEquipState::Count;
//
//	UPROPERTY(Replicated)
//	bool bShouldHiddenEquipments = false;
//};
