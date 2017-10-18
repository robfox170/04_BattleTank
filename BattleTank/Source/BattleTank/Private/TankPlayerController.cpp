// Fill out your copyright notice in the Description page of Project Settings.

#include "TankPlayerController.h"
#include "Engine/World.h"

void ATankPlayerController::BeginPlay()
{
	Super::BeginPlay();

	auto ControlledTank = GetControlledTank();
	if (!ControlledTank)
	{
		UE_LOG(LogTemp, Warning, TEXT("PlayerController not possessing a tank"));
	}
	else
	{
		UE_LOG(LogTemp, Warning, TEXT("PlayerController possessing: %s"), *(ControlledTank->GetName()));
	}
}

void ATankPlayerController::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
	AimTowardsCrosshair();
}


ATank* ATankPlayerController::GetControlledTank() const
{
	// no need to protect this pointer with if(!...)...
	//because the cast will fail if it's a null or invalid pointer
	return Cast<ATank>(GetPawn());
}

void ATankPlayerController::AimTowardsCrosshair()
{
	if (!GetControlledTank()) { return; }

	FVector HitLocation; // Out Parameter
	if (GetSightRayHitLocation(HitLocation))
	{
		GetControlledTank()->AimAt(HitLocation);
	}

	//// Method call for alternative GetSightRayHitLocation overload using GetHitResultAtScreenPosition...
	//FVector HitLocation = FVector(0);
	//FString ObjectHit = "Nothing";
	//GetSightRayHitLocation(HitLocation, ObjectHit);
	//UE_LOG(LogTemp, Warning, TEXT("Object in range: %s Location: %s"), *ObjectHit, *HitLocation.ToString());


}

/// Get world location of linetrace through crosshair, true if hits landscape
/// (Check out alternative method overload below for simplified code)
bool ATankPlayerController::GetSightRayHitLocation(FVector& HitLocation) const
{
	// Find the crosshair position in pixel coordinates
	int32 ViewportSizeX, ViewportSizeY;
	GetViewportSize(ViewportSizeX, ViewportSizeY);
	auto ScreenLocation = FVector2D(ViewportSizeX * CrossHairXLocation, ViewportSizeY * CrossHairYLocation);
	
	// De-project the screen position of the crosshair to a world direction (look direction)
	FVector LookDirection; // Out Parameter
	if (GetLookDirection(ScreenLocation, LookDirection))
	{
		// Line-trace along that look direction and see what we hit (up to max. range)
		GetLookVectorHitLocation(LookDirection, HitLocation);
	}
	return true;
}

/// Get world location of linetrace through crosshair, true if hits landscape
/// Alternative method using APlayerController::GetHitResultAtScreenPosition instead of GetWorld->LineTraceSingleByChannel
/// to obtain HitLocation directly, without helper methods GetLookDirection and GetLookVectorHitLocation
bool ATankPlayerController::GetSightRayHitLocation(FVector& HitLocation, FString& ObjectHit) const
{
	// Find the crosshair position in pixel coordinates
	int32 ViewportSizeX, ViewportSizeY;
	GetViewportSize(ViewportSizeX, ViewportSizeY);
	auto ScreenLocation = FVector2D(ViewportSizeX * CrossHairXLocation, ViewportSizeY * CrossHairYLocation);

	FHitResult HitResult;
	bool bHit = GetHitResultAtScreenPosition(
		ScreenLocation,
		ECollisionChannel::ECC_Visibility,
		false,
		HitResult
	);
	if (bHit && HitResult.Distance <= LineTraceRange)
	{
		HitLocation = HitResult.ImpactPoint;
		ObjectHit = HitResult.GetActor()->GetName();
	}
	return bHit;
}


bool ATankPlayerController::GetLookDirection(FVector2D ScreenLocation, FVector& LookDirection) const
{
	FVector CameraWorldLocation; // To be discarded
	return DeprojectScreenPositionToWorld(
		ScreenLocation.X, 
		ScreenLocation.Y, 
		CameraWorldLocation, 
		LookDirection
	);
}

bool ATankPlayerController::GetLookVectorHitLocation(FVector LookDirection, FVector& HitLocation) const
{
	FHitResult HitResult;
	auto StartLocation = PlayerCameraManager->GetCameraLocation();
	auto EndLocation = StartLocation + (LookDirection * LineTraceRange);
	if (GetWorld()->LineTraceSingleByChannel(
			HitResult,
			StartLocation,
			EndLocation,
			ECollisionChannel::ECC_Visibility
		)
	)
	{
		HitLocation = HitResult.Location;
		return true;
	}
	// Set default coordinates to 0 as a security in case the results are random when nothing is hit (sky for example), 
	// although in Unreal 4.17 it seems unnecessary...
	HitLocation = FVector(0); 
	return false; // Line trace didn't succeed
}



