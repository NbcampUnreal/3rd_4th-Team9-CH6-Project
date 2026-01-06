// Public/Character/RSCharacter.h
#pragma once

#include "GameFramework/Character.h"
#include "AbilitySystemInterface.h"
#include "Interface/InventoryOwner.h"
#include "GameplayTagContainer.h"
#include "Animation/AnimNotify/RSAnimEquipAction.h"
#include "Abilities/GameplayAbilityTypes.h"   // FGameplayEventData
#include "RSCharacter.generated.h"

class AController;
class UAbilitySystemComponent;
class URSHeroComponent;
class URSHeroData;
class URSPawnData;
class URSInputConfig;
class URSCombatStyleData;

class URSAttributeSet_Character;
class URSAttributeSet_Skill;

class URSWidgetComponent;
class URSInventoryComponent;
class URSInventoryManagerComponent;
class URSEquipmentManagerComponent;
class URSEquipManagerComponent;
class URSCosmeticManagerComponent;
enum class ERSAnimEquipAction : uint8;

class URSItemData;
class URSItemInstance;

class UAnimMontage;
class USpringArmComponent;
class UCameraComponent;
class USkeletalMesh;
class USkeletalMeshComponent;
class UGameplayAbility;

struct FTimerHandle;
struct FRSAbilitySet_GrantedHandles;


