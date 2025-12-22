

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

void URSGameplayAbility_CheckHit::ActivateAbility(const FGameplayAbilitySpecHandle Handle,
	const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo,
	const FGameplayEventData* TriggerEventData)
{
	Super::ActivateAbility(Handle, ActorInfo, ActivationInfo, TriggerEventData);

	CurrentLevel = TriggerEventData->EventMagnitude;

	//UKismetSystemLibrary::PrintString(this, FString::Printf(TEXT("URSGameplayAbility_CheckHit::ActivateAbility()")));

	//EndAbility(CurrentSpecHandle, CurrentActorInfo, CurrentActivationInfo, true, false);

	//URSAbilityTask_SweepSingleCapsule* AttackTraceTask = URSAbilityTask_SweepSingleCapsule::CreateTask(this, ARSTargetActor_SweepSingleCapsule::StaticClass());
	URSAbilityTask_SweepSingleCapsule* AttackTraceTask = URSAbilityTask_SweepSingleCapsule::CreateTask(this, TargetActorClass);
	AttackTraceTask->OnComplete.AddDynamic(this, &ThisClass::OnSweepSingleCapsuleResultReady);
	AttackTraceTask->ReadyForActivation();
}

void URSGameplayAbility_CheckHit::OnSweepSingleCapsuleResultReady(const FGameplayAbilityTargetDataHandle& TargetDataHandle)
{
    // ---------- [0] Ability/ActorInfo 생존성 방어 ----------
    // CurrentActorInfo는 EndAbility 후에도 nullptr일 수 있음
    if (CurrentActorInfo == nullptr)
    {
        return; // 이미 종료된 뒤 콜백이 들어온 케이스 방어
    }

    UAbilitySystemComponent* SourceASC = GetAbilitySystemComponentFromActorInfo();
    if (!IsValid(SourceASC))
    {
        // 더 진행해봤자 Apply/Cue 불가
        EndAbility(CurrentSpecHandle, CurrentActorInfo, CurrentActivationInfo, true, true);
        return;
    }

    // 1. TargetData 기본 안전성 방어
    const int32 Index = 0;

    if (TargetDataHandle.Data.Num() <= Index || TargetDataHandle.Data[Index].Get() == nullptr)
    {
        // 타겟이 없거나(미검출) 비정상 타겟데이터가 있다면 그냥 EndAbility해주자. 이걸 안해주면 크러시가남..
        EndAbility(CurrentSpecHandle, CurrentActorInfo, CurrentActivationInfo, true, false);
        return;
    }

    //  2. HitResult 경로 
    if (UAbilitySystemBlueprintLibrary::TargetDataHasHitResult(TargetDataHandle, Index))
    {
        const FHitResult HitResult = UAbilitySystemBlueprintLibrary::GetHitResultFromTargetData(TargetDataHandle, Index);

        AActor* HitActor = HitResult.GetActor();
        if (!IsValid(HitActor))
        {
            // WorldStatic/Component-only hit 등 Actor가 없을 수 있음
            EndAbility(CurrentSpecHandle, CurrentActorInfo, CurrentActivationInfo, true, false);
            return;
        }

        UE_LOG(LogTemp, Log, TEXT("Target %s Detected"), *HitActor->GetName());

        // Target ASC는 있을 수도/없을 수도 있음 (월드 오브젝트/ASC 미보유 Actor)
        UAbilitySystemComponent* TargetASC = UAbilitySystemBlueprintLibrary::GetAbilitySystemComponent(HitActor);

        //  3. GE 적용 방어 
        // TriggerEventData가 nullptr였으면 CurrentLevel이 쓰레기일 수 있으니 ActivateAbility에서 가드 권장
        FGameplayEffectSpecHandle EffectSpecHandle = MakeOutgoingGameplayEffectSpec(AttackDamageEffect, CurrentLevel);

        if (EffectSpecHandle.IsValid())
        {
            // Apply는 TargetData 기반으로 하므로 TargetASC가 없어도 동작할 수 있음(타겟이 ASC를 가진 경우에만 실제 반영)
            ApplyGameplayEffectSpecToTarget(CurrentSpecHandle, CurrentActorInfo, CurrentActivationInfo, EffectSpecHandle, TargetDataHandle);

            // Cue 파라미터 구성
            FGameplayEffectContextHandle CueContextHandle = UAbilitySystemBlueprintLibrary::GetEffectContext(EffectSpecHandle);
            CueContextHandle.AddHitResult(HitResult);

            FGameplayCueParameters CueParam;
            CueParam.EffectContext = CueContextHandle;

            // Cue 실행은 TargetASC가 있는 경우에만
            if (IsValid(TargetASC))
            {
                TargetASC->ExecuteGameplayCue(GAMEPLAYCUE_ATTACK_HIT, CueParam);
            }
        }

        // 버프는 Source에게 - 여기서 말한 Source는 우리 RSCharacter를 말하는것임.
        FGameplayEffectSpecHandle BuffEffectSpecHandle = MakeOutgoingGameplayEffectSpec(AttackBuffEffect);
        if (BuffEffectSpecHandle.IsValid())
        {
            ApplyGameplayEffectSpecToOwner(CurrentSpecHandle, CurrentActorInfo, CurrentActivationInfo, BuffEffectSpecHandle);
        }
    }
    //  4. Actor 경로(히트리절트가 없는 타겟데이터) 
    else if (UAbilitySystemBlueprintLibrary::TargetDataHasActor(TargetDataHandle, Index))
    {
        // 여기서도 Data[Index] 안전성은 위에서 이미 확보됨
        const TArray<TWeakObjectPtr<AActor>> Actors = TargetDataHandle.Data[Index].Get()->GetActors();

        // 방어: 타겟 actor가 비어있으면 종료
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

        FGameplayEffectSpecHandle EffectSpecHandle = MakeOutgoingGameplayEffectSpec(AttackDamageEffect, CurrentLevel);
        if (EffectSpecHandle.IsValid())
        {
            ApplyGameplayEffectSpecToTarget(CurrentSpecHandle, CurrentActorInfo, CurrentActivationInfo, EffectSpecHandle, TargetDataHandle);

            FGameplayEffectContextHandle CueContextHandle = UAbilitySystemBlueprintLibrary::GetEffectContext(EffectSpecHandle);
            CueContextHandle.AddActors(Actors, false);

            FGameplayCueParameters CueParam;
            CueParam.EffectContext = CueContextHandle;

            // SourceASC는 위에서 유효성 확보
            SourceASC->ExecuteGameplayCue(GAMEPLAYCUE_ATTACK_HIT, CueParam);
        }
    }

    EndAbility(CurrentSpecHandle, CurrentActorInfo, CurrentActivationInfo, true, false);
}
