// Fill out your copyright notice in the Description page of Project Settings.

#include "TankTurret.h"

void UTankTurret::Rotate(float RelativeSpeed)
{
	auto Time = GetWorld()->GetTimeSeconds();

	// Rotate the Turret the right amount this frame
	// given a max rotation speed and the frame time
	// Clamp is a static method and doesn't change the RelativeSpeed variable at class level (the UTankTurret::RelativeSpeed property),
	// we are just using the homonymous variable for operations below, it's only clamped within the scope of this function.
	RelativeSpeed = FMath::Clamp<float>(RelativeSpeed, -1, 1); // specifying <type> is optional
	auto RotationChange = RelativeSpeed * MaxDegreesPerSecond * GetWorld()->DeltaTimeSeconds;
	auto Rotation = RelativeRotation.Yaw + RotationChange;
	SetRelativeRotation(FRotator(0, Rotation, 0));
}




