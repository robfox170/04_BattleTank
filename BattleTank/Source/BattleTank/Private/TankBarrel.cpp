// Fill out your copyright notice in the Description page of Project Settings.

#include "TankBarrel.h"

void UTankBarrel::Elevate(float RelativeSpeed)
{
	auto Time = GetWorld()->GetTimeSeconds();

	// Move the barrel the right amount this frame
	// given a max elevation speed and the frame time
	// Clamp is a static method and doesn't change the RelativeSpeed variable at class level (the UTankBarrel::RelativeSpeed property),
	// we are just using the homonymous variable for operations below, it's only clamped within the scope of this function.
	RelativeSpeed = FMath::Clamp<float>(RelativeSpeed, -1, 1); // specifying <type> is optional
	auto ElevationChange = RelativeSpeed * MaxDegreesPerSecond * GetWorld()->DeltaTimeSeconds;
	auto RawElevation = RelativeRotation.Pitch + ElevationChange;
	auto Elevation = FMath::Clamp<float>(RawElevation, MinElevationDegrees, MaxElevationDegrees); 
	SetRelativeRotation(FRotator(Elevation, 0, 0));
	//UE_LOG(LogTemp, Warning, TEXT("Barrel elevated at %f"), Elevation);


}


