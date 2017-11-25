// Fill out your copyright notice in the Description page of Project Settings.

#include "TankAIController.h"
#include "TankAimingComponent.h"
#include "Tank.h"
// also depends on MovementComponent via pathfinding system in MovetoActor()


void ATankAIController::BeginPlay()
{
	Super::BeginPlay();
}

void ATankAIController::SetPawn(APawn* InPawn) // called when gets possessed, so safe place to subscribe (see below). BeginPlay() may race (be too early)
{
	Super::SetPawn(InPawn);
	if (InPawn)
	{
		auto PossessedTank = Cast<ATank>(InPawn);
		if (!ensure(PossessedTank)) { return; }

		// Subscribe our local method to the tank's death event
		PossessedTank->OnDeath.AddUniqueDynamic(this, &ATankAIController::OnPossessedTankDeath);
	}
}

void ATankAIController::OnPossessedTankDeath()
{
	UE_LOG(LogTemp, Warning, TEXT("Tank died"))

}

void ATankAIController::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	auto PlayerTank = GetWorld()->GetFirstPlayerController()->GetPawn();
	auto ControlledTank = GetPawn();

	if (!ensure(PlayerTank && ControlledTank)) { return; } 

		MoveToActor(PlayerTank, AcceptanceRadius); // calls RequestDirectMove() in the TankMovementComponent

		auto AimingComponent = GetPawn()->FindComponentByClass<UTankAimingComponent>();

		AimingComponent->AimAt(PlayerTank->GetActorLocation());

		if (AimingComponent->GetFiringState() == EFiringState::Locked) // TODO: put this statement in Fire() method directly, so that all tanks are equal
		{
			AimingComponent->Fire();

		}
}


