

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

void URSGameplayAbility_CheckHit::OnSweepSingleCapsuleResultReady(
	const FGameplayAbilityTargetDataHandle& TargetDataHandle)
{
	if (UAbilitySystemBlueprintLibrary::TargetDataHasHitResult(TargetDataHandle, 0))
	{
		FHitResult HitResult = UAbilitySystemBlueprintLibrary::GetHitResultFromTargetData(TargetDataHandle, 0);
		UE_LOG(LogTemp, Log, TEXT("Target %s Detected"), *(HitResult.GetActor()->GetName()));

		/*UAbilitySystemComponent* SourceASC = GetAbilitySystemComponentFromActorInfo_Ensured();
		UAbilitySystemComponent* TargetASC = UAbilitySystemBlueprintLibrary::GetAbilitySystemComponent(HitResult.GetActor());
		if (IsValid(SourceASC) == false || IsValid(TargetASC) == false)
		{
			UE_LOG(LogTemp, Error, TEXT("IsValid(SourceASC) == false OR IsValid(TargetASC) == false"));
			return;
		}

		const URSAttributeSet_Character* SourceAttribute = SourceASC->GetSet<URSAttributeSet_Character>();
		URSAttributeSet_Character* TargetAttribute = const_cast<URSAttributeSet_Character*>(TargetASC->GetSet<URSAttributeSet_Character>());
		if (IsValid(SourceAttribute) == false || IsValid(TargetAttribute) == false)
		{
			UE_LOG(LogTemp, Error, TEXT("IsValid(SourceAttribute) == false OR IsValid(TargetASC) == false"));
			return;
		}

		const float AttackDamage = SourceAttribute->GetAttackDamage();
		TargetAttribute->SetHealth(TargetAttribute->GetHealth() - AttackDamage);*/

		//FGameplayEffectSpecHandle EffectSpecHandle = MakeOutgoingGameplayEffectSpec(AttackDamageEffect);
		//if (EffectSpecHandle.IsValid() == true)
		//{
		//	ApplyGameplayEffectSpecToTarget(CurrentSpecHandle, CurrentActorInfo, CurrentActivationInfo, EffectSpecHandle, TargetDataHandle);
		//}

		UAbilitySystemComponent* SourceASC = GetAbilitySystemComponentFromActorInfo_Ensured();
		//const URSAttributeSet_Character* SourceAttributeSet = SourceASC->GetSet<URSAttributeSet_Character>();
		UAbilitySystemComponent* TargetASC = UAbilitySystemBlueprintLibrary::GetAbilitySystemComponent(HitResult.GetActor());

		//FGameplayEffectSpecHandle EffectSpecHandle = MakeOutgoingGameplayEffectSpec(AttackDamageEffect);
		FGameplayEffectSpecHandle EffectSpecHandle = MakeOutgoingGameplayEffectSpec(AttackDamageEffect, CurrentLevel);
		if (EffectSpecHandle.IsValid())
		{
			//EffectSpecHandle.Data->SetSetByCallerMagnitude(DATA_DAMAGE, -SourceAttributeSet->GetAttackDamage());            
			ApplyGameplayEffectSpecToTarget(CurrentSpecHandle, CurrentActorInfo, CurrentActivationInfo, EffectSpecHandle, TargetDataHandle);

			FGameplayEffectContextHandle CueContextHandle = UAbilitySystemBlueprintLibrary::GetEffectContext(EffectSpecHandle);
			CueContextHandle.AddHitResult(HitResult);
			FGameplayCueParameters CueParam;
			CueParam.EffectContext = CueContextHandle;

			if (IsValid(TargetASC) == true)
			{
				TargetASC->ExecuteGameplayCue(GAMEPLAYCUE_ATTACK_HIT, CueParam);
			}
		}

		FGameplayEffectSpecHandle BuffEffectSpecHandle = MakeOutgoingGameplayEffectSpec(AttackBuffEffect);
		if (BuffEffectSpecHandle.IsValid())
		{
			ApplyGameplayEffectSpecToOwner(CurrentSpecHandle, CurrentActorInfo, CurrentActivationInfo, BuffEffectSpecHandle);
		}
	}
	else if (UAbilitySystemBlueprintLibrary::TargetDataHasActor(TargetDataHandle, 0))
	{
		UAbilitySystemComponent* SourceASC = GetAbilitySystemComponentFromActorInfo_Ensured();

		FGameplayEffectSpecHandle EffectSpecHandle = MakeOutgoingGameplayEffectSpec(AttackDamageEffect, CurrentLevel);
		if (EffectSpecHandle.IsValid())
		{
			ApplyGameplayEffectSpecToTarget(CurrentSpecHandle, CurrentActorInfo, CurrentActivationInfo, EffectSpecHandle, TargetDataHandle);

			FGameplayEffectContextHandle CueContextHandle = UAbilitySystemBlueprintLibrary::GetEffectContext(EffectSpecHandle);
			CueContextHandle.AddActors(TargetDataHandle.Data[0].Get()->GetActors(), false);
			FGameplayCueParameters CueParam;
			CueParam.EffectContext = CueContextHandle;

			SourceASC->ExecuteGameplayCue(GAMEPLAYCUE_ATTACK_HIT, CueParam);
		}
	}

	bool bReplicatedEndAbility = true;
	bool bWasCancelled = false;
	EndAbility(CurrentSpecHandle, CurrentActorInfo, CurrentActivationInfo, bReplicatedEndAbility, bWasCancelled);

}