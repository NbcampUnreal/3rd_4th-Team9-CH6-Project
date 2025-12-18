// Fill out your copyright notice in the Description page of Project Settings.


#include "GAS/TA/RSTargetActor_SweepSingleCapsule.h"
#include "Abilities/GameplayAbility.h"
#include "GameFramework/Character.h"
#include "Components/CapsuleComponent.h"
#include "AbilitySystemBlueprintLibrary.h"
#include "AbilitySystemComponent.h"
#include "RemnantSoul.h"
#include "GAS/AS/RSAttributeSet_Character.h"

ARSTargetActor_SweepSingleCapsule::ARSTargetActor_SweepSingleCapsule()
{
}

void ARSTargetActor_SweepSingleCapsule::StartTargeting(UGameplayAbility* Ability)
{
	Super::StartTargeting(Ability);

	SourceActor = Ability->GetCurrentActorInfo()->AvatarActor.Get();
}

void ARSTargetActor_SweepSingleCapsule::ConfirmTargetingAndContinue()
{
	if (SourceActor)
	{
		FGameplayAbilityTargetDataHandle DataHandle = MakeTargetData();
		TargetDataReadyDelegate.Broadcast(DataHandle);
	}
}

FGameplayAbilityTargetDataHandle ARSTargetActor_SweepSingleCapsule::MakeTargetData() const
{
	ACharacter* SourceCharacter = CastChecked<ACharacter>(SourceActor);

	UAbilitySystemComponent* ASC = UAbilitySystemBlueprintLibrary::GetAbilitySystemComponent(SourceActor);
	if (IsValid(ASC) == false)
	{
		UE_LOG(LogTemp, Error, TEXT("ASC is invalid."));
		return FGameplayAbilityTargetDataHandle();
	}

	const URSAttributeSet_Character* AttributeSet = ASC->GetSet<URSAttributeSet_Character>();
	if (IsValid(AttributeSet) == false)
	{
		UE_LOG(LogTemp, Error, TEXT("AttributeSet is invalid."));
		return FGameplayAbilityTargetDataHandle();
	}

	FHitResult OutHitResult;
	//const float AttackRange = 100.0f;
	//const float AttackRadius = 50.f;
	const float AttackRange = AttributeSet->GetAttackRange();
	const float AttackRadius = AttributeSet->GetAttackRadius();

	FCollisionQueryParams Params(FName(), false, SourceCharacter);
	const FVector Forward = SourceCharacter->GetActorForwardVector();
	const FVector Start = SourceCharacter->GetActorLocation() + Forward * SourceCharacter->GetCapsuleComponent()->GetScaledCapsuleRadius();
	const FVector End = Start + Forward * AttackRange;

	bool HitDetected = GetWorld()->SweepSingleByChannel(OutHitResult, Start, End, FQuat::Identity, ECC_Pawn, FCollisionShape::MakeSphere(AttackRadius), Params);

	FGameplayAbilityTargetDataHandle DataHandle;
	if (HitDetected)
	{
		FGameplayAbilityTargetData_SingleTargetHit* TargetData = new FGameplayAbilityTargetData_SingleTargetHit(OutHitResult);
		DataHandle.Add(TargetData);
	}

#if ENABLE_DRAW_DEBUG

	if (bShowDebug)
	{
		FVector CapsuleOrigin = Start + (End - Start) * 0.5f;
		float CapsuleHalfHeight = AttackRange * 0.5f;
		FColor DrawColor = HitDetected ? FColor::Green : FColor::Red;
		DrawDebugCapsule(GetWorld(), CapsuleOrigin, CapsuleHalfHeight, AttackRadius, FRotationMatrix::MakeFromZ(Forward).ToQuat(), DrawColor, false, 3.0f);
	}

#endif

	return DataHandle;
}
