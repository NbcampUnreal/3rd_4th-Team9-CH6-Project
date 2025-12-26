// RSCharacter.h

#pragma once

#include "GameFramework/Character.h"
#include "InputActionValue.h"
#include "AbilitySystemInterface.h"
#include  "Interface/InventoryOwner.h"
#include "Abilities/GameplayAbilityTypes.h"
#include "RSCharacter.generated.h"

class UCameraComponent;
class USpringArmComponent;
class UInputMappingContext;
class UInputAction;
class UAbilitySystemComponent;
class UGameplayAbility;
class URSAttributeSet_Character;
class URSWidgetComponent;
class URSAttributeSet_Skill;
class URSPawnData;
class URSInputConfig;
// Item 관련 매니저 컴포넌트들
class URSCosmeticManagerComponent;
class URSEquipManagerComponent;
class URSEquipmentManagerComponent;
class URSInventoryManagerComponent;



class URSItemData;
class URSInventoryComponent;
struct FTimerHandle;

UCLASS()
class REMNANTSOUL_API ARSCharacter : public ACharacter, public IAbilitySystemInterface, public IInventoryOwner
{
	GENERATED_BODY()

#pragma region ACharacter Override

public:
	ARSCharacter();

	virtual void SetupPlayerInputComponent(UInputComponent* PlayerInputComponent) override;

	virtual void BeginPlay() override;

public:
	const URSPawnData* GetPawnData() const { return PawnData; }

	const URSInputConfig* GetInputConfig() const;

protected:
	UFUNCTION()
	virtual void OnOutOfHealth();




#pragma endregion


#pragma region Component

public:
	FORCEINLINE USpringArmComponent* GetSpringArm() const { return SpringArm; }

	FORCEINLINE UCameraComponent* GetCamera() const { return Camera; }

	virtual UAbilitySystemComponent* GetAbilitySystemComponent() const override;

protected:
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "ARSCharacter|Component")
	TObjectPtr<USpringArmComponent> SpringArm;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "ARSCharacter|Component")
	TObjectPtr<UCameraComponent> Camera;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "ARSCharacter|Component")
	TObjectPtr<UAbilitySystemComponent> ASC;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "ARSCharacter|Component")
	TObjectPtr<URSWidgetComponent> HPBar;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "ARSCharacter|Component")
	TObjectPtr<USkeletalMeshComponent> Weapon;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "RS|Character")
	TObjectPtr<class URSHeroComponent> HeroComponent;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "RS|PawnData")
	TObjectPtr<const URSPawnData> PawnData;

	// Item 관련 Component들
		/** 인벤토리 매니저 (새 구조) */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "RS|Inventory")
	TObjectPtr<URSInventoryManagerComponent> InventoryManager;

	/** 논리 장비 매니저 (슬롯 → ItemInstance) */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "RS|Equipment")
	TObjectPtr<URSEquipmentManagerComponent> EquipmentManager;

	/** GAS 기반 장비 효과 매니저 (Ability/Attribute/Tag) */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "RS|Equipment")
	TObjectPtr<URSEquipManagerComponent> EquipManager;

	/** 코스메틱 매니저 (무기 액터/메시/소켓) */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "RS|Cosmetic")
	TObjectPtr<URSCosmeticManagerComponent> CosmeticManager;

#pragma endregion


#pragma region Input

private:
	
	
	

protected:
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "ARSCharacter|Input")
	TObjectPtr<UInputMappingContext> InputMappingContext;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "ARSCharacter|Input")
	TObjectPtr<UInputAction> MoveAction;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "ARSCharacter|Input")
	TObjectPtr<UInputAction> LookAction;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "ARSCharacter|Input")
	TObjectPtr<UInputAction> JumpAction;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "ARSCharacter|Input")
	TObjectPtr<UInputAction> SuperJumpAction;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "ARSCharacter|Input")
	TObjectPtr<UInputAction> AttackAction;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "ARSCharacter|Input")
	TObjectPtr<UInputAction> SkillAction;
	
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "ARSCharacter|Input")
	TObjectPtr<UInputAction> InteractAction;

#pragma endregion


#pragma region Ability

protected:
	UPROPERTY(EditDefaultsOnly, Category = "ARSCharacter|GameplayAbilitySystem")
	TArray<TSubclassOf<UGameplayAbility>> GrantedAbilities;

	UPROPERTY(EditDefaultsOnly, Category = "ARSCharacter|GameplayAbilitySystem")
	TMap<int32, TSubclassOf<UGameplayAbility>> GrantedInputAbilities;

	UPROPERTY(EditDefaultsOnly, Category = "ARSCharacter|GameplayAbilitySystem")
	TObjectPtr<URSAttributeSet_Character> AttributeSet;

	UPROPERTY(EditDefaultsOnly, Category = "ARSCharacter|GameplayAbilitySystem")
	TObjectPtr<URSAttributeSet_Skill> SkillAttributeSet;

#pragma endregion


#pragma region Attack

public:
	UPROPERTY(EditDefaultsOnly, Category = "ARSCharacter|Attack")
	TObjectPtr<UAnimMontage> AttackMontage;

#pragma endregion


#pragma region Equip

protected:
	void EquipWeapon(const FGameplayEventData* EventData);

	void UnequipWeapon(const FGameplayEventData* EventData);

protected:
	UPROPERTY(EditAnywhere)
	TObjectPtr<USkeletalMesh> WeaponMesh;

	UPROPERTY(EditAnywhere)
	float WeaponRange;

	UPROPERTY(EditAnywhere)
	float WeaponAttackDamage;

#pragma endregion


#pragma region Skill

public:
	UAnimMontage* GetSkillActionMontage() const { return SkillActionMontage; };

protected:
	UPROPERTY(EditDefaultsOnly)
	TObjectPtr<UAnimMontage> SkillActionMontage;

	UPROPERTY(EditDefaultsOnly)
	TSubclassOf<UGameplayAbility> SkillAbilityClass;

#pragma endregion

	//[추가]
#pragma region Interaction
public:
	void TryInteract();
private:
	//void HandleInteractInput(const FInputActionValue& InValue);
	void UpdateInteractFocus();
	
	bool TraceInteractTarget(AActor*& OutActor, FHitResult& OutHit) const;


	UPROPERTY(EditDefaultsOnly, Category="ARSCharacter|Interaction")
	float InteractTraceDistance = 500.0f;
	
	UPROPERTY(VisibleInstanceOnly, Category="ARSCharacter|Interaction")
	FHitResult CurrentInteractHit;

	UPROPERTY(EditDefaultsOnly, Category="ARSCharacter|Interaction")
	float InteractDistance = 500.f; // 실제 픽업 거리
	
	UPROPERTY(EditDefaultsOnly, Category="ARSCharacter|Interaction")
	float InteractTraceInterval = 0.1f;

	UPROPERTY(VisibleInstanceOnly, Category="ARSCharacter|Interaction")
	TObjectPtr<AActor> CurrentInteractTarget = nullptr;

	UPROPERTY(VisibleInstanceOnly, Category="ARSCharacter|Interaction")
	TObjectPtr<URSItemData> CurrentInteractItemData = nullptr;

	FTimerHandle InteractTraceTimer;

	
#pragma endregion
	
#pragma region Inventory
public:
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category="Inventory")
	TObjectPtr<URSInventoryComponent> Inventory;
	
	// 인터페이스 구현 선언
	virtual bool TryAddItem_Implementation(
		URSItemData* ItemData,
		int32 Count
	) override;

	virtual bool TryRemoveItem_Implementation(
		URSItemData* ItemData,
		int32 Count
	) override;
	
	
#pragma endregion
	
};
