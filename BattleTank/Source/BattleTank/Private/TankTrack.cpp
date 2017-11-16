// Fill out your copyright notice in the Description page of Project Settings.

#include "TankTrack.h"
// #include "Engine/World.h"

UTankTrack::UTankTrack()
{
	// Set this component to be initialized when the game starts, and to be ticked every frame.  You can turn these features
	// off to improve performance if you don't need them.
	PrimaryComponentTick.bCanEverTick = false;
	// ...
}

void UTankTrack::BeginPlay()
{
	OnComponentHit.AddDynamic(this, &UTankTrack::OnHit);
}

// Apply side force only when the tank touches the ground, i.e. when it is hit by it
// Since OnHit is called every frame, no need to have a TickComponent
void UTankTrack::OnHit(UPrimitiveComponent* HitComponent, AActor* OtherActor, UPrimitiveComponent* OtherComponent, FVector NormalImpulse, const FHitResult& Hit)
{
	DriveTrack();
	ApplySidewaysForce();
	CurrentThrottle = 0;
}


void UTankTrack::ApplySidewaysForce()
{
	// Work out the required acceleration this frame to correct (a = s / t)
	auto SlippageSpeed = FVector::DotProduct(GetComponentVelocity(), GetRightVector());
	auto DeltaTime = GetWorld()->GetDeltaSeconds();
	auto CorrectionAcceleration = -SlippageSpeed / DeltaTime * GetRightVector();

	// Calculate and apply force sideways (F = m a)
	auto TankRoot = Cast<UStaticMeshComponent>(GetOwner()->GetRootComponent()); // Cast as StaticMeshComp. to access GetMass() method
	auto CorrectionForce = (TankRoot->GetMass() * CorrectionAcceleration) / 2; // because there are 2 tracks
	TankRoot->AddForce(CorrectionForce);

	//UE_LOG(LogTemp, Warning, TEXT("Slippage speed is %f"), SlippageSpeed)
}



void UTankTrack::SetThrottle(float Throttle)
{
	// Ensures that back/forth and left/right thumbsick input combined don't exceed a full throttle ranging from -1 to 1
	CurrentThrottle = FMath::Clamp<float>(CurrentThrottle + Throttle, -1, 1);
}

void UTankTrack::DriveTrack()
{
	auto ForceApplied = GetForwardVector() * CurrentThrottle * TrackMaxDrivingForce;
	auto ForceLocation = GetComponentLocation();
	auto TankRoot = Cast<UPrimitiveComponent>(GetOwner()->GetRootComponent());
	TankRoot->AddForceAtLocation(ForceApplied, ForceLocation);
}

