#include "Gimmick/RSWeaponBox.h"

#include "AbilitySystemBlueprintLibrary.h"


void ARSWeaponBox::NotifyActorBeginOverlap(AActor* Other)
{
	Super::NotifyActorBeginOverlap(Other);

	UAbilitySystemBlueprintLibrary::SendGameplayEventToActor(Other, WeaponEventTag, FGameplayEventData());
}
