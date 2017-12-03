// Fill out your copyright notice in the Description page of Project Settings.

#include "TankAIController.h"
#include "TankAimingComponent.h"
#include "Tank.h"
//#include "Engine/World.h"
// also depends on MovementComponent via pathfinding system in MovetoActor()


void ATankAIController::BeginPlay()
{
	Super::BeginPlay();

	PlayerTank = Cast<ATank>(GetWorld()->GetFirstPlayerController()->GetPawn());
	if (!ensure(PlayerTank)) { return; }
}

void ATankAIController::SetPawn(APawn* InPawn) // called when gets possessed, so safe place to subscribe (see below). BeginPlay() may race (be too early)
{
	Super::SetPawn(InPawn);
	if (InPawn)
	{
		auto PossessedTank = Cast<ATank>(InPawn); // must be cast to ATank, because OnDeath is one of it's properties, not available - of course - on the inherited Pawn
		if (!ensure(PossessedTank)) { return; }

		// Subscribe our local method to the tank's death event
		PossessedTank->OnDeath.AddUniqueDynamic(this, &ATankAIController::OnPossessedTankDeath);
	}
}

void ATankAIController::OnPossessedTankDeath()
{
	if (!GetPawn()) {return;}// No ensure here to avoid an insertion firing at runtime
	GetPawn()->DetachFromControllerPendingDestroy();
}

void ATankAIController::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
	 
	if (PlayerTank->GetHealthPercent() > 0) // PlayerTank alive, seek and destroy it!
	{
		SeekAndDestroy();
	}
}

void ATankAIController::SeekAndDestroy()
{
	auto AimingComponent = GetPawn()->FindComponentByClass<UTankAimingComponent>();
	if (!ensure(AimingComponent)) { return; }

	// TODO: add a flag bSeekInSightOnly as an UPROPERTY option, to make it possible to remove this handicap for AI tanks, 
	// because the player has the advantage of positioning his tank according to the AI tanks' healthbars, 
	// while the AI tanks won't go after him if he remains out of sight
	// AI Tanks aim and move towards the playertank only...
	if (IsPlayerTankInSight()) 
	{
		MoveToActor(PlayerTank, AcceptanceRadius); // calls RequestDirectMove() in the TankMovementComponent

		AimingComponent->AimAt(PlayerTank->GetActorLocation());
	}

	// Fire only if within shooting range, using the same linetracing limit as the player tank
	auto ShootingDistance = FVector::Dist(GetPawn()->GetActorLocation(), PlayerTank->GetActorLocation());
	if (AimingComponent->GetFiringState() == EFiringState::Locked // TODO: put this statement in Fire() method directly, so that all tanks are equal
		&& ShootingDistance <= AimingComponent->GetMaxShootingRange()
		)
	{
		AimingComponent->Fire();
	}
}

bool ATankAIController::IsPlayerTankInSight()
{
	FHitResult HitResult;
	return GetWorld()->LineTraceSingleByChannel(
		HitResult,
		GetPawn()->GetActorLocation(),
		PlayerTank->GetActorLocation(),
		ECollisionChannel::ECC_Camera
	);
}


