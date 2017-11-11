// Copyright EmbraceIT Ltd

#include "Tank.h"
#include "TankAimingComponent.h"
#include "TankBarrel.h"
#include "Projectile.h"
//#include "Engine/World.h"


// Sets default values
ATank::ATank()
{
 	// Set this pawn to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	//// No need to protect pointers when added at construction
	//// Alternatively, spawn an instance in the editor directly by making the class blueprintspawnable, 
	//TankAimingComponent = CreateDefaultSubobject<UTankAimingComponent>(FName("Aiming Component"));
	auto TankName = GetName();
	UE_LOG(LogTemp, Warning, TEXT("%s DONKEY: Tank C++ Construct"), *TankName)
}

void ATank::BeginPlay()
{
	Super::BeginPlay(); // Needed for BP BeginPlay to run! Else, just delete the whole method so it won't override...
}

void ATank::AimAt(FVector HitLocation)
{
	// SUPER IMPORTANT, without it, will crash now that this component is not added in the constructor as a subobject, but spawned in blueprint
	if (!TankAimingComponent) { return; }
	TankAimingComponent->AimAt(HitLocation, LaunchSpeed);

}

void ATank::Fire()
{
	//bool IsReloaded = (FPlatformTime::Seconds() - LastFireTime) > ReloadTimeInSeconds;
	// if (Barrel && IsReloaded)

	if (!Barrel) { return; } // pointer protected separately maybe better than above version?
	if((FPlatformTime::Seconds() - LastFireTime) > ReloadTimeInSeconds)
	{
		// Spawn a projectile at the socket location on the barrel
		auto Projectile = GetWorld()->SpawnActor<AProjectile>(
			ProjectileBlueprint,
			Barrel->GetSocketLocation("Projectile"),
			Barrel->GetSocketRotation("Projectile")
			);

		Projectile->LaunchProjectile(LaunchSpeed);
		LastFireTime = FPlatformTime::Seconds();
	}
		

}


