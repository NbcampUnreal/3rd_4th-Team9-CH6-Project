#include "GAS/TA/RSTargetActor_OverlapMultiSphere.h"

#include "AbilitySystemBlueprintLibrary.h"
#include "RemnantSoul.h"
#include "GameFramework/Character.h"
#include "Engine/OverlapResult.h"
#include "GAS/AS/RSAttributeSet_Skill.h"

FGameplayAbilityTargetDataHandle ARSTargetActor_OverlapMultiSphere::MakeTargetData() const
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

	const URSAttributeSet_Skill* AttributeSet = ASC->GetSet<URSAttributeSet_Skill>();
	if (!IsValid(AttributeSet))
	{
		UE_LOG(LogTemp, Error, TEXT("[TargetActor] AttributeSet is invalid."));
		return FGameplayAbilityTargetDataHandle();
	}

	TArray<FOverlapResult> Overlaps;
	const float SkillRadius = AttributeSet->GetSkillRadius();

	const FVector Origin = SourceCharacter->GetActorLocation();
	FCollisionQueryParams Params(FName(), false, SourceCharacter);

	GetWorld()->OverlapMultiByChannel(
		Overlaps,
		Origin,
		FQuat::Identity,
		ECC_Pawn,
		FCollisionShape::MakeSphere(SkillRadius),
		Params
	);

	TArray<TWeakObjectPtr<AActor>> HitActors;
	for (const FOverlapResult& Overlap : Overlaps)
	{
		AActor* HitActor = Overlap.OverlapObjectHandle.FetchActor<AActor>();
		if (HitActor && !HitActors.Contains(HitActor))
		{
			HitActors.Add(HitActor);
		}
	}

	FGameplayAbilityTargetData_ActorArray* ActorsData = new FGameplayAbilityTargetData_ActorArray();
	ActorsData->SetActors(HitActors);

#if ENABLE_DRAW_DEBUG
	if (bShowDebug)
	{
		const FColor DrawColor = HitActors.Num() > 0 ? FColor::Green : FColor::Red;
		DrawDebugSphere(GetWorld(), Origin, SkillRadius, 16, DrawColor, false, 5.0f);
	}
#endif

	return FGameplayAbilityTargetDataHandle(ActorsData);
}
