#include "AmmoPickup.h"
#include "TPS/Character/TPSCharacter.h"
#include "TPS/TPSComponents/CombatComponent.h"

void AAmmoPickup::OnSphereOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, 
	UPrimitiveComponent* OtherComponent, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	Super::OnSphereOverlap(OverlappedComponent, OtherActor, OtherComponent, OtherBodyIndex, bFromSweep, SweepResult);

	ATPSCharacter* TPSCharacter = Cast < ATPSCharacter>(OtherActor);
	if (TPSCharacter)
	{
		UCombatComponent* Combat = TPSCharacter->GetCombat();
		if (Combat)
		{
			Combat->PickupAmmo(WeaponType, AmmoAmount);
		}
	}
	Destroy();

}
