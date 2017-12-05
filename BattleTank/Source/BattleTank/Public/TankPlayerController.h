// Copyright EmbraceIT Ltd

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/PlayerController.h"
#include "TankPlayerController.generated.h" // Must be the last include

//class UTankAimingComponent; // No need to add this because of the BlueprintImplementableEvent FoundAimingComponent below

/**
 * Responsible for helping the player aim
 */
UCLASS()
class BATTLETANK_API ATankPlayerController : public APlayerController
{
	GENERATED_BODY()

protected:
	UFUNCTION(BlueprintImplementableEvent, Category = "Setup")
	void FoundAimingComponent(UTankAimingComponent* AimCompRef); // With the above UFunction type, no need to define this method

	UFUNCTION(BlueprintCallable, Category = "Setup")
	void OnPossessedTankDeath();

private:
	virtual void BeginPlay() override;

	virtual void SetPawn(APawn* InPawn) override; // called when gets possessed

	virtual void Tick(float DeltaTime) override;

	UPROPERTY(EditDefaultsOnly)
	float CrossHairXLocation = 0.5;

	UPROPERTY(EditDefaultsOnly)
	float CrossHairYLocation = 0.33333;

	float LineTraceRange; // Gets set to TankAimingComponent's MaxShootingRange in BeginPlay()
	
	void AimTowardsCrosshair();

	bool GetSightRayHitLocation(FVector& OutHitLocation) const;

	/// Alternative method using APlayerController::GetHitResultAtScreenPosition instead of GetWorld->LineTraceSingleByChannel
	/// to obtain HitLocation directly, without helper methods GetLookDirection and GetLookVectorHitLocation
	bool GetSightRayHitLocation(FVector& OutHitLocation, FString& OutObjectHit) const;

	bool GetLookDirection(FVector2D ScreenLocation, FVector& OutLookDirection) const;

	bool GetLookVectorHitLocation(FVector LookDirection, FVector& OutHitLocation) const;


	
};
