

#include "GAS/GA/RSGameplayAbility_CheckHit.h"
#include "Kismet/KismetSystemLibrary.h"
#include "AbilitySystemBlueprintLibrary.h"
#include "RemnantSoul.h"
#include "GAS/AT/RSAbilityTask_SweepSingleCapsule.h"
#include "GAS/TA/RSTargetActor_SweepSingleCapsule.h"
#include "GAS/AS/RSAttributeSet_Character.h"
#include "RSGameplayTags.h"

URSGameplayAbility_CheckHit::URSGameplayAbility_CheckHit()
{
	InstancingPolicy = EGameplayAbilityInstancingPolicy::InstancedPerActor;
}

void URSGameplayAbility_CheckHit::ActivateAbility(
    const FGameplayAbilitySpecHandle Handle,
    const FGameplayAbilityActorInfo* ActorInfo,
    const FGameplayAbilityActivationInfo ActivationInfo,
    const FGameplayEventData* TriggerEventData)
{
    Super::ActivateAbility(Handle, ActorInfo, ActivationInfo, TriggerEventData);

    // [0] ActorInfo/ASC 기본 가드
    if (!ActorInfo || !ActorInfo->AvatarActor.IsValid())
    {
        EndAbility(Handle, ActorInfo, ActivationInfo, true, true);
        return;
    }

    // [1] TriggerEventData 가드 (EventMagnitude 사용)
    if (!TriggerEventData)
    {
        EndAbility(Handle, ActorInfo, ActivationInfo, true, true);
        return;
    }

    CurrentLevel = TriggerEventData->EventMagnitude;

    // [2] Commit 규칙 통일 (비용/쿨타임/태그 적용 실패 시 종료)
    if (!CommitAbility(Handle, ActorInfo, ActivationInfo))
    {
        EndAbility(Handle, ActorInfo, ActivationInfo, true, true);
        return;
    }

    URSAbilityTask_SweepSingleCapsule* AttackTraceTask =
        URSAbilityTask_SweepSingleCapsule::CreateTask(this, TargetActorClass);

    if (!AttackTraceTask)
    {
        EndAbility(Handle, ActorInfo, ActivationInfo, true, true);
        return;
    }

    AttackTraceTask->OnComplete.AddDynamic(this, &ThisClass::OnSweepSingleCapsuleResultReady);
    AttackTraceTask->ReadyForActivation();
}

