// Copyright EmbraceIT Ltd

#include "TankPlayerController.h"
#include "TankAimingComponent.h"
#include "Tank.h"
#include "CollisionQueryParams.h"


void ATankPlayerController::BeginPlay()
{
	Super::BeginPlay();
	// TODO: sort out what needs to be protected, the Pawn, the AimingComponent, in BeginPlay too or only in AimTowardsCrosshair?
	//if (!ensure(GetPawn())) { return; } 
	auto AimingComponent = GetPawn()->FindComponentByClass<UTankAimingComponent>();
	//if (!ensure(AimingComponent)) { return; }
	FoundAimingComponent(AimingComponent);
	LineTraceRange = AimingComponent->GetMaxShootingRange();
}

void ATankPlayerController::SetPawn(APawn* InPawn) // called when gets possessed, so safe place to subscribe (see below). BeginPlay() may race (be too early)
{
	Super::SetPawn(InPawn);
	if (InPawn)
	{
		auto PossessedTank = Cast<ATank>(InPawn);
		if (!ensure(PossessedTank)) { return; }

		// Subscribe our local method to the tank's death event
		PossessedTank->OnDeath.AddUniqueDynamic(this, &ATankPlayerController::OnPossessedTankDeath);
	}
}

void ATankPlayerController::OnPossessedTankDeath()
{
	StartSpectatingOnly();
}

void ATankPlayerController::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
	AimTowardsCrosshair();
}

void ATankPlayerController::AimTowardsCrosshair()
{
	// protect the pawn pointer BEFORE it gets used to find the AimingComponent, 
	// else opening TankPlayerController_BP will crash the editor
	// and also if not possessing a tank(?)
	if (!GetPawn()) { return; }
	auto AimingComponent = GetPawn()->FindComponentByClass<UTankAimingComponent>();
	if (!ensure(AimingComponent)) { return; }

	FVector HitLocation; // Out Parameter
	bool bGotHitLocation = GetSightRayHitLocation(HitLocation);
	if (bGotHitLocation)
	{
		AimingComponent->AimAt(HitLocation);
	}
	//UE_LOG(LogTemp, Warning, TEXT("Hit location: %s"), *HitLocation.ToString());


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
		return GetLookVectorHitLocation(LookDirection, HitLocation);
	}
	return false;
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
		ECollisionChannel::ECC_Camera,
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

// TODO: find reason for trembling barrel when trying to aim too low and overlap with sight. 
bool ATankPlayerController::GetLookVectorHitLocation(FVector LookDirection, FVector& HitLocation) const
{
	FHitResult HitResult;
	auto StartLocation = PlayerCameraManager->GetCameraLocation();
	auto EndLocation = StartLocation + (LookDirection * LineTraceRange);
	FCollisionQueryParams Params;
	Params.AddIgnoredActor(GetPawn());
	if (GetWorld()->LineTraceSingleByChannel(
		HitResult,
		StartLocation,
		EndLocation,
		ECollisionChannel::ECC_Camera, 
		Params
		)
	)
	{
		HitLocation = HitResult.Location;
		//UE_LOG(LogTemp, Warning, TEXT("Hit Object: %s"), *(HitResult.GetActor()->GetName()))
		//if (HitResult.GetActor() == GetPawn())
		//{
		//	return false;
		//}
		//else 
		return true;
	}
	// Set default coordinates to 0 as a security in case the results are random when nothing is hit (sky for example), 
	// although in Unreal 4.17 it seems unnecessary...
	HitLocation = FVector(0); 
	return false; // Line trace didn't succeed
}



