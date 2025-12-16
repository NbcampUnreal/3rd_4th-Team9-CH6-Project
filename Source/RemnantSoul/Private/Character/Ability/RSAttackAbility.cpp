// Fill out your copyright notice in the Description page of Project Settings.


#include "Character/Ability/RSAttackAbility.h"
#include "AbilitySystemComponent.h"
#include "GameFramework/Character.h"
#include "Kismet/GameplayStatics.h"
#include "AbilitySystemGlobals.h"
#include "DrawDebugHelpers.h" //어빌리티가 실행되는지 확인할 디버깅

URSAttackAbility::URSAttackAbility()
{
	InstancingPolicy = EGameplayAbilityInstancingPolicy::InstancedPerActor;

	//추후 이 파트 점검할 것, 중단점 걸림.
	AbilityTags.AddTag(FGameplayTag::RequestGameplayTag(TEXT("Ability.Attack")));
	//ActivationOwnedTags.AddTag(FGameplayTag::RequestGameplayTag(TEXT("State.Attacking")));

	// 캐릭터가 사망한 후에는 공격 불가로 추후 확장 가능.
	//BlockAbilitiesWithTag.AddTag(FGameplayTag::RequestGameplayTag(TEXT("State.Dead")));
}

void URSAttackAbility::ActivateAbility(
	const FGameplayAbilitySpecHandle Handle,
	const FGameplayAbilityActorInfo* ActorInfo,
	const FGameplayAbilityActivationInfo ActivationInfo,
	const FGameplayEventData* TriggerEventData)
{

	if (!CommitAbility(Handle, ActorInfo, ActivationInfo))
	{
		EndAbility(Handle, ActorInfo, ActivationInfo, true, false);
		return;
	}

	PerformAttackTrace(ActorInfo);

	EndAbility(Handle, ActorInfo, ActivationInfo, true, false);
}

void URSAttackAbility::PerformAttackTrace(const FGameplayAbilityActorInfo* ActorInfo)
{
	ACharacter* OwnerCharacter = Cast<ACharacter>(ActorInfo->AvatarActor.Get());
	if (!OwnerCharacter) return;

	const FVector Start = OwnerCharacter->GetActorLocation();
	const FVector End = Start + OwnerCharacter->GetActorForwardVector() * AttackRange;

	FCollisionShape Shape = FCollisionShape::MakeSphere(AttackRadius);
	FCollisionQueryParams Params;
	Params.AddIgnoredActor(OwnerCharacter);

	TArray<FHitResult> Hits;

	bool bHit = OwnerCharacter->GetWorld()->SweepMultiByChannel(
		Hits,
		Start,
		End,
		FQuat::Identity,
		ECC_Pawn,
		Shape,
		Params
	);

#if ENABLE_DRAW_DEBUG
	DrawDebugCapsule(
		OwnerCharacter->GetWorld(),
		(Start + End) * 0.5f,
		AttackRange * 0.5f,
		AttackRadius,
		FQuat::Identity,
		bHit ? FColor::Red : FColor::Green,
		false,
		1.f
	);
#endif

	if (!bHit) return;

	for (const FHitResult& Hit : Hits)
	{
		AActor* TargetActor = Hit.GetActor();
		if (!TargetActor) continue;

		UAbilitySystemComponent* TargetASC =
			UAbilitySystemGlobals::GetAbilitySystemComponentFromActor(TargetActor);

		if (!TargetASC || !DamageEffectClass) continue;

		FGameplayEffectSpecHandle SpecHandle =
			MakeOutgoingGameplayEffectSpec(DamageEffectClass, GetAbilityLevel());

		if (SpecHandle.IsValid())
		{
			TargetASC->ApplyGameplayEffectSpecToSelf(*SpecHandle.Data.Get());
		}
	}
}