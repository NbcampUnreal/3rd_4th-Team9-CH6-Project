// Source/ChronoVeil/Game/AbilitySystem/Abilities/Movement/CVGA_Move_Flash.cpp

#include "Game/AbilitySystem/Abilities/Movement/CVGA_Move_Flash.h"
#include "Game/Character/CVCharacter.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "DrawDebugHelpers.h"

void UCVGA_Move_Flash::ActivateAbility(
	const FGameplayAbilitySpecHandle Handle,
	const FGameplayAbilityActorInfo* ActorInfo,
	const FGameplayAbilityActivationInfo ActivationInfo,
	const FGameplayEventData* TriggerEventData)
{
	if (!CommitAbility(Handle, ActorInfo, ActivationInfo))
	{
		EndAbility(Handle, ActorInfo, ActivationInfo, true, true);
		return;
	}

	ACVCharacter* Character = nullptr;
	if (ActorInfo && ActorInfo->AvatarActor.IsValid())
	{
		Character = Cast<ACVCharacter>(ActorInfo->AvatarActor.Get());
	}

	if (Character)
	{
		UWorld* World = Character->GetWorld();
		if (World)
		{
			const FVector Start = Character->GetActorLocation();
			const FRotator ControlRot(0.f, Character->GetControlRotation().Yaw, 0.f);
			const FVector Forward = ControlRot.Vector();

			const FVector End = Start + Forward * MaxFlashDistance;

			FHitResult Hit;
			FCollisionQueryParams Params(SCENE_QUERY_STAT(FlashTrace), false, Character);

			FVector TargetLocation = End;

			// 앞에 벽이 있으면 그 바로 앞까지만
			if (World->LineTraceSingleByChannel(Hit, Start, End, ECC_Visibility, Params))
			{
				// 약간 뒤로 밀어줘서 벽 안에 박히지 않게
				TargetLocation = Hit.Location - Forward * 50.f;
			}

			// Z는 캡슐 절반높이를 고려해 바닥에 박히지 않도록 할 수도 있음
			TargetLocation.Z = Start.Z + TeleportHeightOffset;

			// 텔레포트 (Collision 고려)
			Character->TeleportTo(TargetLocation, Character->GetActorRotation(), false, true);

#if WITH_EDITOR
			// 디버그 확인용
			// DrawDebugLine(World, Start, TargetLocation, FColor::Cyan, false, 1.f, 0, 2.f);
#endif
		}
	}

	EndAbility(Handle, ActorInfo, ActivationInfo, true, false);
}
