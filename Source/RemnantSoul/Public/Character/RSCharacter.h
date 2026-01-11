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
class URSAttributeSet_Stamina;
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
class ARSWeaponPickupActor;
class URSItemTemplate;

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
	UAnimMontage* GetSkillActionMontage() const { return SkillActionMontage; }

	UAnimMontage* GetAttackComboMontage() const { return Attack_ComboMontage; }
	UAnimMontage* GetAttackStaffMontage() const { return Attack_StaffMontage; }
	
	UAnimMontage* GetAttackSlashComboMontage() const { return Attack_SlashComboMontage; }
	UAnimMontage* GetAttackThrustChargeMontage() const { return Attack_ThrustChargeMontage; }
	UAnimMontage* GetAttackThrustReleaseMontage() const { return Attack_ThrustReleaseMontage; }

	UAnimMontage* GetSkillDefaultCastMontage() const { return Skill_DefaultCastMontage; }
	UAnimMontage* GetSkillProjectileMontage() const { return Skill_ProjectileMontage; }
	UAnimMontage* GetSkillMeteorMontage() const { return Skill_MeteorMontage; }
	UAnimMontage* GetSkillObstacleMontage() const { return Skill_ObstacleMontage; }


	void SetHeroData(const URSHeroData* InHeroData);

	// Roll
	void SetRollDirectionDegrees(float InDegrees) { RollDirectionDegrees = InDegrees; }
	float GetRollDirectionDegrees() const { return RollDirectionDegrees; }

	URSItemInstance* GetEquippedWeaponByInputTag(FGameplayTag InputTag) const;
	FGameplayTag ResolveWeaponSlotFromInputTag(FGameplayTag InputTag) const;

public:
	UFUNCTION(BlueprintCallable, Category = "RS|Pickup")
	bool PickupWeaponTemplate(URSItemTemplate* WeaponTemplate, int32 Count, bool bAutoEquip);

	bool PickupFromActor(ARSWeaponPickupActor* Pickup, bool bAutoEquip);
	

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
	
	bool TraceInteractTarget(AActor*& OutActor, FHitResult& OutHit) const;

	// IInventoryOwner interface implementation
	virtual bool TryAddItem_Implementation(URSItemData* ItemData, int32 Count) override;
	virtual bool TryRemoveItem_Implementation(URSItemData* ItemData, int32 Count) override;

	// Input
	virtual UInputComponent* CreatePlayerInputComponent() override;




private:
	bool HasValidPlayerHeroData() const;
	void DebugLogAbilityMatchesForInputTag(const FGameplayTag& InputTag) const;

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

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "ARSCharacter|Component")
	TObjectPtr<URSAttributeSet_Stamina> AttributeSet_Stamina;

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
	TObjectPtr<UAnimMontage> AttackComboMontage;

	UPROPERTY(EditDefaultsOnly, Category = "ARSCharacter|Skill")
	TObjectPtr<UAnimMontage> SkillActionMontage;

	UPROPERTY(EditDefaultsOnly, Category = "ARSCharacter|Skill")
	TSubclassOf<UGameplayAbility> SkillAbilityClass;

	// -------------------------
	// Attack / Skill (Ability tag -> Montage mapping)
	// -------------------------
	
	// Ability.Attack.Combo
	UPROPERTY(EditDefaultsOnly, Category = "RSCharacter|Montage|Attack")
	TObjectPtr<UAnimMontage> Attack_ComboMontage;

	UPROPERTY(EditDefaultsOnly, Category = "RSCharacter|Montage|Attack")
	TObjectPtr<UAnimMontage> Attack_StaffMontage;

	// Ability.Attack.Slash.Combo
	UPROPERTY(EditDefaultsOnly, Category = "RSCharacter|Montage|Attack")
	TObjectPtr<UAnimMontage> Attack_SlashComboMontage;

	// Ability.Attack.ThrustCharge
	// - 차지 시작~유지(루프) / 릴리즈를 분리하고 싶으면 2개로 쪼개는게 안정적
	UPROPERTY(EditDefaultsOnly, Category = "RSCharacter|Montage|Attack")
	TObjectPtr<UAnimMontage> Attack_ThrustChargeMontage;

	UPROPERTY(EditDefaultsOnly, Category = "RSCharacter|Montage|Attack")
	TObjectPtr<UAnimMontage> Attack_ThrustReleaseMontage;

	// Ability.Skill.*
	// 스킬은 보통 "공통 캐스팅(상체)" + "액션(발사/소환)" 2단이 많아서,
	// 캐스팅은 공통 1개 + 액션은 스킬별 1개를 추천
	UPROPERTY(EditDefaultsOnly, Category = "RSCharacter|Montage|Skill")
	TObjectPtr<UAnimMontage> Skill_DefaultCastMontage;

	// Ability.Skill.Projectile
	UPROPERTY(EditDefaultsOnly, Category = "RSCharacter|Montage|Skill")
	TObjectPtr<UAnimMontage> Skill_ProjectileMontage;

	// Ability.Skill.Meteor
	UPROPERTY(EditDefaultsOnly, Category = "RSCharacter|Montage|Skill")
	TObjectPtr<UAnimMontage> Skill_MeteorMontage;

	// Ability.Skill.Obstacle
	UPROPERTY(EditDefaultsOnly, Category = "RSCharacter|Montage|Skill")
	TObjectPtr<UAnimMontage> Skill_ObstacleMontage;


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
	//void InteractFocused();
	
	UPROPERTY(EditDefaultsOnly, Category = "ARSCharacter|Interaction")
	float InteractTraceDistance = 900.f;

	UPROPERTY(EditDefaultsOnly, Category = "ARSCharacter|Interaction")
	float InteractDistance = 900.f;

	UPROPERTY(EditDefaultsOnly, Category = "ARSCharacter|Interaction")
	float InteractTraceInterval = 0.1f;

	UPROPERTY(EditDefaultsOnly, Category="ARSCharacter|Interaction|Focus", meta=(ClampMin="1"))
	int32 FocusConfirmCount = 2;           

	UPROPERTY(EditDefaultsOnly, Category="ARSCharacter|Interaction|Focus", meta=(ClampMin="0.0"))
	float FocusLoseGraceTime = 0.15f;      

	TWeakObjectPtr<AActor> PendingFocusActor;
	int32 PendingFocusHits = 0;
	float LastFocusSeenTime = 0.f;
	
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
	float InteractCameraTraceRadius = 30.0f;
	void UpdateInteractFocus();

	// -------------------------
	// Roll Degrees
	// -------------------------
private:
	UPROPERTY(BlueprintReadOnly, meta = (AllowPrivateAccess = "true"))
	float RollDirectionDegrees = 0.f;
};
