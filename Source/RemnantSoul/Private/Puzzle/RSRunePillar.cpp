#include "Puzzle/RSRunePillar.h"

#include "Components/SkeletalMeshComponent.h"
#include "GameFramework/Controller.h"
#include "GameFramework/Pawn.h"
#include "Engine/World.h"
#include "GameFramework/Actor.h"
#include "Components/PrimitiveComponent.h"

#include "Puzzle/RSRunePillarAnimInstance.h"

const FName ARSRunePillar::BoneTop(TEXT("bone_003"));
const FName ARSRunePillar::BoneMid(TEXT("bone_002"));
const FName ARSRunePillar::BoneBot(TEXT("bone_001"));

ARSRunePillar::ARSRunePillar()
{
	PrimaryActorTick.bCanEverTick = false;

	Root = CreateDefaultSubobject<USceneComponent>(TEXT("Root"));
	SetRootComponent(Root);

	Mesh = CreateDefaultSubobject<USkeletalMeshComponent>(TEXT("Mesh"));
	Mesh->SetupAttachment(Root);

	// Trace로 뼈를 맞추려면 Query+Visibility Block 권장
	Mesh->SetCollisionEnabled(ECollisionEnabled::QueryOnly);
	Mesh->SetCollisionResponseToAllChannels(ECR_Ignore);
	Mesh->SetCollisionResponseToChannel(ECC_Visibility, ECR_Block);

	Mesh->SetRenderCustomDepth(false);
}

void ARSRunePillar::BeginPlay()
{
	Super::BeginPlay();
	// 90도 고정 = 4스테이트 고정
	MaxState = 4;
	StepDegrees = 90.f;
	
	TopState = FMath::Clamp(TopState, 0, 3);
	MidState = FMath::Clamp(MidState, 0, 3);
	BotState = FMath::Clamp(BotState, 0, 3);

	TargetTop = FMath::Clamp(TargetTop, 0, 3);
	TargetMid = FMath::Clamp(TargetMid, 0, 3);
	TargetBot = FMath::Clamp(TargetBot, 0, 3);
	const int32 TopIdx = Mesh ? Mesh->GetBoneIndex(BoneTop) : INDEX_NONE;
	const int32 MidIdx = Mesh ? Mesh->GetBoneIndex(BoneMid) : INDEX_NONE;
	const int32 BotIdx = Mesh ? Mesh->GetBoneIndex(BoneBot) : INDEX_NONE;

	UE_LOG(LogTemp, Warning, TEXT("[RunePillar] Mesh=%s Anim=%s"),
		*GetNameSafe(Mesh),
		Mesh ? *GetNameSafe(Mesh->GetAnimInstance()) : TEXT("None"));

	UE_LOG(LogTemp, Warning, TEXT("[RunePillar] BoneIndex Top(%s)=%d Mid(%s)=%d Bot(%s)=%d"),
		*BoneTop.ToString(), TopIdx,
		*BoneMid.ToString(), MidIdx,
		*BoneBot.ToString(), BotIdx);

	ApplyAnimRotations();

	
}

bool ARSRunePillar::CanInteract_Implementation(AActor* Interactor) const
{
	return IsValid(Interactor) && !bSolved;
}

void ARSRunePillar::Interact_Implementation(AActor* Interactor)
{
	
	
	if (!CanInteract_Implementation(Interactor) || !Mesh)
	{
		return;
	}

	FName HitBone(NAME_None);
	if (!TryResolveHitBone(Interactor, HitBone))
	{
		return;
	}

	AdvanceByBone(HitBone);
	ApplyAnimRotations();

	if (!bSolved && IsSolved())
	{
		UE_LOG(LogTemp, Warning, TEXT("[RunePillar] SOLVED! T/M/B=%d/%d/%d Target=%d/%d/%d"),
			TopState, MidState, BotState, TargetTop, TargetMid, TargetBot);

		bSolved = true;
		BP_OnSolved(Interactor);
		OpenFogWall();
	}
	else
	{
		UE_LOG(LogTemp, Warning, TEXT("[RunePillar] NotSolved. T/M/B=%d/%d/%d Target=%d/%d/%d bSolved=%d"),
			TopState, MidState, BotState, TargetTop, TargetMid, TargetBot, bSolved?1:0);
	}
}

void ARSRunePillar::OnFocusBegin_Implementation(AActor* Interactor)
{
	if (bHighlightOnFocus) SetHighlight(true);
}

void ARSRunePillar::OnFocusEnd_Implementation(AActor* Interactor)
{
	if (bHighlightOnFocus) SetHighlight(false);
}

URSRunePillarAnimInstance* ARSRunePillar::GetRuneAnim() const
{
	return Mesh ? Cast<URSRunePillarAnimInstance>(Mesh->GetAnimInstance()) : nullptr;
}

void ARSRunePillar::ApplyAnimRotations() const
{
	if (URSRunePillarAnimInstance* Anim = GetRuneAnim())
	{
		Anim->TopYaw = StepDegrees * float(TopState);
		Anim->MidYaw = StepDegrees * float(MidState);
		Anim->BotYaw = StepDegrees * float(BotState);
	}
}

