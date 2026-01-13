#include "GAS/GA/RSGameplayAbility_Sprint_Check.h"

#include "GameFramework/Character.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "AbilitySystemComponent.h"
#include "AbilitySystemBlueprintLibrary.h"
#include "RSGameplayTags.h"

#include UE_INLINE_GENERATED_CPP_BY_NAME(RSGameplayAbility_Sprint_Check)

URSGameplayAbility_Sprint_Check::URSGameplayAbility_Sprint_Check()
{
	const FRSGameplayTags& Tags = FRSGameplayTags::Get();
	AbilityTags.AddTag(Tags.Ability_Sprint_Check);

	ReplicationPolicy = EGameplayAbilityReplicationPolicy::ReplicateYes;
	InstancingPolicy = EGameplayAbilityInstancingPolicy::InstancedPerActor;
}

void URSGameplayAbility_Sprint_Check::ActivateAbility(
	const FGameplayAbilitySpecHandle Handle,
	const FGameplayAbilityActorInfo* ActorInfo,
	const FGameplayAbilityActivationInfo ActivationInfo,
	const FGameplayEventData* TriggerEventData)
{
	Super::ActivateAbility(Handle, ActorInfo, ActivationInfo, TriggerEventData);

	// 입력 기반 체크는 로컬에서만 수행
	if (!IsLocallyControlled())
	{
		EndAbility(Handle, ActorInfo, ActivationInfo, true, false);
		return;
	}

	FVector MoveVectorWS = FVector::ZeroVector;
	if (!CheckSprintConditions(MoveVectorWS))
	{
		CancelAbility(Handle, ActorInfo, ActivationInfo, true);
		return;
	}

	// 권한 없으면 서버에 요청(서버가 Active 이벤트를 다시 발생시키게 함)
	if (!HasAuthority(&ActivationInfo))
	{
		Server_RequestSprint(MoveVectorWS);
		// 클라 즉시 반응(체감): Active 이벤트도 로컬에서 미리 쏴준다.
		SendSprintActiveEvent(MoveVectorWS);
	}
	else
	{
		SendSprintActiveEvent(MoveVectorWS);
	}

	EndAbility(Handle, ActorInfo, ActivationInfo, true, false);
}

bool URSGameplayAbility_Sprint_Check::CheckSprintConditions(FVector& OutMoveVectorWS) const
{
	ACharacter* Char = Cast<ACharacter>(GetAvatarActorFromActorInfo());
	if (!Char)
	{
		return false;
	}

	UCharacterMovementComponent* Move = Char->GetCharacterMovement();
	if (!Move || Move->IsFalling())
	{
		return false;
	}

	UAbilitySystemComponent* ASC = GetAbilitySystemComponentFromActorInfo();
	if (!ASC)
	{
		return false;
	}

	// (선택) 자원 체크: Attribute가 유효하면 검사, 아니면 스킵
	if (SprintCostAttribute.IsValid())
	{
		bool bFound = false;
		const float RemainValue =
			UAbilitySystemBlueprintLibrary::GetFloatAttributeFromAbilitySystemComponent(ASC, SprintCostAttribute, bFound);

		if (!bFound || RemainValue <= MinRemainValueToSprint)
		{
			return false;
		}
	}

	// 방향 판정: 가속(입력) 우선, 없으면 속도 사용
	const FVector Accel = Move->GetCurrentAcceleration();
	FVector DirWS = !Accel.IsNearlyZero() ? Accel.GetSafeNormal() : Move->Velocity.GetSafeNormal();
	if (DirWS.IsNearlyZero())
	{
		return false;
	}

	const FTransform T = Char->GetActorTransform();
	const FVector LocalDir = T.InverseTransformVectorNoScale(DirWS).GetSafeNormal();

	// 전방(로컬 X) 체크
	if (LocalDir.X <= ForwardThreshold)
	{
		return false;
	}

	OutMoveVectorWS = DirWS;
	return true;
}

void URSGameplayAbility_Sprint_Check::Server_RequestSprint_Implementation(const FVector& MoveVectorWS)
{
	SendSprintActiveEvent(MoveVectorWS);
}

void URSGameplayAbility_Sprint_Check::SendSprintActiveEvent(const FVector& MoveVectorWS) const
{
	const FRSGameplayTags& Tags = FRSGameplayTags::Get();

	FGameplayEventData Payload;
	// 간단 전달: Normal에 방향을 싣는다(프로젝트 규칙이면 바꿔도 됨)
	FHitResult HR;
	HR.Normal = MoveVectorWS;
	Payload.TargetData = UAbilitySystemBlueprintLibrary::AbilityTargetDataFromHitResult(HR);

	UAbilitySystemBlueprintLibrary::SendGameplayEventToActor(GetAvatarActorFromActorInfo(), Tags.Ability_Sprint_Active, Payload);
}
