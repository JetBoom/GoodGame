#include "GoodGame.h"
#include "AffectorArea.h"


AAffectorArea::AAffectorArea()
{
	PrimaryActorTick.bCanEverTick = true;
	PrimaryActorTick.bStartWithTickEnabled = true;
	PrimaryActorTick.bAllowTickOnDedicatedServer = true;
	PrimaryActorTick.TickInterval = 0.f;

	Collider = CreateDefaultSubobject<USphereComponent>(TEXT("Collider"));
	Collider->SetEnableGravity(false);
	Collider->SetNotifyRigidBodyCollision(true);
	Collider->SetCanEverAffectNavigation(false);
	Collider->InitSphereRadius(128.f);
	Collider->SetCollisionResponseToAllChannels(ECollisionResponse::ECR_Ignore);
	Collider->SetCollisionResponseToChannel(ECollisionChannel::ECC_Pawn, ECollisionResponse::ECR_Overlap);
	Collider->bGenerateOverlapEvents = true;
	RootComponent = Collider;
}

void AAffectorArea::SetRadius(const float radius)
{
	Collider->SetSphereRadius(radius);
}

void AAffectorArea::BeginPlay()
{
	Super::BeginPlay();

	Collider->OnComponentBeginOverlap.AddDynamic(this, &AAffectorArea::OnOverlapBegin);
}

void AAffectorArea::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	if (bOnlyOnBeginOverlap)
		return;

	TArray<AActor*> actors;
	Collider->GetOverlappingActors(actors);

	const float curTime = GetWorld()->TimeSeconds;
	const FVector location = GetActorLocation();

	for (AActor* actor : actors) {
		if (NextAffect.Contains(actor) && curTime >= NextAffect[actor]) {
			const UPrimitiveComponent* component = actor->FindComponentByClass<UPrimitiveComponent>();
			if (!component || UGGStatics::IsVisible(location, component, actor)) {
				NextAffect[actor] = curTime + AffectorDelay;
				DoAffect(actor);
			}
		}
	}
}

void AAffectorArea::OnOverlapBegin(UPrimitiveComponent* OverlappedComp, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	if (OtherActor && PassesFilter(OtherActor) && (bAffectsOwner || OtherActor != GetOwner())) {
		if (!NextAffect.Contains(OtherActor))
			NextAffect.Add(OtherActor, 0.f);

		if (bOnlyOnBeginOverlap)
			DoAffect(OtherActor);
	}
}

bool AAffectorArea::PassesFilter(const AActor* actor) const
{
	return actor->IsA<APawn>();
}

void AAffectorArea::DoAffect(AActor* actor) {}
void AAffectorArea::UpdateFromProjectile(const AProjectile* projectile) {}
