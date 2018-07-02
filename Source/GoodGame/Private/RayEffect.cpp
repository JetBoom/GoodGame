#include "GoodGame.h"
#include "UnrealNetwork.h"
#include "RayEffect.h"


ARayEffect::ARayEffect()
{
	RootComponent = CreateDefaultSubobject<USceneComponent>(TEXT("Root"));
	RootComponent->SetMobility(EComponentMobility::Movable);
	
	ParticleSystem = CreateDefaultSubobject<UParticleSystemComponent>(TEXT("RayParticleSystem"));
	ParticleSystem->bAutoActivate = false;
	ParticleSystem->SetupAttachment(RootComponent);

	ParticleSystem->InstanceParameters.SetNum(3, false);

	FParticleSysParam param;
	param.ParamType = EParticleSysParamType::PSPT_Vector;
	param.Name = FName("EndOffset");
	ParticleSystem->InstanceParameters[0] = param;

	param = FParticleSysParam();
	param.ParamType = EParticleSysParamType::PSPT_Scalar;
	param.Name = FName("RayDistance");
	ParticleSystem->InstanceParameters[1] = param;

	param = FParticleSysParam();
	param.ParamType = EParticleSysParamType::PSPT_Scalar;
	param.Name = FName("RayDistanceDouble");
	ParticleSystem->InstanceParameters[2] = param;
}

/*void ARayEffect::BeginPlay()
{
	Super::BeginPlay();
}

void ARayEffect::EndPlay(const EEndPlayReason::Type EndPlayReason)
{
	Super::EndPlay(EndPlayReason);
}*/

void ARayEffect::UpdateParticleParameters()
{
	ParticleSystem->SetFloatParameter(FName("RayDistance"), RayDistance);
	ParticleSystem->SetFloatParameter(FName("RayDistanceDouble"), RayDistance * 2);
	ParticleSystem->SetVectorParameter(FName("EndOffset"), FVector(RayDistance, 0.f, 0.f));
	if (!ParticleSystem->bIsActive)
		ParticleSystem->SetActive(true, true);

	//DrawDebugLine(GetWorld(), GetActorLocation(), GetActorLocation() + GetActorForwardVector() * RayDistance, FColor::Red, false, 4.f, 0, 3.f);
}

void ARayEffect::GetLifetimeReplicatedProps(TArray< FLifetimeProperty > & OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME_CONDITION(ARayEffect, RayDistance, COND_InitialOnly);
}
