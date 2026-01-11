//#include "GAS/GA/RSGameplayAbility_Skill.h"
//
//#include "Character/RSCharacter.h"
//#include "GameFramework/CharacterMovementComponent.h"
//#include "Abilities/Tasks/AbilityTask_PlayMontageAndWait.h"
//
//URSGameplayAbility_Skill::URSGameplayAbility_Skill()
//{
//}
//
//void URSGameplayAbility_Skill::ActivateAbility(
//    const FGameplayAbilitySpecHandle Handle,
//    const FGameplayAbilityActorInfo* ActorInfo,
//    const FGameplayAbilityActivationInfo ActivationInfo,
//    const FGameplayEventData* TriggerEventData)
//{
//    Super::ActivateAbility(Handle, ActorInfo, ActivationInfo, TriggerEventData);
//
//    ARSCharacter* AvatarCharacter = ActorInfo ? Cast<ARSCharacter>(ActorInfo->AvatarActor.Get()) : nullptr;
//    if (!IsValid(AvatarCharacter))
//    {
//        EndAbility(Handle, ActorInfo, ActivationInfo, true, true);
//        return;
//    }
//
//    ActiveSkillActionMontage = AvatarCharacter->GetSkillActionMontage();
//    if (!IsValid(ActiveSkillActionMontage))
//    {
//        EndAbility(Handle, ActorInfo, ActivationInfo, true, true);
//        return;
//    }
//
//    // Commit 규칙 통일
//    if (!CommitAbility(Handle, ActorInfo, ActivationInfo))
//    {
//        EndAbility(Handle, ActorInfo, ActivationInfo, true, true);
//        return;
//    }
//
//    AvatarCharacter->GetCharacterMovement()->SetMovementMode(MOVE_None);
//
//    UAbilityTask_PlayMontageAndWait* PlayMontageTask =
//        UAbilityTask_PlayMontageAndWait::CreatePlayMontageAndWaitProxy(
//            this, TEXT("SkillMontage"), ActiveSkillActionMontage, 1.0f);
//
//    if (!PlayMontageTask)
//    {
//        EndAbility(Handle, ActorInfo, ActivationInfo, true, true);
//        return;
//    }
//
//    PlayMontageTask->OnCompleted.AddDynamic(this, &ThisClass::OnCompleteCallback);
//    PlayMontageTask->OnInterrupted.AddDynamic(this, &ThisClass::OnInterruptedCallback);
//    PlayMontageTask->ReadyForActivation();
//}
//
//void URSGameplayAbility_Skill::EndAbility(
//    const FGameplayAbilitySpecHandle Handle,
//    const FGameplayAbilityActorInfo* ActorInfo,
//    const FGameplayAbilityActivationInfo ActivationInfo,
//    bool bReplicateEndAbility,
//    bool bWasCancelled)
//{
//    ARSCharacter* AvatarCharacter = ActorInfo ? Cast<ARSCharacter>(ActorInfo->AvatarActor.Get()) : nullptr;
//    if (IsValid(AvatarCharacter))
//    {
//        AvatarCharacter->GetCharacterMovement()->SetMovementMode(MOVE_Walking);
//    }
//
//    Super::EndAbility(Handle, ActorInfo, ActivationInfo, bReplicateEndAbility, bWasCancelled);
//}
//
//
//void URSGameplayAbility_Skill::OnCompleteCallback()
//{
//	EndAbility(CurrentSpecHandle, CurrentActorInfo, CurrentActivationInfo, true, false);
//}
//
//void URSGameplayAbility_Skill::OnInterruptedCallback()
//{
//	EndAbility(CurrentSpecHandle, CurrentActorInfo, CurrentActivationInfo, true, true);
//}



// ===== RSGameplayAbility_Skill.cpp =====
#include "GAS/GA/RSGameplayAbility_Skill.h"

#include "Character/RSCharacter.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "Abilities/Tasks/AbilityTask_PlayMontageAndWait.h"

#include "NiagaraFunctionLibrary.h"
#include "NiagaraComponent.h"
#include "NiagaraSystem.h"
#include "GameFramework/Actor.h"
#include "Components/SkeletalMeshComponent.h"

URSGameplayAbility_Skill::URSGameplayAbility_Skill()
{
}

