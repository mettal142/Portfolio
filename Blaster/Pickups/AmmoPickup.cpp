#include "AmmoPickup.h"
#include "Blaster/Character/BlasterCharacter.h"
#include "Blaster/BlasterComponents/CombatComponent.h"

void AAmmoPickup::OnSphereOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, 
	UPrimitiveComponent* OtherComponent, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	Super::OnSphereOverlap(OverlappedComponent, OtherActor, OtherComponent, OtherBodyIndex, bFromSweep, SweepResult);

	ABlasterCharacter* BlasterCharacter = Cast < ABlasterCharacter>(OtherActor);
	if (BlasterCharacter)
	{
		UCombatComponent* Combat = BlasterCharacter->GetCombat();
		if (Combat)
		{
			Combat->PickupAmmo(WeaponType, AmmoAmount);
		}
	}
	Destroy();

}