bool ARSRunePillar::TryResolveHitBone(AActor* Interactor, FName& OutBone) const
{
    OutBone = NAME_None;
	
	

    const UWorld* World = GetWorld();
    if (!World || !Mesh || !IsValid(Interactor))
    {
        return false;
    }

    FVector ViewLoc;
    FRotator ViewRot;

    if (const APawn* Pawn = Cast<APawn>(Interactor))
    {
        if (const AController* C = Pawn->GetController())
        {
            C->GetPlayerViewPoint(ViewLoc, ViewRot);
        }
        else
        {
            return false;
        }
    }
    else if (const AController* C = Cast<AController>(Interactor))
    {
        C->GetPlayerViewPoint(ViewLoc, ViewRot);
    }
    else
    {
        return false;
    }

    const FVector Start = ViewLoc;
    const FVector End   = Start + (ViewRot.Vector() * TraceDistance); 

	if (!World || !Mesh)
	{
		UE_LOG(LogTemp, Warning, TEXT("[TryResolveHitBone] World/Mesh invalid World=%s Mesh=%s"),
			*GetNameSafe(World), *GetNameSafe(Mesh));
		return false;
	}
    FCollisionQueryParams Params(SCENE_QUERY_STAT(RunePillarInteract), bTraceComplex);
    Params.AddIgnoredActor(Interactor);
    Params.bReturnPhysicalMaterial = false;

    FHitResult Hit;
    bool bHit = World->LineTraceSingleByChannel(Hit, Start, End, ECC_Visibility, Params);

	UE_LOG(LogTemp, Warning, TEXT("[RunePillar] HIT Actor=%s Comp=%s Bone=%s Impact=%s"),
	*GetNameSafe(Hit.GetActor()),
	*GetNameSafe(Hit.Component.Get()),
	*Hit.BoneName.ToString(),
	*Hit.ImpactPoint.ToString());
    if (!bHit)
    {
        const float Radius = 8.f; 
        bHit = World->SweepSingleByChannel(
            Hit,
            Start,
            End,
            FQuat::Identity,
            ECC_Visibility,
            FCollisionShape::MakeSphere(Radius),
            Params
        );
    }

    if (!bHit) return false;
    if (Hit.GetActor() != this) return false;
    if (Hit.Component.Get() != Mesh) return false;

    if (Hit.BoneName != NAME_None)
    {
        OutBone = Hit.BoneName;
        return true;
    }

    const FVector P = Hit.ImpactPoint;

    auto GetBoneLocSafe = [this](FName Bone, FVector& OutLoc) -> bool
    {
        const int32 Idx = Mesh->GetBoneIndex(Bone);
        if (Idx == INDEX_NONE) return false;
        OutLoc = Mesh->GetBoneLocation(Bone);
        return true;
    };

    FVector TopLoc, MidLoc, BotLoc;
    const bool bHasTop = GetBoneLocSafe(BoneTop, TopLoc);
    const bool bHasMid = GetBoneLocSafe(BoneMid, MidLoc);
    const bool bHasBot = GetBoneLocSafe(BoneBot, BotLoc);

    if (!(bHasTop || bHasMid || bHasBot))
    {
        return false;
    }

    float BestD2 = TNumericLimits<float>::Max();
    FName BestBone = NAME_None;

    auto Consider = [&](bool bValid, const FVector& Loc, FName BoneName)
    {
        if (!bValid) return;
        const float D2 = FVector::DistSquared(P, Loc);
        if (D2 < BestD2)
        {
            BestD2 = D2;
            BestBone = BoneName;
        }
    };

    Consider(bHasTop, TopLoc, BoneTop);
    Consider(bHasMid, MidLoc, BoneMid);
    Consider(bHasBot, BotLoc, BoneBot);

    OutBone = BestBone;
    return OutBone != NAME_None;
}

void ARSRunePillar::AdvanceByBone(const FName& Bone)
{
	const int32 BT = TopState, BM = MidState, BB = BotState;

	auto Advance = [](int32& State)
	{
		State = (State + 1) % 4; // 90도 고정이면 무조건 4
	};

	if (Bone == BoneTop)      Advance(TopState);
	else if (Bone == BoneMid) Advance(MidState);
	else if (Bone == BoneBot) Advance(BotState);
	
	UE_LOG(LogTemp, Warning, TEXT("[RunePillar] Advance Bone=%s  %d/%d/%d -> %d/%d/%d"),
		*Bone.ToString(), BT, BM, BB, TopState, MidState, BotState);
}

bool ARSRunePillar::IsSolved() const
{
	return TopState == TargetTop
		&& MidState == TargetMid
		&& BotState == TargetBot;
}

void ARSRunePillar::SetHighlight(bool bEnable) const
{
	if (!Mesh) return;

	Mesh->SetRenderCustomDepth(bEnable);
	if (bEnable)
	{
		Mesh->SetCustomDepthStencilValue(FMath::Clamp(HighlightStencilValue, 0, 255));
	}
}

void ARSRunePillar::OpenFogWall() const
{
	if (!bHideFogWallOnSolved && !bDisableFogCollisionOnSolved)
	{
		return;
	}

	if (!IsValid(FogWallActor))
	{
		UE_LOG(LogTemp, Warning, TEXT("[RSRunePillar] FogWallActor is NULL"));
		return;
	}

	if (bDisableFogCollisionOnSolved)
	{
		FogWallActor->SetActorEnableCollision(false);

		TArray<UPrimitiveComponent*> PrimComps;
		FogWallActor->GetComponents<UPrimitiveComponent>(PrimComps);
		for (UPrimitiveComponent* Comp : PrimComps)
		{
			if (Comp)
			{
				Comp->SetCollisionEnabled(ECollisionEnabled::NoCollision);
			}
		}
	}

	// 2) 숨기기
	if (bHideFogWallOnSolved)
	{
		FogWallActor->SetActorHiddenInGame(true);

		TArray<UPrimitiveComponent*> PrimComps;
		FogWallActor->GetComponents<UPrimitiveComponent>(PrimComps);
		for (UPrimitiveComponent* Comp : PrimComps)
		{
			if (Comp)
			{
				Comp->SetVisibility(false, true);
				Comp->SetHiddenInGame(true, true);
			}
		}
	}

	UE_LOG(LogTemp, Warning, TEXT("[RSRunePillar] FogWall opened: %s"), *GetNameSafe(FogWallActor));
}
