// Fill out your copyright notice in the Description page of Project Settings.

#include "TankAIController.h"
#include "Tank.h"


void ATankAIController::BeginPlay()
{
	Super::BeginPlay();

}

void ATankAIController::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	auto ControlledTank = Cast<ATank>(GetPawn());
	auto PlayerTank = Cast<ATank>(GetWorld()->GetFirstPlayerController()->GetPawn());

	if (!PlayerTank || !ControlledTank) { return; }
	
	MoveToActor(PlayerTank, AcceptanceRadius); // calls RequestDirectMove() in the TankMovementComponent

	ControlledTank->AimAt(PlayerTank->GetActorLocation());

	ControlledTank->Fire();
	
}


