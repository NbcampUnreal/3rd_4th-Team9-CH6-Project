// Fill out your copyright notice in the Description page of Project Settings.


#include "Character/Ability/RS_Roll_Ability.h"
#include "AbilitySystemComponent.h"
#include "Character/RSBaseCharacter.h"
#include "Animation/AnimInstance//RSAnimInstance.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "GAS/AT/RSAbilityTask_RollMove.h"
#include "Abilities/Tasks/AbilityTask_WaitGameplayEvent.h"


URS_Roll_Ability::URS_Roll_Ability()
{
	InstancingPolicy = EGameplayAbilityInstancingPolicy::InstancedPerActor;
	NetExecutionPolicy = EGameplayAbilityNetExecutionPolicy::LocalOnly;

	//어빌리티를 실행할 태그 설정.
	AbilityTags.AddTag(FGameplayTag::RequestGameplayTag(TEXT("Ability.Roll")));

	//이 태그가 있을 시 어빌리티를 실행하지 않음.
	ActivationBlockedTags.AddTag(FGameplayTag::RequestGameplayTag(TEXT("Roll.CoolDown")));

	//이 어빌리티의 소유자가 State.Dead 태그를 소유할 시 실행 불가, 플레이어 캐릭터 사망 시 어빌리티 실행을 막는 구조로 구상 중이며 추후 리스폰 완성되면 최종적으로 확립할 예정.
	//ActivationBlockedTags.AddTag(FGameplayTag::RequestGameplayTag(TEXT("State.Dead")));

	//이벤트 기반 구르기 어빌리티 종료 태그
	RollEndEventTag = FGameplayTag::RequestGameplayTag(TEXT("Event.Roll.End"));

	CostGameplayEffectClass = RollCostGE;
	CooldownGameplayEffectClass = RollCooldownGE;
	
}


void URS_Roll_Ability::ActivateAbility(
	const FGameplayAbilitySpecHandle Handle,
	const FGameplayAbilityActorInfo* ActorInfo,
	const FGameplayAbilityActivationInfo ActivationInfo,
	const FGameplayEventData* TriggerEventData
)
{
	
	if (!CommitAbility(Handle, ActorInfo, ActivationInfo))
	{
		EndAbility(Handle, ActorInfo, ActivationInfo, false, true);
		return;
	}

	ACharacter* Character =
		Cast<ACharacter>(ActorInfo->AvatarActor.Get());
	if (!Character)
	{
		EndAbility(Handle, ActorInfo, ActivationInfo, false, true);
		return;
	}

	// 이동 차단
	if (UCharacterMovementComponent* MoveComp =
		Character->GetCharacterMovement())
	{
		MoveComp->DisableMovement();
	}

	// Roll 방향 계산
	const FVector InputDir = Character->GetLastMovementInputVector();
	const FVector RollDir = InputDir.IsNearlyZero()? Character->GetActorForwardVector(): InputDir.GetSafeNormal();

	const FVector Forward = Character->GetActorForwardVector();
	const FVector Right   = Character->GetActorRightVector();

	const float ForwardDot = FVector::DotProduct(Forward, RollDir);
	const float RightDot   = FVector::DotProduct(Right,   RollDir);

	CachedRollAngle =FMath::RadiansToDegrees(FMath::Atan2(RightDot, ForwardDot));

	if (URSAnimInstance* Anim =Cast<URSAnimInstance>(Character->GetMesh()->GetAnimInstance()))
	{
		Anim->SetRollAngle(CachedRollAngle);
		Anim->SetIsRolling(true);
	}

	// Roll 이동 태스크
	const FVector RollMoveDir = Forward * ForwardDot + Right * RightDot;

	URSAbilityTask_RollMove* RollMoveTask = URSAbilityTask_RollMove::RollMove(this,RollMoveDir,600.f,0.6f
		);

	if (RollMoveTask)
	{
		RollMoveTask->OnBlocked.AddDynamic(this,&URS_Roll_Ability::OnRollBlocked);
		RollMoveTask->ReadyForActivation();
	}
	else
	{
	}
	
	// AnimNotify 종료 이벤트 대기
	
	WaitEndEvent = UAbilityTask_WaitGameplayEvent::WaitGameplayEvent(this, RollEndEventTag);
	WaitEndEvent->EventReceived.AddDynamic(this, &URS_Roll_Ability::OnRollEndEvent);
	WaitEndEvent->ReadyForActivation();
	
}


void URS_Roll_Ability::OnRollEndEvent(FGameplayEventData Payload)
{
	EndAbility(CurrentSpecHandle, CurrentActorInfo, CurrentActivationInfo, false, false);
}

void URS_Roll_Ability::EndAbility(
	const FGameplayAbilitySpecHandle Handle,
	const FGameplayAbilityActorInfo* ActorInfo,
	const FGameplayAbilityActivationInfo ActivationInfo,
	bool bReplicateEndAbility,
	bool bWasCancelled
)
{
	if (ActorInfo && ActorInfo->AvatarActor.IsValid())
	{
		if (ACharacter* Character =
			Cast<ACharacter>(ActorInfo->AvatarActor.Get()))
		{
			if (UCharacterMovementComponent* MoveComp =Character->GetCharacterMovement())
			{
				MoveComp->SetMovementMode(MOVE_Walking);
			}

			if (URSAnimInstance* Anim =Cast<URSAnimInstance>(Character->GetMesh()->GetAnimInstance()))
			{
				Anim->SetRollAngle(0.f);
				Anim->SetIsRolling(false);
			}
		}
	}
	
	CachedRollAngle = 0.f;

	Super::EndAbility(
		Handle,
		ActorInfo,
		ActivationInfo,
		bReplicateEndAbility,
		bWasCancelled
	);
}


void URS_Roll_Ability::OnRollBlocked()
{

	EndAbility(CurrentSpecHandle, CurrentActorInfo, CurrentActivationInfo, false, true);
}