void URSGameplayAbility_CheckHit::OnSweepSingleCapsuleResultReady(
    const FGameplayAbilityTargetDataHandle& TargetDataHandle)
{
    // ---------- [0] Ability/ActorInfo 생존성 방어 ----------
    if (CurrentActorInfo == nullptr)
    {
        return;
    }

    UAbilitySystemComponent* SourceASC = GetAbilitySystemComponentFromActorInfo();
    if (!IsValid(SourceASC))
    {
        EndAbility(CurrentSpecHandle, CurrentActorInfo, CurrentActivationInfo, true, true);
        return;
    }

    const int32 Index = 0;

    if (TargetDataHandle.Data.Num() <= Index || TargetDataHandle.Data[Index].Get() == nullptr)
    {
        EndAbility(CurrentSpecHandle, CurrentActorInfo, CurrentActivationInfo, true, false);
        return;
    }

    const FRSGameplayTags& RS_Tags = FRSGameplayTags::Get();

    // -------------------------
    // 1) HitResult 경로
    // -------------------------
    if (UAbilitySystemBlueprintLibrary::TargetDataHasHitResult(TargetDataHandle, Index))
    {
        const FHitResult HitResult =
            UAbilitySystemBlueprintLibrary::GetHitResultFromTargetData(TargetDataHandle, Index);

        AActor* HitActor = HitResult.GetActor();
        if (!IsValid(HitActor))
        {
            EndAbility(CurrentSpecHandle, CurrentActorInfo, CurrentActivationInfo, true, false);
            return;
        }

        UAbilitySystemComponent* TargetASC =
            UAbilitySystemBlueprintLibrary::GetAbilitySystemComponent(HitActor);

        FGameplayEffectSpecHandle EffectSpecHandle =
            MakeOutgoingGameplayEffectSpec(AttackDamageEffect, CurrentLevel);

        if (EffectSpecHandle.IsValid())
        {
            // 핵심: ExecCalc가 읽을 BaseDamage + 결정태그를 Spec에 주입
            SetupDamageSpec(EffectSpecHandle);

            ApplyGameplayEffectSpecToTarget(
                CurrentSpecHandle, CurrentActorInfo, CurrentActivationInfo,
                EffectSpecHandle, TargetDataHandle);

            FGameplayEffectContextHandle CueContextHandle =
                UAbilitySystemBlueprintLibrary::GetEffectContext(EffectSpecHandle);
            CueContextHandle.AddHitResult(HitResult);

            FGameplayCueParameters CueParam;
            CueParam.EffectContext = CueContextHandle;

            // Target에게 Cue (ASC 있는 경우만)
            if (IsValid(TargetASC))
            {
                TargetASC->ExecuteGameplayCue(RS_Tags.GameplayCue_Attack_Hit, CueParam);
            }

            // Source에게도 Cue (원하면 유지)
            SourceASC->ExecuteGameplayCue(RS_Tags.GameplayCue_Attack_Hit, CueParam);
        }

        // 버프는 Source에게
        FGameplayEffectSpecHandle BuffEffectSpecHandle = MakeOutgoingGameplayEffectSpec(AttackBuffEffect);
        if (BuffEffectSpecHandle.IsValid())
        {
            ApplyGameplayEffectSpecToOwner(
                CurrentSpecHandle, CurrentActorInfo, CurrentActivationInfo,
                BuffEffectSpecHandle);
        }

        EndAbility(CurrentSpecHandle, CurrentActorInfo, CurrentActivationInfo, true, false);
        return;
    }

    // -------------------------
    // 2) ActorArray 경로 (HitResult 없음)
    // -------------------------
    if (UAbilitySystemBlueprintLibrary::TargetDataHasActor(TargetDataHandle, Index))
    {
        const TArray<TWeakObjectPtr<AActor>> Actors =
            TargetDataHandle.Data[Index].Get()->GetActors();

        bool bHasValidActor = false;
        for (const TWeakObjectPtr<AActor>& A : Actors)
        {
            if (A.IsValid())
            {
                bHasValidActor = true;
                break;
            }
        }

        if (!bHasValidActor)
        {
            EndAbility(CurrentSpecHandle, CurrentActorInfo, CurrentActivationInfo, true, false);
            return;
        }

        FGameplayEffectSpecHandle EffectSpecHandle =
            MakeOutgoingGameplayEffectSpec(AttackDamageEffect, CurrentLevel);

        if (EffectSpecHandle.IsValid())
        {
            // 핵심: ExecCalc가 읽을 BaseDamage + 결정태그를 Spec에 주입
            SetupDamageSpec(EffectSpecHandle);

            ApplyGameplayEffectSpecToTarget(
                CurrentSpecHandle, CurrentActorInfo, CurrentActivationInfo,
                EffectSpecHandle, TargetDataHandle);

            // Cue 파라미터(ActorArray 기반)
            FGameplayEffectContextHandle CueContextHandle =
                UAbilitySystemBlueprintLibrary::GetEffectContext(EffectSpecHandle);
            CueContextHandle.AddActors(Actors, false);

            FGameplayCueParameters CueParam;
            CueParam.EffectContext = CueContextHandle;

            // 멀티 타겟: 각 타겟 ASC에 Cue (있는 경우만)
            for (const TWeakObjectPtr<AActor>& A : Actors)
            {
                AActor* TargetActor = A.Get();
                if (!IsValid(TargetActor))
                {
                    continue;
                }

                UAbilitySystemComponent* TargetASC_Local =
                    UAbilitySystemBlueprintLibrary::GetAbilitySystemComponent(TargetActor);

                if (IsValid(TargetASC_Local))
                {
                    TargetASC_Local->ExecuteGameplayCue(RS_Tags.GameplayCue_Attack_Hit, CueParam);
                }
            }

            // Source Cue
            SourceASC->ExecuteGameplayCue(RS_Tags.GameplayCue_Attack_Hit, CueParam);
        }

        EndAbility(CurrentSpecHandle, CurrentActorInfo, CurrentActivationInfo, true, false);
        return;
    }

    EndAbility(CurrentSpecHandle, CurrentActorInfo, CurrentActivationInfo, true, false);
}

void URSGameplayAbility_CheckHit::SetupDamageSpec(FGameplayEffectSpecHandle& SpecHandle) const
{
    if (!SpecHandle.IsValid() || SpecHandle.Data == nullptr)
    {
        return;
    }

    const FRSGameplayTags& Tags = FRSGameplayTags::Get();

    // (1) GA는 "BaseDamage"만 넣는다.
    // 지금은 예시 상수. 다음 단계에서 무기/스탯에서 가져오면 됨.
    const float BaseDamage = 30.0f;
    SpecHandle.Data->SetSetByCallerMagnitude(Tags.Data_Damage, BaseDamage);

    // (2) 배율 결정을 위한 "결정 태그"는 Spec에만 부여한다. (ExecCalc가 읽음)
    // - 라이트/헤비: 입력/공격 종류에 따라 GA가 결정
    // - 소스: 근접/스킬 등
    // - 무기 타입: 현재 장비 기반이 이상적. 지금은 예시
    SpecHandle.Data->DynamicGrantedTags.AddTag(Tags.Damage_Source_Melee);
    SpecHandle.Data->DynamicGrantedTags.AddTag(Tags.Damage_Type_Light);

    // 무기 타입 예시 (나중에 장비에서 읽어와서 넣으면 됨)
    SpecHandle.Data->DynamicGrantedTags.AddTag(Tags.Weapon_Sword_OneHand);
}
