// Fill out your copyright notice in the Description page of Project Settings.

#include "TankTrack.h"

void UTankTrack::SetThrottle(float Throttle)
{
	//auto Time = GetWorld()->GetTimeSeconds();

	UE_LOG(LogTemp, Warning, TEXT("%s throttle is %f"), *GetName(), Throttle);

}


