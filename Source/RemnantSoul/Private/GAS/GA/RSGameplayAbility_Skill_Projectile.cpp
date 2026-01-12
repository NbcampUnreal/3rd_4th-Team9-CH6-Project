#include "GAS/GA/RSGameplayAbility_Skill_Projectile.h"

#include "Character/RSCharacter.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "Abilities/Tasks/AbilityTask_PlayMontageAndWait.h"

URSGameplayAbility_Skill_Projectile::URSGameplayAbility_Skill_Projectile()
{
}

//void URSGameplayAbility_Skill_Projectile::ActivateAbility(
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
//void URSGameplayAbility_Skill_Projectile::EndAbility(
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
//void URSGameplayAbility_Skill_Projectile::OnCompleteCallback()
//{
//	EndAbility(CurrentSpecHandle, CurrentActorInfo, CurrentActivationInfo, true, false);
//}
//
//void URSGameplayAbility_Skill_Projectile::OnInterruptedCallback()
//{
//	EndAbility(CurrentSpecHandle, CurrentActorInfo, CurrentActivationInfo, true, true);
//}
