#include "GAS/GA/RSGameplayAbility_Attack.h"
#include "Kismet/KismetSystemLibrary.h"
#include "Character/RSCharacter.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "Abilities/Tasks/AbilityTask_PlayMontageAndWait.h"

URSGameplayAbility_Attack::URSGameplayAbility_Attack()
{
	InstancingPolicy = EGameplayAbilityInstancingPolicy::InstancedPerActor;
}

void URSGameplayAbility_Attack::ActivateAbility(
	const FGameplayAbilitySpecHandle Handle,
	const FGameplayAbilityActorInfo* ActorInfo,
	const FGameplayAbilityActivationInfo ActivationInfo,
	const FGameplayEventData* TriggerEventData)
{
	Super::ActivateAbility(Handle, ActorInfo, ActivationInfo, TriggerEventData);

	ARSCharacter* AvatarCharacter = ActorInfo ? Cast<ARSCharacter>(ActorInfo->AvatarActor.Get()) : nullptr;
	if (!IsValid(AvatarCharacter))
	{
		EndAbility(Handle, ActorInfo, ActivationInfo, true, true);
		return;
	}

	UAnimMontage* AttackMontage = AvatarCharacter->GetAttackComboMontage();
	if (!IsValid(AttackMontage))
	{
		EndAbility(Handle, ActorInfo, ActivationInfo, true, true);
		return;
	}

	if (!CommitAbility(Handle, ActorInfo, ActivationInfo))
	{
		EndAbility(Handle, ActorInfo, ActivationInfo, true, true);
		return;
	}

	if (StaminaCostEffectClass)
	{
		const UGameplayEffect* GE = StaminaCostEffectClass->GetDefaultObject<UGameplayEffect>();
		ApplyGameplayEffectToOwner(Handle, ActorInfo, ActivationInfo, GE, 1);
	}

	if (UCharacterMovementComponent* MoveComp = AvatarCharacter->GetCharacterMovement())
	{
		MoveComp->SetMovementMode(EMovementMode::MOVE_None);
	}

	CurrentCombo = 1;
	const FName FirstSection = GetSectionNameForCombo(CurrentCombo);
	const float FirstRate = GetPlayRateForCombo(CurrentCombo);

	UAbilityTask_PlayMontageAndWait* PlayAttackTask =
		UAbilityTask_PlayMontageAndWait::CreatePlayMontageAndWaitProxy(
			this,
			TEXT("PlayAttack"),
			AttackMontage,
			FirstRate,          // 여기부터 1타 속도 적용
			FirstSection        // Attack01
		);

	ApplyMontagePlayRate(AttackMontage, FirstRate);

	if (!PlayAttackTask)
	{
		EndAbility(Handle, ActorInfo, ActivationInfo, true, true);
		return;
	}

	PlayAttackTask->OnCompleted.AddDynamic(this, &ThisClass::OnCompleted);
	PlayAttackTask->OnInterrupted.AddDynamic(this, &ThisClass::OnCanceled);
	PlayAttackTask->ReadyForActivation();

	StartComboTimer();
}


void URSGameplayAbility_Attack::OnCompleted()
{
	EndAbility(CurrentSpecHandle, CurrentActorInfo, CurrentActivationInfo, true, false);
}

void URSGameplayAbility_Attack::OnCanceled()
{
	EndAbility(CurrentSpecHandle, CurrentActorInfo, CurrentActivationInfo, true, false);
}

FName URSGameplayAbility_Attack::GetNextAnimMontageSection()
{
	CurrentCombo = FMath::Clamp(CurrentCombo + 1, 1, 3);
	FName NextSection = *FString::Printf(TEXT("%s%02d"), TEXT("Attack"), CurrentCombo);
	return NextSection;
}

void URSGameplayAbility_Attack::StartComboTimer()
{
	if (ComboTimerHandle.IsValid() == true)
	{
		ComboTimerHandle.Invalidate();
	}

	if (CurrentCombo <= 2)
	{
		GetWorld()->GetTimerManager().SetTimer(ComboTimerHandle, this, &ThisClass::CheckComboInput, 0.7f, false);
	}
}

void URSGameplayAbility_Attack::InputPressed(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo)
{
	Super::InputPressed(Handle, ActorInfo, ActivationInfo);

	IsNextComboInputPressed = true;
}

void URSGameplayAbility_Attack::CheckComboInput()
{
	ComboTimerHandle.Invalidate();

	if (!IsNextComboInputPressed)
	{
		return;
	}

	// 다음 콤보로 진행
	CurrentCombo = FMath::Clamp<uint8>(CurrentCombo + 1, 1, 3);

	ARSCharacter* AvatarCharacter = CurrentActorInfo ? Cast<ARSCharacter>(CurrentActorInfo->AvatarActor.Get()) : nullptr;
	if (!IsValid(AvatarCharacter))
	{
		return;
	}

	UAnimMontage* AttackMontage = AvatarCharacter->GetAttackComboMontage();
	if (!IsValid(AttackMontage))
	{
		return;
	}

	const FName NextSection = GetSectionNameForCombo(CurrentCombo);
	const float NextRate = GetPlayRateForCombo(CurrentCombo);

	// 섹션 점프
	MontageJumpToSection(NextSection);

	// 점프 후 PlayRate 갱신 (이게 핵심)
	ApplyMontagePlayRate(AttackMontage, NextRate);

	// 다음 입력 타이머
	StartComboTimer();

	IsNextComboInputPressed = false;
}


void URSGameplayAbility_Attack::EndAbility(
	const FGameplayAbilitySpecHandle Handle,
	const FGameplayAbilityActorInfo* ActorInfo,
	const FGameplayAbilityActivationInfo ActivationInfo,
	bool bReplicateEndAbility,
	bool bWasCancelled)
{
	ARSCharacter* AvatarCharacter = ActorInfo ? Cast<ARSCharacter>(ActorInfo->AvatarActor.Get()) : nullptr;
	if (IsValid(AvatarCharacter))
	{
		AvatarCharacter->GetCharacterMovement()->SetMovementMode(EMovementMode::MOVE_Walking);
	}

	if (ComboTimerHandle.IsValid())
	{
		ComboTimerHandle.Invalidate();
	}

	CurrentCombo = 0;
	IsNextComboInputPressed = false;

	Super::EndAbility(Handle, ActorInfo, ActivationInfo, bReplicateEndAbility, bWasCancelled);
}

float URSGameplayAbility_Attack::GetPlayRateForCombo(uint8 Combo) const
{
	switch (Combo)
	{
	case 1: return 1.10f;
	case 2: return 1.40f;
	case 3: return 1.55f;
	default: return 1.0f;
	}
}

FName URSGameplayAbility_Attack::GetSectionNameForCombo(uint8 Combo) const
{
	return *FString::Printf(TEXT("Attack%02d"), Combo); // Attack01, Attack02, Attack03
}

void URSGameplayAbility_Attack::ApplyMontagePlayRate(UAnimMontage* Montage, float PlayRate)
{
	if (!Montage) return;

	const FGameplayAbilityActorInfo* Info = CurrentActorInfo;
	if (!Info) return;

	UAnimInstance* AnimInst = Info->GetAnimInstance();
	if (!AnimInst) return;

	AnimInst->Montage_SetPlayRate(Montage, PlayRate);
}
