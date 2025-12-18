#include "GAS/TA/RSTargetActor_OverlapMultiSphere.h"

#include "AbilitySystemBlueprintLibrary.h"
#include "RemnantSoul.h"
#include "GameFramework/Character.h"
#include "Engine/OverlapResult.h"
#include "GAS/AS/RSAttributeSet_Skill.h"

FGameplayAbilityTargetDataHandle ARSTargetActor_OverlapMultiSphere::MakeTargetData() const
{
	ACharacter* SourceCharacter = CastChecked<ACharacter>(SourceActor);

	UAbilitySystemComponent* ASC = UAbilitySystemBlueprintLibrary::GetAbilitySystemComponent(SourceActor);
	if (IsValid(ASC) == false)
	{
		UE_LOG(LogTemp, Error, TEXT("ASC is invalid."));
		return FGameplayAbilityTargetDataHandle();
	}

	const URSAttributeSet_Skill* AttributeSet = ASC->GetSet<URSAttributeSet_Skill>();
	if (IsValid(AttributeSet) == false)
	{
		UE_LOG(LogTemp, Error, TEXT("AttributeSet is invalid."));
		return FGameplayAbilityTargetDataHandle();
	}

	TArray<FOverlapResult> Overlaps;
	const float SkillRadius = AttributeSet->GetSkillRadius();

	FVector Origin = SourceCharacter->GetActorLocation();
	FCollisionQueryParams Params(FName(), false, SourceCharacter);
	GetWorld()->OverlapMultiByChannel(Overlaps, Origin, FQuat::Identity, ECC_Pawn, FCollisionShape::MakeSphere(SkillRadius), Params);

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
		FColor DrawColor = HitActors.Num() > 0 ? FColor::Green : FColor::Red;
		DrawDebugSphere(GetWorld(), Origin, SkillRadius, 16, DrawColor, false, 5.0f);
	}

#endif

	return FGameplayAbilityTargetDataHandle(ActorsData);
}
