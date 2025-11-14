#include "Game/WorldObjects/Devices/CVDevice_HpPotion.h"
#include "Kismet/GameplayStatics.h"


void ACVDevice_HpPotion::OnItemOverlap(UPrimitiveComponent* OverlappedComp, AActor* OtherActor,
	UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	if (OtherActor && OtherActor != this)
	{
		UGameplayStatics::SpawnEmitterAtLocation(GetWorld(), nullptr, GetActorLocation()); // Èú VFX
		UGameplayStatics::PlaySoundAtLocation(GetWorld(), nullptr, GetActorLocation());   // Èú Sound
		Destroy();
	}
}