void URSGameplayAbility_Skill::ActivateAbility(
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

	ActiveSkillActionMontage = AvatarCharacter->GetSkillActionMontage();
	if (!IsValid(ActiveSkillActionMontage))
	{
		EndAbility(Handle, ActorInfo, ActivationInfo, true, true);
		return;
	}

	// Commit 규칙 통일
	if (!CommitAbility(Handle, ActorInfo, ActivationInfo))
	{
		EndAbility(Handle, ActorInfo, ActivationInfo, true, true);
		return;
	}

	// VFX는 "실패해도 스킬을 망치면 안 됨" (Commit 이후 안전하게)
	SpawnSkillVfx(ActorInfo);

	AvatarCharacter->GetCharacterMovement()->SetMovementMode(MOVE_None);

	UAbilityTask_PlayMontageAndWait* PlayMontageTask =
		UAbilityTask_PlayMontageAndWait::CreatePlayMontageAndWaitProxy(
			this, TEXT("SkillMontage"), ActiveSkillActionMontage, 1.0f);

	if (!PlayMontageTask)
	{
		EndAbility(Handle, ActorInfo, ActivationInfo, true, true);
		return;
	}

	PlayMontageTask->OnCompleted.AddDynamic(this, &ThisClass::OnCompleteCallback);
	PlayMontageTask->OnInterrupted.AddDynamic(this, &ThisClass::OnInterruptedCallback);
	PlayMontageTask->ReadyForActivation();
}

void URSGameplayAbility_Skill::EndAbility(
	const FGameplayAbilitySpecHandle Handle,
	const FGameplayAbilityActorInfo* ActorInfo,
	const FGameplayAbilityActivationInfo ActivationInfo,
	bool bReplicateEndAbility,
	bool bWasCancelled)
{
	// VFX 정리 (EndAbility 경로는 기존과 동일하게 유지)
	CleanupSkillVfx();

	ARSCharacter* AvatarCharacter = ActorInfo ? Cast<ARSCharacter>(ActorInfo->AvatarActor.Get()) : nullptr;
	if (IsValid(AvatarCharacter))
	{
		AvatarCharacter->GetCharacterMovement()->SetMovementMode(MOVE_Walking);
	}

	Super::EndAbility(Handle, ActorInfo, ActivationInfo, bReplicateEndAbility, bWasCancelled);
}

void URSGameplayAbility_Skill::OnCompleteCallback()
{
	EndAbility(CurrentSpecHandle, CurrentActorInfo, CurrentActivationInfo, true, false);
}

void URSGameplayAbility_Skill::OnInterruptedCallback()
{
	EndAbility(CurrentSpecHandle, CurrentActorInfo, CurrentActivationInfo, true, true);
}

void URSGameplayAbility_Skill::SpawnSkillVfx(const FGameplayAbilityActorInfo* ActorInfo)
{
	CleanupSkillVfx();

	if (!SkillVfx || !ActorInfo || !ActorInfo->AvatarActor.IsValid())
	{
		return;
	}

	AActor* Avatar = ActorInfo->AvatarActor.Get();
	if (!Avatar)
	{
		return;
	}

	switch (VfxSpawnPolicy)
	{
	case ERSSkillVfxSpawnPolicy::None:
		return;

	case ERSSkillVfxSpawnPolicy::SpawnAtActor:
	{
		const FVector Loc = Avatar->GetActorLocation() + VfxWorldOffset;
		const FRotator Rot = Avatar->GetActorRotation();

		SpawnedVfxComponent = UNiagaraFunctionLibrary::SpawnSystemAtLocation(
			Avatar->GetWorld(),
			SkillVfx,
			Loc,
			Rot,
			FVector(1.f),
			true,
			true,
			ENCPoolMethod::None,
			true
		);
	}
	break;

	case ERSSkillVfxSpawnPolicy::AttachToMeshSocket:
	{
		USkeletalMeshComponent* MeshComp = nullptr;

		if (const ARSCharacter* RSChar = Cast<ARSCharacter>(Avatar))
		{
			MeshComp = RSChar->GetMesh();
		}
		else
		{
			MeshComp = Avatar->FindComponentByClass<USkeletalMeshComponent>();
		}

		if (!MeshComp)
		{
			return;
		}

		SpawnedVfxComponent = UNiagaraFunctionLibrary::SpawnSystemAttached(
			SkillVfx,
			MeshComp,
			VfxAttachSocketName,
			FVector::ZeroVector,
			FRotator::ZeroRotator,
			EAttachLocation::SnapToTarget,
			true,
			true,
			ENCPoolMethod::None,
			true
		);
	}
	break;

	default:
		break;
	}
}

void URSGameplayAbility_Skill::CleanupSkillVfx()
{
	if (SpawnedVfxComponent)
	{
		SpawnedVfxComponent->Deactivate();
		SpawnedVfxComponent->DestroyComponent();
		SpawnedVfxComponent = nullptr;
	}
}
