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
	ACharacter* SourceCharacter = Cast<ACharacter>(SourceActor);
	if (!IsValid(SourceCharacter))
	{
		UE_LOG(LogTemp, Error, TEXT("[TargetActor] SourceActor is not a Character or invalid"));
		return FGameplayAbilityTargetDataHandle();
	}

	UAbilitySystemComponent* ASC = UAbilitySystemBlueprintLibrary::GetAbilitySystemComponent(SourceActor);
	if (!IsValid(ASC))
	{
		UE_LOG(LogTemp, Error, TEXT("[TargetActor] ASC is invalid."));
		return FGameplayAbilityTargetDataHandle();
	}

	const URSAttributeSet_Character* AttributeSet = ASC->GetSet<URSAttributeSet_Character>();
	if (!IsValid(AttributeSet))
	{
		UE_LOG(LogTemp, Error, TEXT("[TargetActor] AttributeSet is invalid."));
		return FGameplayAbilityTargetDataHandle();
	}

    FHitResult OutHitResult;
    const float AttackRange = AttributeSet->GetAttackRange();
    const float AttackRadius = AttributeSet->GetAttackRadius();

    FCollisionQueryParams Params(FName(), false, SourceCharacter);
    const FVector Forward = SourceCharacter->GetActorForwardVector();
    const FVector Start = SourceCharacter->GetActorLocation() + Forward * SourceCharacter->GetCapsuleComponent()->GetScaledCapsuleRadius();
    const FVector End = Start + Forward * AttackRange;

    const bool bHitDetected = GetWorld()->SweepSingleByChannel(
        OutHitResult,
        Start,
        End,
        FQuat::Identity,
        ECC_Pawn,
        FCollisionShape::MakeSphere(AttackRadius),
        Params
    );

#if ENABLE_DRAW_DEBUG
    if (bShowDebug)
    {
        const FColor DrawColor = bHitDetected ? FColor::Green : FColor::Red;
        DrawDebugSphere(GetWorld(), End, AttackRadius, 16, DrawColor, false, 3.0f);
        DrawDebugLine(GetWorld(), Start, End, DrawColor, false, 3.0f, 0, 1.0f);
    }
#endif

    UE_LOG(LogTemp, Warning,
        TEXT("[TargetActor] SweepSphere Hit=%d Start=%s End=%s Range=%.1f Radius=%.1f HitActor=%s"),
        bHitDetected ? 1 : 0,
        *Start.ToString(),
        *End.ToString(),
        AttackRange,
        AttackRadius,
        bHitDetected ? *GetNameSafe(OutHitResult.GetActor()) : TEXT("None")
    );

    FGameplayAbilityTargetDataHandle DataHandle;
    if (bHitDetected)
    {
        auto* TargetData = new FGameplayAbilityTargetData_SingleTargetHit(OutHitResult);
        DataHandle.Add(TargetData);
    }

    return DataHandle;

}