UCLASS()
class REMNANTSOUL_API ARSCharacter
	: public ACharacter
	, public IAbilitySystemInterface
	, public IInventoryOwner
{
	GENERATED_BODY()

public:
	ARSCharacter();

	// AActor / APawn / ACharacter
	virtual void SetupPlayerInputComponent(UInputComponent* PlayerInputComponent) override;
	virtual void BeginPlay() override;

	// AbilitySystemInterface
	virtual UAbilitySystemComponent* GetAbilitySystemComponent() const override;

	// Input -> GAS (HeroComponent에서 호출)
	void AbilityInputTagPressed(const FGameplayTag& InputTag);
	void AbilityInputTagReleased(const FGameplayTag& InputTag);

	// Interaction
	void TryInteract();

	// CombatStyle -> Anim
	UFUNCTION(BlueprintCallable, Category = "RS|CombatStyle")
	void OnCombatStyleChanged(const URSCombatStyleData* NewStyle);

	// SSOT Accessors
	bool IsPlayerControlledPawn() const;
	const URSPawnData* GetPawnData() const;
	const URSInputConfig* GetInputConfig() const;
	const URSHeroData* GetHeroData() const { return HeroData; }

	// AnimNotify 관련 : 무기장착 AnimNotify에서 호출
	UFUNCTION(BlueprintCallable, Category = "RS|Equip")
	void HandleAnimEquipAction(ERSAnimEquipAction Action);


public:
	// Attack
	UAnimMontage* GetAttackMontage() const { return AttackMontage; }

	// Skill
	UAnimMontage* GetSkillActionMontage() const { return SkillActionMontage; }

	void SetHeroData(const URSHeroData* InHeroData);

	// Roll
	void SetRollDirectionDegrees(float InDegrees) { RollDirectionDegrees = InDegrees; }
	float GetRollDirectionDegrees() const { return RollDirectionDegrees; }

	URSItemInstance* GetEquippedWeaponByInputTag(FGameplayTag InputTag) const;
	FGameplayTag ResolveWeaponSlotFromInputTag(FGameplayTag InputTag) const;


protected:
	// replication/possession
	virtual void PossessedBy(AController* NewController) override;
	virtual void OnRep_PlayerState() override;

	void InitializeAbilitySystemAndPawnData();

	UFUNCTION()
	virtual void OnOutOfHealth();

	// Equip callbacks (GameplayEvent)
	void EquipWeapon(const FGameplayEventData* EventData);
	void UnequipWeapon(const FGameplayEventData* EventData);

	// Interaction helpers
	void UpdateInteractFocus();
	bool TraceInteractTarget(AActor*& OutActor, FHitResult& OutHit) const;

	// IInventoryOwner interface implementation
	virtual bool TryAddItem_Implementation(URSItemData* ItemData, int32 Count) override;
	virtual bool TryRemoveItem_Implementation(URSItemData* ItemData, int32 Count) override;

	// Input
	virtual UInputComponent* CreatePlayerInputComponent() override;

private:
	bool HasValidPlayerHeroData() const;

protected:
	// Components
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "ARSCharacter|Component")
	TObjectPtr<USpringArmComponent> SpringArm;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "ARSCharacter|Component")
	TObjectPtr<UCameraComponent> Camera;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "ARSCharacter|Component")
	TObjectPtr<UAbilitySystemComponent> ASC;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "RS|Character")
	TObjectPtr<URSHeroComponent> HeroComponent;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "ARSCharacter|Component")
	TObjectPtr<URSAttributeSet_Character> AttributeSet;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "ARSCharacter|Component")
	TObjectPtr<URSAttributeSet_Skill> SkillAttributeSet;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "ARSCharacter|Component")
	TObjectPtr<URSWidgetComponent> HPBar;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "ARSCharacter|Component")
	TObjectPtr<USkeletalMeshComponent> Weapon;

	// Item managers
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "RS|Item", meta = (AllowPrivateAccess = "true"))
	TObjectPtr<class URSInventoryManagerComponent> InventoryManager;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "RS|Item", meta = (AllowPrivateAccess = "true"))
	TObjectPtr<class URSEquipmentManagerComponent> EquipmentManager;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "RS|Item", meta = (AllowPrivateAccess = "true"))
	TObjectPtr<class URSEquipManagerComponent> EquipManager;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "RS|Item", meta = (AllowPrivateAccess = "true"))
	TObjectPtr<class URSCosmeticManagerComponent> CosmeticManager;

	// Inventory (interface target)
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Inventory")
	TObjectPtr<URSInventoryComponent> Inventory;

	// -------------------------
	// Data (SSOT)
	// -------------------------
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "RS|Hero")
	TObjectPtr<URSHeroData> HeroData = nullptr; // Player SSOT: HeroData->PawnData

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "RS|PawnData")
	TObjectPtr<URSPawnData> DebugPawnDataFallback = nullptr; // AI/테스트용(선택)

	// AbilitySet handles (중복 부여 방지용)
	UPROPERTY(Transient)
	TArray<FRSAbilitySet_GrantedHandles> PawnGrantedAbilitySetHandles;

	// -------------------------
	// Attack / Skill
	// -------------------------
	UPROPERTY(EditDefaultsOnly, Category = "ARSCharacter|Attack")
	TObjectPtr<UAnimMontage> AttackMontage;

	UPROPERTY(EditDefaultsOnly, Category = "ARSCharacter|Skill")
	TObjectPtr<UAnimMontage> SkillActionMontage;

	UPROPERTY(EditDefaultsOnly, Category = "ARSCharacter|Skill")
	TSubclassOf<UGameplayAbility> SkillAbilityClass;

	// -------------------------
	// Equip / Weapon Stats
	// -------------------------
	UPROPERTY(EditAnywhere, Category = "ARSCharacter|Equip")
	TObjectPtr<USkeletalMesh> WeaponMesh;

	UPROPERTY(EditAnywhere, Category = "ARSCharacter|Equip")
	float WeaponRange = 0.f;

	UPROPERTY(EditAnywhere, Category = "ARSCharacter|Equip")
	float WeaponAttackDamage = 0.f;

	// -------------------------
	// Animation state
	// -------------------------
	UPROPERTY(Transient, BlueprintReadOnly, Category = "RS|CombatStyle")
	FGameplayTag CurrentAnimLayerTag;

	UPROPERTY(Transient, BlueprintReadOnly, Category = "RS|CombatStyle")
	FGameplayTagContainer CurrentAnimStyleTags;

	// -------------------------
	// Interaction state
	// -------------------------
	UPROPERTY(EditDefaultsOnly, Category = "ARSCharacter|Interaction")
	float InteractTraceDistance = 900.0f;

	UPROPERTY(EditDefaultsOnly, Category = "ARSCharacter|Interaction")
	float InteractDistance = 900.f;

	UPROPERTY(EditDefaultsOnly, Category = "ARSCharacter|Interaction")
	float InteractTraceInterval = 0.1f;

	UPROPERTY(VisibleInstanceOnly, Category = "ARSCharacter|Interaction")
	FHitResult CurrentInteractHit;

	UPROPERTY(VisibleInstanceOnly, Category = "ARSCharacter|Interaction")
	TObjectPtr<AActor> CurrentInteractTarget = nullptr;

	UPROPERTY(VisibleInstanceOnly, Category = "ARSCharacter|Interaction")
	TObjectPtr<URSItemData> CurrentInteractItemData = nullptr;

	FTimerHandle InteractTraceTimer;
	
	UPROPERTY(EditDefaultsOnly, Category = "ARSCharacter|Interaction", meta=(ClampMin="0.0"))
	float InteractTraceRadius = 30.0f;
	
	UPROPERTY(EditDefaultsOnly, Category = "ARSCharacter|Interaction", meta=(ClampMin="0.0"))
	float InteractCameraTraceRadius = 8.0f;

	

	// -------------------------
	// Roll Degrees
	// -------------------------
private:
	UPROPERTY(BlueprintReadOnly, meta = (AllowPrivateAccess = "true"))
	float RollDirectionDegrees = 0.f;
};
