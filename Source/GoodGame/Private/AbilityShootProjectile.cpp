#include "GoodGame.h"
#include "AbilityShootProjectile.h"


AAbilityShootProjectile::AAbilityShootProjectile() {}

void AAbilityShootProjectile::OnAbilityEnd_Implementation(const EStopCastingReason Reason)
{
	Super::OnAbilityEnd_Implementation(Reason);

	if (Reason == EStopCastingReason::Success && ProjectileClass && HasAuthority()) {
		AGoodGameCharacter* owner = Cast<AGoodGameCharacter>(GetOwner());
		if (owner) {
			static FActorSpawnParameters spawnparams;
			spawnparams.Instigator = owner;
			spawnparams.Owner = owner;

			FVector pos;
			FRotator ang;
			owner->GetShootTransform(pos, ang);

			GetWorld()->SpawnActor<AProjectile>(ProjectileClass, pos, ang, spawnparams);
		}
	}
}
