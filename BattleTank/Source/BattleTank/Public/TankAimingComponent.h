// Copyright EmbraceIT Ltd

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "TankAimingComponent.generated.h"

class AProjectile;


UENUM()
enum class EFiringState : uint8
{
	Reloading,
	Aiming,
	Locked,
	OutOfAmmo
};

// forward declaration instead - but not same as - include, more efficient compilation, works for classes referenced as pointers,
// like in headers, but if methods are called, a full include is necessary (like in TankAimingComponent.cpp)
class UTankBarrel;
class UTankTurret;

// Holds barrel's properties and Elevate method
UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class BATTLETANK_API UTankAimingComponent : public UActorComponent
{
	GENERATED_BODY()

public:	
	UFUNCTION(BlueprintCallable, Category = "Setup")
	void Initialize(UTankBarrel* BarrelToSet, UTankTurret* TurretToSet);

	UFUNCTION(BlueprintCallable, Category = "Firing")
	void Fire();

	void AimAt(FVector HitLocation);

	EFiringState GetFiringState() const;

	UFUNCTION(BlueprintCallable, Category = "Firing")
	int32 GetRoundsLeft() const;


protected: 
	// to be accessible to a subclass in blueprint, can't be private, and has to be declared as UPROPERTY
	UPROPERTY(BlueprintReadOnly, Category = "State")
	EFiringState FiringState = EFiringState::Reloading;

private:	
	// Sets default values for this component's properties
	UTankAimingComponent();

	virtual void BeginPlay() override;

	virtual void TickComponent(float DeltaTime, enum ELevelTick TickType, FActorComponentTickFunction *ThisTickFunction) override;

	void MoveBarrelTowards(FVector AimDirection);

	bool IsBarrelMoving();

	FVector AimDirection;

	UPROPERTY(EditDefaultsOnly, Category = "Setup")
	TSubclassOf<AProjectile> ProjectileBlueprint;

	UPROPERTY(EditDefaultsOnly, Category = "Firing")
	float LaunchSpeed = 8000; // 80 m/s or 288 km/h

	UPROPERTY(EditDefaultsOnly, Category = "Firing")
	float ReloadTimeInSeconds =3;

	UPROPERTY(EditDefaultsOnly, Category = "Firing")
	int32 RoundsLeft = 20;

	double LastFireTime = 0;

	UTankBarrel* Barrel = nullptr;
	UTankTurret* Turret = nullptr;



		
	
};
