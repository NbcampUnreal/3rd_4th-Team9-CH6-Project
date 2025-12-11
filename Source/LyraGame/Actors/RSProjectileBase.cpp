//#include "RSProjectileBase.h"
//
//#include "AbilitySystemComponent.h"
//#include "AbilitySystemGlobals.h"
//#include "RSGameplayTags.h"
//#include "RSEquipmentBase.h"
//#include "NiagaraComponent.h"
//#include "NiagaraFunctionLibrary.h"
//#include "Character/LyraCharacter.h"
//#include "Components/SphereComponent.h"
//#include "GameFramework/ProjectileMovementComponent.h"
//#include "Kismet/KismetMathLibrary.h"
//#include "System/LyraAssetManager.h"
//#include "System/LyraGameData.h"
//
//#include UE_INLINE_GENERATED_CPP_BY_NAME(RSProjectileBase)
//
//ARSProjectileBase::ARSProjectileBase(const FObjectInitializer& ObjectInitializer)
//	: Super(ObjectInitializer)
//{
//	PrimaryActorTick.bCanEverTick = false;
//	AActor::SetReplicateMovement(true);
//	bNetLoadOnClient = false;
//	bReplicates = true;
//
//	AActor::SetLifeSpan(5.f);
//	
//	SphereCollisionComponent = CreateDefaultSubobject<USphereComponent>("SphereCollisionComponent");
//	SphereCollisionComponent->SetCollisionProfileName("Projectile");
//	SphereCollisionComponent->SetupAttachment(GetRootComponent());
//	SphereCollisionComponent->bReturnMaterialOnMove = true;
//	SphereCollisionComponent->SetCanEverAffectNavigation(false);
//	SetRootComponent(SphereCollisionComponent);
//
//	ProjectileMeshComponent = CreateDefaultSubobject<UStaticMeshComponent>("ProjectileMeshComponent");
//	ProjectileMeshComponent->SetCollisionProfileName("NoCollision");
//	ProjectileMeshComponent->SetupAttachment(SphereCollisionComponent);
//	ProjectileMeshComponent->SetCanEverAffectNavigation(false);
//	
//	TrailNiagaraComponent = CreateDefaultSubobject<UNiagaraComponent>("TrailNiagaraComponent");
//	TrailNiagaraComponent->SetupAttachment(ProjectileMeshComponent);
//	
//    ProjectileMovementComponent = CreateDefaultSubobject<UProjectileMovementComponent>("ProjectileMovementComponent");
//	ProjectileMovementComponent->bRotationFollowsVelocity = true;
//	ProjectileMovementComponent->bInterpMovement = true;
//	ProjectileMovementComponent->bInterpRotation = true;
//}
//
//void ARSProjectileBase::BeginPlay()
//{
//	Super::BeginPlay();
//
//	SphereCollisionComponent->IgnoreActorWhenMoving(GetInstigator(), true);
//
//	switch (CollisionDetectionType)
//	{
//	case ECollisionDetectionType::Hit:
//		SphereCollisionComponent->SetGenerateOverlapEvents(false);
//		SphereCollisionComponent->OnComponentHit.AddUniqueDynamic(this, &ThisClass::HandleComponentHit);
//		break;
//	case ECollisionDetectionType::Overlap:
//		SphereCollisionComponent->SetGenerateOverlapEvents(true);
//		SphereCollisionComponent->OnComponentBeginOverlap.AddUniqueDynamic(this, &ThisClass::HandleComponentOverlap);
//		break;
//	}
//}
//
//void ARSProjectileBase::Destroyed()
//{
//	if (HitActors.Num() <= 0 && HitEffect)
//	{
//		UNiagaraFunctionLibrary::SpawnSystemAtLocation(this, HitEffect, GetActorLocation());
//	}
//	
//	if (HasAuthority() && bAttachToHitComponent && AttachingComponent.IsValid())
//	{
//		AttachingComponent->OnComponentDeactivated.RemoveDynamic(this, &ThisClass::HandleOtherComponentDeactivated);
//		AttachingComponent = nullptr;
//	}
//	
//	Super::Destroyed();
//}
//
//void ARSProjectileBase::SetSpeed(float Speed)
//{
//	ProjectileMovementComponent->Velocity = ProjectileMovementComponent->Velocity.GetSafeNormal() * Speed;
//}
//
//void ARSProjectileBase::HandleComponentHit(UPrimitiveComponent* HitComponent, AActor* OtherActor, UPrimitiveComponent* OtherComponent, FVector NormalImpulse, const FHitResult& HitResult)
//{
//	if (HitActors.Num() > 0)
//		return;
//	
//	HitActors.Add(OtherActor);
//
//	SphereCollisionComponent->Deactivate();
//	TrailNiagaraComponent->Deactivate();
//	ProjectileMovementComponent->Deactivate();
//	
//	if (HasAuthority())
//	{
//		if (bAttachToHitComponent)
//		{
//			AttachingComponent = OtherComponent;
//			OtherComponent->OnComponentDeactivated.AddUniqueDynamic(this, &ThisClass::HandleOtherComponentDeactivated);
//			AttachToComponent(OtherComponent, FAttachmentTransformRules::KeepWorldTransform, HitResult.BoneName);
//		}
//		else
//		{
//			SetLifeSpan(2.f);
//		}
//	}
//	
//	HandleCollisionDetection(OtherActor, OtherComponent, HitResult);
//}
//
//void ARSProjectileBase::HandleComponentOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComponent, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
//{
//	if (HitActors.Contains(OtherActor))
//		return;
//
//	HitActors.Add(OtherActor);
//	
//	FHitResult HitResult = SweepResult;
//	HitResult.bBlockingHit = bFromSweep;
//	HandleCollisionDetection(OtherActor, OtherComponent, HitResult);
//}
//
//void ARSProjectileBase::HandleOtherComponentDeactivated(UActorComponent* OtherComponent)
//{
//	if (HasAuthority())
//	{
//		Destroy();
//	}
//}
//
//void ARSProjectileBase::HandleCollisionDetection(AActor* OtherActor, UPrimitiveComponent* OtherComponent, const FHitResult& HitResult)
//{
//	if (OtherActor == nullptr || OtherComponent == nullptr)
//		return;
//	
//	if (HasAuthority())
//	{
//		ARSEquipmentBase* TargetWeapon = Cast<ARSEquipmentBase>(OtherActor);
//		ALyraCharacter* TargetCharacter = Cast<ALyraCharacter>(OtherActor);
//		if (TargetCharacter == nullptr)
//		{
//			TargetCharacter = Cast<ALyraCharacter>(OtherActor->GetOwner());
//			OtherActor = TargetCharacter;
//		}
//		
//		bool bBlockingHit = false;
//
//		UAbilitySystemComponent* TargetASC = UAbilitySystemGlobals::GetAbilitySystemComponentFromActor(OtherActor);
//		UAbilitySystemComponent* SourceASC = UAbilitySystemGlobals::GetAbilitySystemComponentFromActor(GetInstigator());
//
//		if (TargetWeapon)
//		{
//			bBlockingHit = true;
//		}
//		else if (TargetCharacter)
//		{
//			if (TargetASC && TargetASC->HasMatchingGameplayTag(RSGameplayTags::Status_Block))
//			{
//				FVector TargetLocation = OtherActor->GetActorLocation();
//				FVector TargetDirection = OtherActor->GetActorForwardVector();
//							
//				FVector InstigatorLocation = GetInstigator()->GetActorLocation();
//				FVector TargetToInstigator = InstigatorLocation - TargetLocation;
//					
//				float Degree = UKismetMathLibrary::DegAcos(TargetDirection.Dot(TargetToInstigator.GetSafeNormal()));
//				if (Degree <= 45.f)
//				{
//					bBlockingHit = true;
//				}
//			}
//		}
//		
//		if (SourceASC && HitGameplayCueTag.IsValid() && HitResult.bBlockingHit)
//		{
//			FGameplayCueParameters SourceCueParams;
//			SourceCueParams.Location = HitResult.ImpactPoint;
//			SourceCueParams.Normal = HitResult.ImpactNormal;
//			SourceCueParams.PhysicalMaterial = bBlockingHit ? nullptr : HitResult.PhysMaterial;
//			SourceASC->ExecuteGameplayCue(HitGameplayCueTag, SourceCueParams);
//		}
//
//		if (TargetASC && GetOwner() != OtherActor && GetOwner() != OtherComponent->GetOwner() && GetInstigator() != OtherActor)
//		{
//			const TSubclassOf<UGameplayEffect> DamageGEClass = ULyraAssetManager::GetSubclassByPath(ULyraGameData::Get().DamageGameplayEffect_SetByCaller);
//
//			FHitResult HitResultCopy = HitResult;
//			HitResultCopy.bBlockingHit = bBlockingHit;
//			
//			FGameplayEffectContextHandle EffectContextHandle = SourceASC->MakeEffectContext();
//			EffectContextHandle.AddHitResult(HitResultCopy);
//			EffectContextHandle.AddInstigator(SourceASC->AbilityActorInfo->OwnerActor.Get(), this);
//
//			FGameplayEffectSpecHandle EffectSpecHandle = SourceASC->MakeOutgoingSpec(DamageGEClass, 1.f, EffectContextHandle);
//			EffectSpecHandle.Data->SetSetByCallerMagnitude(RSGameplayTags::SetByCaller_BaseDamage, bBlockingHit ? Damage / 3.f : Damage);
//			TargetASC->ApplyGameplayEffectSpecToSelf(*EffectSpecHandle.Data.Get());
//		}
//	}
//	else
//	{
//		if (HitEffect)
//		{
//			UNiagaraFunctionLibrary::SpawnSystemAtLocation(this, HitEffect, HitResult.ImpactPoint);
//		}
//	}
//}
