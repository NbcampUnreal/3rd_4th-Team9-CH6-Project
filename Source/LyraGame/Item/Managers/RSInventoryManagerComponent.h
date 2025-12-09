//#pragma once
//
//#include "RSDefine.h"
//#include "Net/Serialization/FastArraySerializer.h"
//#include "RSInventoryManagerComponent.generated.h"
//
//class URSItemTemplate;
//class URSItemInstance;
//struct FRSItemRarityProbability;
//class URSEquipmentManagerComponent;
//class URSInventoryManagerComponent;
//
//DECLARE_MULTICAST_DELEGATE_ThreeParams(FOnInventoryEntryChanged, const FIntPoint&/*ItemSlotPos*/, URSItemInstance*, int32/*ItemCount*/);
//
//USTRUCT(BlueprintType)
//struct FRSInventoryEntry : public FFastArraySerializerItem
//{
//	GENERATED_BODY()
//
//private:
//	URSItemInstance* Init(int32 InItemTemplateID, int32 InItemCount, EItemRarity InItemRarity);
//	void Init(URSItemInstance* InItemInstance, int32 InItemCount);
//	URSItemInstance* Reset();
//	
//public:
//	URSItemInstance* GetItemInstance() const { return ItemInstance; }
//	int32 GetItemCount() const { return ItemCount; }
//	
//private:
//	friend struct FRSInventoryList;
//	friend class URSInventoryManagerComponent;
//	friend class URSItemManagerComponent;
//	
//	UPROPERTY()
//	TObjectPtr<URSItemInstance> ItemInstance;
//
//	UPROPERTY()
//	int32 ItemCount = 0;
//};
//
//USTRUCT(BlueprintType)
//struct FRSInventoryList : public FFastArraySerializer
//{
//	GENERATED_BODY()
//
//public:
//	FRSInventoryList() : InventoryManager(nullptr) { }
//	FRSInventoryList(URSInventoryManagerComponent* InOwnerComponent) : InventoryManager(InOwnerComponent) { }
//
//public:
//	bool NetDeltaSerialize(FNetDeltaSerializeInfo& DeltaParams);
//	void PostReplicatedAdd(const TArrayView<int32> AddedIndices, int32 FinalSize);
//	void PostReplicatedChange(const TArrayView<int32> ChangedIndices, int32 FinalSize);
//
//private:
//	void BroadcastChangedMessage(const FIntPoint& ItemSlotPos, URSItemInstance* ItemInstance, int32 ItemCount);
//	
//public:
//	const TArray<FRSInventoryEntry>& GetAllEntries() const { return Entries; }
//	
//private:
//	friend class URSInventoryManagerComponent;
//	friend class URSEquipmentManagerComponent;
//	friend class URSItemManagerComponent;
//	
//	UPROPERTY()
//	TArray<FRSInventoryEntry> Entries;
//	
//	UPROPERTY(NotReplicated)
//	TObjectPtr<URSInventoryManagerComponent> InventoryManager;
//};
//
//template<>
//struct TStructOpsTypeTraits<FRSInventoryList> : public TStructOpsTypeTraitsBase2<FRSInventoryList>
//{
//	enum
//	{
//		WithNetDeltaSerializer = true
//	};
//};
//
//UCLASS(BlueprintType, Blueprintable)
//class URSInventoryManagerComponent : public UActorComponent
//{
//	GENERATED_BODY()
//
//public:
//	URSInventoryManagerComponent(const FObjectInitializer& ObjectInitializer = FObjectInitializer::Get());
//
//protected:
//	virtual void InitializeComponent() override;
//	virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;
//	virtual bool ReplicateSubobjects(UActorChannel* Channel, FOutBunch* Bunch, FReplicationFlags* RepFlags) override;
//	virtual void ReadyForReplication() override;
//	
//public:
//	int32 CanMoveOrMergeItem(URSInventoryManagerComponent* OtherComponent, const FIntPoint& FromItemSlotPos, const FIntPoint& ToItemSlotPos) const;
//	int32 CanMoveOrMergeItem(URSEquipmentManagerComponent* OtherComponent, EEquipmentSlotType FromEquipmentSlotType, const FIntPoint& ToItemSlotPos) const;
//
//	int32 CanMoveOrMergeItem_Quick(URSInventoryManagerComponent* OtherComponent, const FIntPoint& FromItemSlotPos, TArray<FIntPoint>& OutToItemSlotPoses, TArray<int32>& OutToItemCounts) const;
//	int32 CanMoveOrMergeItem_Quick(URSEquipmentManagerComponent* OtherComponent, EEquipmentSlotType FromEquipmentSlotType, TArray<FIntPoint>& OutToItemSlotPoses, TArray<int32>& OutToItemCounts) const;
//	
//	int32 CanAddItem(int32 ItemTemplateID, EItemRarity ItemRarity, int32 ItemCount, TArray<FIntPoint>& OutToItemSlotPoses, TArray<int32>& OutToItemCounts) const;
//	bool CanRemoveItem(int32 ItemTemplateID, int32 ItemCount, TArray<FIntPoint>& OutToItemSlotPoses, TArray<int32>& OutToItemCounts) const;
//
//public:
//	UFUNCTION(BlueprintCallable, BlueprintAuthorityOnly)
//	int32 TryAddItemByRarity(TSubclassOf<URSItemTemplate> ItemTemplateClass, EItemRarity ItemRarity, int32 ItemCount);
//	
//	UFUNCTION(BlueprintCallable, BlueprintAuthorityOnly)
//	int32 TryAddItemByProbability(TSubclassOf<URSItemTemplate> ItemTemplateClass, int32 ItemCount, const TArray<FRSItemRarityProbability>& ItemProbabilities);
//	
//	UFUNCTION(BlueprintCallable, BlueprintAuthorityOnly)
//	bool TryRemoveItem(int32 ItemTemplateID, int32 ItemCount);
//
//private:
//	void AddItem_Unsafe(const FIntPoint& ItemSlotPos, URSItemInstance* ItemInstance, int32 ItemCount);
//	URSItemInstance* RemoveItem_Unsafe(const FIntPoint& ItemSlotPos, int32 ItemCount);
//	
//private:
//	void MarkSlotChecks(TArray<bool>& InSlotChecks, bool bIsUsing, const FIntPoint& ItemSlotPos, const FIntPoint& ItemSlotCount) const;
//	void MarkSlotChecks(bool bIsUsing, const FIntPoint& ItemSlotPos, const FIntPoint& ItemSlotCount);
//
//public:
//	bool IsEmpty(const TArray<bool>& InSlotChecks, const FIntPoint& ItemSlotPos, const FIntPoint& ItemSlotCount) const;
//	bool IsEmpty(const FIntPoint& ItemSlotPos, const FIntPoint& ItemSlotCount) const;
//	bool IsAllEmpty();
//	
//	URSItemInstance* GetItemInstance(const FIntPoint& ItemSlotPos) const;
//	int32 GetItemCount(const FIntPoint& ItemSlotPos) const;
//	
//	const TArray<FRSInventoryEntry>& GetAllEntries() const;
//	int32 GetTotalCountByID(int32 ItemTemplateID) const;
//	FIntPoint GetInventorySlotCount() const { return InventorySlotCount; }
//	TArray<bool>& GetSlotChecks() { return SlotChecks; }
//
//public:
//	FOnInventoryEntryChanged OnInventoryEntryChanged;
//	
//private:
//	friend class URSEquipmentManagerComponent;
//	friend class URSItemManagerComponent;
//	
//	UPROPERTY(Replicated)
//	FRSInventoryList InventoryList;
//	
//	UPROPERTY(Replicated)
//	TArray<bool> SlotChecks;
//	
//	FIntPoint InventorySlotCount = FIntPoint(10, 5);
//};
