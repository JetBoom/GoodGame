#include "GoodGame.h"
#include "AbilityTornado.h"


void AAbilityTornado::OnAbilityEnd_Implementation(const EStopCastingReason Reason)
{
	Super::OnAbilityEnd_Implementation(Reason);

	if (Reason == EStopCastingReason::Success && HasAuthority()) {
		AGoodGameCharacter* owner = Cast<AGoodGameCharacter>(GetOwner());
		if (owner) {
			FVector origin = owner->GetActorLocation();

			if (TornadoDamageType) {
				TArray<AActor*> filter;
				filter.Add(owner);

				UGameplayStatics::ApplyRadialDamage(GetWorld(), TornadoDamage, origin, TornadoRadius, TornadoDamageType, filter, this, owner->Controller, true, ECC_AOE);
			}

			if (TornadoRepulsor) {
				static FActorSpawnParameters spawnparams;
				spawnparams.Instigator = owner;
				spawnparams.Owner = owner;

				GetWorld()->SpawnActor<ARepulsor>(TornadoRepulsor, origin, FRotator(90.f, 0.f, 0.f), spawnparams);
			}
		}
	}
}
