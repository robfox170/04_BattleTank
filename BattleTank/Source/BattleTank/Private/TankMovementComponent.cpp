// Copyright EmbraceIT Ltd

#include "TankMovementComponent.h"
#include "TankTrack.h"

void UTankMovementComponent::Initialize(UTankTrack* LeftTrackToSet, UTankTrack* RightTrackToSet)
{
	LeftTrack = LeftTrackToSet;
	RightTrack = RightTrackToSet;
}

void UTankMovementComponent::IntendMoveForward(float Throw)
{
	if (!ensure(LeftTrack && RightTrack)) { return; } // or if(!ensure(LeftTrack) || !ensure(RightTrack)) to have separate messages
	LeftTrack->SetThrottle(Throw);
	RightTrack->SetThrottle(Throw);
}

void UTankMovementComponent::IntendTurnRight(float Throw)
{
	if (!ensure(LeftTrack && RightTrack)) { return; }
	LeftTrack->SetThrottle(Throw);
	RightTrack->SetThrottle(-Throw);
}

// For the AI Tanks, called directly by MoveToActor() in TankAIController (method inherited from AAIController using pathfinding logic)
void UTankMovementComponent::RequestDirectMove(const FVector& MoveVelocity, bool bForceMaxSpeed)
{
	// No need to call Super:: as we are replacing the functionality here
	auto TankForward = GetOwner()->GetActorForwardVector().GetSafeNormal();
	auto AIForwardIntention = MoveVelocity.GetSafeNormal();

	// Using vector dot product (projection or scalar product) to obtain a forward throw based on a cosine curve (1 0 -1 0) 
	// representing the parallelism of the tank forward and the AI forward intention vectors
	float ForwardThrow = FVector::DotProduct(TankForward, AIForwardIntention);
	IntendMoveForward(ForwardThrow);

	// Using vector cross product to obtain a right throw based on a sine curve (0 1 0 -1) 
	// representing the perpendicularity of the tank forward and the AI forward intention vectors. 
	// Note that order in the operation matters, because the result is oriented along the Z axis
	auto RightThrow = FVector::CrossProduct(TankForward, AIForwardIntention).Z;
	IntendTurnRight(RightThrow);
	
	//auto TankName = GetOwner()->GetName();
	//UE_LOG(LogTemp, Warning, TEXT("%s : AIForwardIntention is %s "), *TankName, *(AIForwardIntention.ToString()))
}






