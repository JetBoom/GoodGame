#include "GoodGame.h"
#include "CharacterMovementComponentEx.h"
#include "Repulsor.h"


ARepulsor::ARepulsor()
{
	PrimaryActorTick.bCanEverTick = true;

	//InitialLifeSpan = 1.f;
	
	Collider = CreateDefaultSubobject<USphereComponent>(TEXT("Collider"));
	Collider->SetEnableGravity(false);
	Collider->SetNotifyRigidBodyCollision(true);
	Collider->SetCanEverAffectNavigation(false);
	Collider->InitSphereRadius(128.f);
	Collider->SetCollisionResponseToAllChannels(ECollisionResponse::ECR_Ignore);
	Collider->SetCollisionResponseToChannel(ECollisionChannel::ECC_Pawn, ECollisionResponse::ECR_Overlap);
	Collider->bGenerateOverlapEvents = true;
	RootComponent = Collider;

	bAffectsOwner = true;
}

void ARepulsor::BeginPlay()
{
	Super::BeginPlay();

	Collider->OnComponentBeginOverlap.AddDynamic(this, &ARepulsor::OnOverlapBegin);
	//Collider->OnComponentEndOverlap.AddDynamic(this, &ARepulsor::OnOverlapEnd);
}

void ARepulsor::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	if (bOnlyOnBeginOverlap)
		return;

	TArray<AActor*> actors;
	Collider->GetOverlappingActors(actors, ACharacter::StaticClass());

	ACharacter* character;

	const float force = Force * DeltaTime;

	UCharacterMovementComponent* movement;
	for (auto& a : actors) {
		if (!bAffectsOwner && a == GetOwner())
			continue;

		character = Cast<ACharacter>(a);
		if (character) {
			movement = character->GetCharacterMovement();

			if (RepulsorType == ERepulsorType::Radial) {
				if (movement->IsMovingOnGround())
					movement->SetMovementMode(MOVE_Falling);
				movement->AddRadialImpulse(GetActorLocation(), Collider->GetScaledSphereRadius(), force, ERadialImpulseFalloff::RIF_Constant, true);
			}
			else {
				FVector dir = GetActorRotation().Vector();
				if (dir.Z < MinFlowZ) {
					dir.Z = MinFlowZ;
					dir.Normalize();
				}

				/*if (dir.Z > 0.f && movement->MovementMode == EMovementMode::MOVE_Walking)
					movement->SetMovementMode(EMovementMode::MOVE_Falling);*/
				movement->AddImpulse(dir * force, true);
			}
		}
	}
}

void ARepulsor::OnOverlapBegin(UPrimitiveComponent* OverlappedComp, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	if (!OtherActor || !bAffectsOwner && OtherActor == GetOwner())
		return;

	auto character = Cast<AGoodGameCharacter>(OtherActor);
	if (character) {
		UCharacterMovementComponentEx* movement = (UCharacterMovementComponentEx*)character->GetCharacterMovement();
		//movement->RepulsorCount++;

		if (RepulsorType == ERepulsorType::FlowThrough && movement->IsMovingOnGround()) {
			movement->Velocity.Z = 400.f;
			movement->SetMovementMode(MOVE_Falling);
		}

		if (bOnlyOnBeginOverlap) {
			if (movement->IsMovingOnGround())
				movement->SetMovementMode(MOVE_Falling);
			movement->AddRadialImpulse(GetActorLocation(), Collider->GetScaledSphereRadius(), Force, ERadialImpulseFalloff::RIF_Constant, true);
		}
	}
}

void ARepulsor::OnOverlapEnd(UPrimitiveComponent* OverlappedComp, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex)
{
	/*if (!OtherActor || !bAffectsOwner && OtherActor == GetOwner())
		return;

	auto character = Cast<AGoodGameCharacter>(OtherActor);
	if (character) {
		UCharacterMovementComponentEx* movement = (UCharacterMovementComponentEx*)character->GetCharacterMovement();
		//movement->RepulsorCount--;
	}*/
}
