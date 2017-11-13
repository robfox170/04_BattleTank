// Copyright EmbraceIT Ltd

#include "Tank.h"
#include "TankBarrel.h"
#include "Projectile.h"
//#include "Engine/World.h"


// Sets default values
ATank::ATank()
{
 	// Set this pawn to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;
}

void ATank::BeginPlay()
{
	Super::BeginPlay(); // Needed for BP BeginPlay to run! Else, just delete the whole method so it won't override...
}

void ATank::Fire()
{
	if (!ensure(Barrel)) { return; }
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


