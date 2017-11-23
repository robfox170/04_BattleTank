// Fill out your copyright notice in the Description page of Project Settings.

#include "Projectile.h"
#include "GameFramework/ProjectileMovementComponent.h"
#include "Particles/ParticleSystemComponent.h"
#include "PhysicsEngine/RadialForceComponent.h"
#include "Engine/World.h"
#include "TimerManager.h"



// Sets default values
AProjectile::AProjectile()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = false;

	CollisionMesh = CreateDefaultSubobject<UStaticMeshComponent>(FName("Collision Mesh"));
	// Set sensible defaults for the parameters exposed in blueprint
	SetRootComponent(CollisionMesh); // Optional, already set by default, but now explicitly
	CollisionMesh->SetNotifyRigidBodyCollision(true); // Corresponds to Collision parameter "Simulation Generates Hit Events" set to true by default
	CollisionMesh->SetVisibility(false);

	LaunchBlast = CreateDefaultSubobject<UParticleSystemComponent>(FName("Launch Blast"));
	LaunchBlast->AttachToComponent(RootComponent, FAttachmentTransformRules::KeepRelativeTransform);

	ImpactBlast = CreateDefaultSubobject<UParticleSystemComponent>(FName("Impact Blast"));
	ImpactBlast->AttachToComponent(RootComponent, FAttachmentTransformRules::KeepRelativeTransform);
	ImpactBlast->bAutoActivate = false;

	ExplosionForce = CreateDefaultSubobject<URadialForceComponent>(FName("Explosion Force"));
	// IMPORTANT, looks attached in Projectile_BP, but is by default attached to World root (see its transform details when game paused)
	// hence, although usually components are attached correctly by default, better make it explicit here
	ExplosionForce->AttachToComponent(RootComponent, FAttachmentTransformRules::KeepRelativeTransform); 

	ProjectileMovement = CreateDefaultSubobject<UProjectileMovementComponent>(FName("Projectile Movement"));
	ProjectileMovement->bAutoActivate = false;
}

// Called when the game starts or when spawned
void AProjectile::BeginPlay()
{
	Super::BeginPlay();
	
	// Projectile is an actor, so the delegate method OnHit for the hit event must be registered with the collision mesh
	CollisionMesh->OnComponentHit.AddDynamic(this, &AProjectile::OnHit); 
}


void AProjectile::LaunchProjectile(float Speed)
{
	ProjectileMovement->SetVelocityInLocalSpace(FVector::ForwardVector * Speed);
	ProjectileMovement->Activate();

}

void AProjectile::OnHit(UPrimitiveComponent* HitComponent, AActor* OtherActor, UPrimitiveComponent* OtherComponent, FVector NormalImpulse, const FHitResult& Hit)
{
	LaunchBlast->Deactivate();
	//FString bWasDeactivated = LaunchBlast->bWasDeactivated ? "True" : "False";
	//UE_LOG(LogTemp, Warning, TEXT("LaunchBlast was deactivated: %s"), *bWasDeactivated)
	ImpactBlast->Activate();
	ExplosionForce->FireImpulse();

	// Set the ImpactBlast component as root in order to destroy the Projectile mesh on impact and avoid it lingering in the world
	SetRootComponent(ImpactBlast);
	CollisionMesh->DestroyComponent();

	FTimerHandle Timer; // not used, but needed as out parameter
	GetWorld()->GetTimerManager().SetTimer(Timer, this, &AProjectile::OnTimerExpire, DestroyDelay, false);

}

void AProjectile::OnTimerExpire()
{
	// Destroy the projectile, not just the Collision mesh in OnHit(), else it is using memory for nothing
	Destroy();
}


