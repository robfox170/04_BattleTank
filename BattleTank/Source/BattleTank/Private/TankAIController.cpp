// Fill out your copyright notice in the Description page of Project Settings.

#include "TankAIController.h"
#include "TankAimingComponent.h"
// also depends on MovementComponent via pathfinding system in MovetoActor()


void ATankAIController::BeginPlay()
{
	Super::BeginPlay();
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


