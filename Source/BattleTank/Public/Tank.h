// Copyright EmbraceIT Ltd

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Pawn.h"
#include "Tank.generated.h"
class AProjectile;


DECLARE_DYNAMIC_MULTICAST_DELEGATE(FTankDelegate);

UCLASS()
class BATTLETANK_API ATank : public APawn
{
	GENERATED_BODY()

public:
	// Called by the engine when actor damage is dealt
	virtual float TakeDamage(float DamageAmount, struct FDamageEvent const & DamageEvent, class AController * EventInstigator, AActor * DamageCauser) override;

	// Return current health as a percentage of starting health, between 0 and 1
	UFUNCTION(BlueprintPure, Category = "Health") // BlueprintPure is like C++ const, it doesn't affect the owning object and it's members
	float GetHealthPercent();

	UPROPERTY(BlueprintAssignable)
	FTankDelegate OnDeath;

private:
	// Sets default values for this pawn's properties
	ATank();

	virtual void BeginPlay() override;

	UPROPERTY(EditDefaultsOnly, Category = "Setup")
	int32 StartingHealth = 100;

	// if CurrentHealth is set to StartingHealth here, it's too soon and
	// a change of the latter's value in Tank_BP won't update CurrentHealth.
	// It needs to be initialized in BeginPlay()
	UPROPERTY(VisibleAnywhere, Category = "Health")
	int32 CurrentHealth;
};