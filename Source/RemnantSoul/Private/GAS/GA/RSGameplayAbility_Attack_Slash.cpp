// Fill out your copyright notice in the Description page of Project Settings.


#include "GAS/GA/RSGameplayAbility_Attack_Slash.h"
#include "RemnantSoul.h"
#include "RSGameplayTags.h"
#include "Kismet/KismetSystemLibrary.h"
#include "Character/RSCharacter.h"
#include "AbilitySystemBlueprintLibrary.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "Abilities/Tasks/AbilityTask_PlayMontageAndWait.h"

URSGameplayAbility_Attack_Slash::URSGameplayAbility_Attack_Slash()
{
	InstancingPolicy = EGameplayAbilityInstancingPolicy::InstancedPerActor;
}

void URSGameplayAbility_Attack_Slash::ActivateAbility(
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

	UAnimMontage* AttackMontage = AvatarCharacter->GetAttackSlashComboMontage();
	if (!IsValid(AttackMontage))
	{
		EndAbility(Handle, ActorInfo, ActivationInfo, true, true);
		return;
	}

	CachedAttackMontage = AttackMontage;

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

	IsNextComboInputPressed = false;

	SendCheckHitEvent(1);

	StartComboTimer();
}


void URSGameplayAbility_Attack_Slash::OnCompleted()
{
	EndAbility(CurrentSpecHandle, CurrentActorInfo, CurrentActivationInfo, true, false);
}

void URSGameplayAbility_Attack_Slash::OnCanceled()
{
	EndAbility(CurrentSpecHandle, CurrentActorInfo, CurrentActivationInfo, true, false);
}

FName URSGameplayAbility_Attack_Slash::GetNextAnimMontageSection()
{
	CurrentCombo = FMath::Clamp(CurrentCombo + 1, 1, 3);
	FName NextSection = *FString::Printf(TEXT("%s%02d"), TEXT("Attack"), CurrentCombo);
	return NextSection;
}

//void URSGameplayAbility_Attack_Slash::StartComboTimer()
//{
//	if (ComboTimerHandle.IsValid() == true)
//	{
//		ComboTimerHandle.Invalidate();
//	}
//
//	if (CurrentCombo <= 2)
//	{
//		GetWorld()->GetTimerManager().SetTimer(ComboTimerHandle, this, &ThisClass::CheckComboInput, 0.7f, false);
//	}
//}

void URSGameplayAbility_Attack_Slash::StartComboTimer()
{
	if (ComboTimerHandle.IsValid())
	{
		GetWorld()->GetTimerManager().ClearTimer(ComboTimerHandle);
		ComboTimerHandle.Invalidate();
	}

	if (CurrentCombo >= 3)
	{
		return;
	}

	if (!IsValid(CachedAttackMontage))
	{
		// 안전장치: 캐시가 없으면 기존 값으로라도 동작
		GetWorld()->GetTimerManager().SetTimer(ComboTimerHandle, this, &ThisClass::CheckComboInput, 0.35f, false);
		return;
	}

	const FName SectionName = GetSectionNameForCombo(CurrentCombo);
	const int32 SectionIndex = CachedAttackMontage->GetSectionIndex(SectionName);
	if (SectionIndex == INDEX_NONE)
	{
		// 섹션 이름이 다르면 여기서부터 콤보가 절대 안 된다.
		UE_LOG(LogRemnantSoul, Error, TEXT("[Slash] Montage section not found: %s"), *SectionName.ToString());
		GetWorld()->GetTimerManager().SetTimer(ComboTimerHandle, this, &ThisClass::CheckComboInput, 0.35f, false);
		return;
	}

	const float SectionLength = CachedAttackMontage->GetSectionLength(SectionIndex);

	// 현재 콤보의 재생 속도 반영
	const float CurrentRate = GetPlayRateForCombo(CurrentCombo);
	const float EffectiveLength = (CurrentRate > 0.f) ? (SectionLength / CurrentRate) : SectionLength;

	// 콤보 입력 윈도우: 섹션 체감 길이에 비례
	const float ComboWindowTime = FMath::Clamp(EffectiveLength * 0.90f, 0.20f, 1.10f);

	GetWorld()->GetTimerManager().SetTimer(
		ComboTimerHandle, this, &ThisClass::CheckComboInput, ComboWindowTime, false);
}


void URSGameplayAbility_Attack_Slash::InputPressed(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo)
{
	Super::InputPressed(Handle, ActorInfo, ActivationInfo);

	IsNextComboInputPressed = true;
}

void URSGameplayAbility_Attack_Slash::CheckComboInput()
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

	UAnimMontage* AttackMontage = AvatarCharacter->GetAttackSlashComboMontage();
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

	SendCheckHitEvent(CurrentCombo);

	// 다음 입력 타이머
	StartComboTimer();

	IsNextComboInputPressed = false;
}


void URSGameplayAbility_Attack_Slash::EndAbility(
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

float URSGameplayAbility_Attack_Slash::GetPlayRateForCombo(uint8 Combo) const
{
	switch (Combo)
	{
	case 1: return 0.90f;
	case 2: return 1.00f;
	case 3: return 1.10f;
	default: return 1.0f;
	}
}

FName URSGameplayAbility_Attack_Slash::GetSectionNameForCombo(uint8 Combo) const
{
	return *FString::Printf(TEXT("Attack%02d"), Combo); // Attack01, Attack02, Attack03
}

void URSGameplayAbility_Attack_Slash::ApplyMontagePlayRate(UAnimMontage* Montage, float PlayRate)
{
	if (!Montage) return;

	const FGameplayAbilityActorInfo* Info = CurrentActorInfo;
	if (!Info) return;

	UAnimInstance* AnimInst = Info->GetAnimInstance();
	if (!AnimInst) return;

	AnimInst->Montage_SetPlayRate(Montage, PlayRate);
}

void URSGameplayAbility_Attack_Slash::SendCheckHitEvent(int32 ComboIndex) const
{
	AActor* Avatar = GetAvatarActorFromActorInfo();
	if (!IsValid(Avatar))
	{
		return;
	}

	const FRSGameplayTags& Tags = FRSGameplayTags::Get();

	FGameplayEventData Data;
	Data.EventTag = Tags.Event_Attack_CheckHit;   // 여기만 변경
	Data.EventMagnitude = (float)ComboIndex;

	UAbilitySystemBlueprintLibrary::SendGameplayEventToActor(Avatar, Data.EventTag, Data);
}
