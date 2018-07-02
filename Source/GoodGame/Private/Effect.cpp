#include "GoodGame.h"
#include "Effect.h"


AEffect::AEffect()
{
	PrimaryActorTick.bCanEverTick = false;

	InitialLifeSpan = 5.f;
	bCanBeDamaged = false;
	bReplicateMovement = false;
	NetPriority = 1.f;
	bNetTemporary = true;
}

void AEffect::PostNetInit()
{
	Super::PostNetInit();

	bTearOff = true;
	SetLifeSpan(InitialLifeSpan);
}
