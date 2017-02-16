// Fill out your copyright notice in the Description page of Project Settings.

#include "BuildingEscape_v90.h"
#include "Grabber.h"

// Sets default values for this component's properties
UGrabber::UGrabber()
{
	// Set this component to be initialized when the game starts, and to be ticked every frame.  You can turn these
	// features
	// off to improve performance if you don't need them.
	PrimaryComponentTick.bCanEverTick = true;

	colorRed = FColor(255, 0, 0);  // RGB initialized
	// ECollisionChannel CollisionChannel = ECC_PhysicsBody; Because that is the parent of PhysicsActor
	collisionObjectQueryParams = FCollisionObjectQueryParams(ECollisionChannel::ECC_PhysicsBody);
}

/// Called when the game starts
void UGrabber::BeginPlay()
{
	Super::BeginPlay();

	/// Get all the needed pointers for the class
	if (((pActorOwner = GetOwner()) == nullptr) || ((pWorld = GetWorld()) == nullptr) ||
	    ((pPlayerController = pWorld->GetFirstPlayerController()) == nullptr))
	{
		UE_LOG(LogTemp, Error, TEXT("Failed to get pActorOwner or pWorld, or pPlayerController"))
		return;
	}

	/// Setup Collision Components
	// 1. Input - FName No idea right now
	// 2. Input - False refers to bool bInTraceComplex and that is when the Collision Shape is complex and resembles a
	// lot to the
	// actual body of the object. Simple is just a big clump and much better.
	// 3. Input - Object to Ignore. Need to pass ourselves because if not we will be colliding with ourselves all of
	// the time apparently
	collisionQueryParams = FCollisionQueryParams(FName(TEXT("")), false, pActorOwner);

	// Find and attach PhysicsHandle
	if ((pPhysicsHandle = pActorOwner->FindComponentByClass<UPhysicsHandleComponent>()) == nullptr)
		UE_LOG(LogTemp, Error, TEXT("Couldnt find the PhysicsHandle inside of %s"), *pActorOwner->GetName())

	// Find and attach Input Component
	if ((pInputComponent = pActorOwner->FindComponentByClass<UInputComponent>()) == nullptr)
		UE_LOG(LogTemp, Error, TEXT("Failed to find UInputComponent"))
	else
	{
		// Grab comes from the name you set in `Project Settings/Input/Bindings/Action Mapping`
		// EInputEven::IE_PRESSED, Put puttings this doenst work
		// this is the UserClass *Object
		// Pointer to function that will be called on pressed key
		pInputComponent->BindAction(FName(TEXT("Grab")), IE_Pressed, this, &UGrabber::Grab);

		pInputComponent->BindAction(FName(TEXT("Grab")), IE_Released, this, &UGrabber::GrabReleased);
	}
}

/// Called every frame
void UGrabber::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction *ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

	if (pPhysicsHandle == nullptr)
	{
		UE_LOG(LogTemp, Error, TEXT("Failed to load attached PhysicsHandle"))
		return;
	}

	if (pPhysicsHandle->GrabbedComponent)
	{
		CalculateVectorStartEndPos(vePos, veRayEnd);
		pPhysicsHandle->SetTargetLocation(veRayEnd);
	}
}

void UGrabber::Grab(void)
{
	try
	{
		UE_LOG(LogTemp, Warning, TEXT("We pressed the grabber key"))
		CalculateVectorStartEndPos(vePos, veRayEnd);

		if (CheckActorHitResults(vePos, veRayEnd, hitResult))
		{
			UE_LOG(LogTemp, Warning, TEXT("Attempting to grab %s"), *hitResult.GetActor()->GetName())
			GrabComponent(hitResult);
		}
		else
		{
			UE_LOG(LogTemp, Warning, TEXT("Nothing to grab"))
		}
	}
	catch (...)
	{
		UE_LOG(LogTemp, Error, TEXT("Grab(): Exception"))
		return;
	}
}

void UGrabber::GrabReleased(void)
{
	if (pPhysicsHandle == nullptr)
	{
		UE_LOG(LogTemp, Warning, TEXT("Failed to load attached PhysicsHandle"))
		return;
	}

	if (pPhysicsHandle->GrabbedComponent)
		pPhysicsHandle->ReleaseComponent();
}

void UGrabber::CustomDrawDebugLine(const FVector veStartPos, const FVector veEndPos)
{
	if (pWorld == nullptr)
	{
		UE_LOG(LogTemp, Error, TEXT("CustomDrawDebugLine(): Failed to get pWorld"))
		return;
	}

	// Debugging
	// UE_LOG(LogTemp, Display, TEXT("Location: %s, Viewpoint: %s"), *vePos.ToString(), *roViewPoint.ToString())
	DrawDebugLine(pWorld,    // UWorld *
	              vePos,     // LineStart,
	              veRayEnd,  // LineEnd = LineStart + 80x
	              colorRed,  // Red Color,
	              false,     // bPersistentLines
	              -1.f,      // LifeTime
	              0,         // DepthPriority
	              10.f       // Thickness
	              );
}

void UGrabber::CalculateVectorStartEndPos(FVector &veStartPos, FVector &veEndPos)
{
	try
	{
		if (pPlayerController == nullptr)
		{
			UE_LOG(LogTemp, Error, TEXT("CalculateVectorStartEndPos(): Failed to get pPlayerController"))
			return;
		}
		pPlayerController->GetPlayerViewPoint(vePos, roViewPoint);   // Get Pos and Viewpoint
		veRayEnd = vePos + (roViewPoint.Vector() * fDrawLineReach);  // roViewPoint.Vector() returns a unit vector
	}
	catch (...)
	{
		UE_LOG(LogTemp, Error, TEXT("CalculateVectorStartEndPos(): Exception"))
		return;
	}
}

// Uses LineTraceSingleByObjectType to check if our Pawn has hit anything
bool UGrabber::CheckActorHitResults(const FVector veStartPos, const FVector veEndPos, FHitResult &outHitResult)
{
	if (pWorld == nullptr)
	{
		UE_LOG(LogTemp, Error, TEXT("CheckActorHitResults(): Failed to get pWorld"))
		return false;
	}

	// Line Trace (AKA Ray-Casting) out to reach to reach distance
	// Trace with an Array of Actors to Ignore
	// Re-initialize hit info
	outHitResult = FHitResult(ForceInit);

	return pWorld->LineTraceSingleByObjectType(outHitResult,                // result
	                                           veStartPos,                  // start
	                                           veEndPos,                    // end
	                                           collisionObjectQueryParams,  // List of object types looking for
	                                           collisionQueryParams         // Additional arguments
	                                           );
}

void UGrabber::GrabComponent(const FHitResult HitResult) const
{
	try
	{
		if (pPhysicsHandle == nullptr)
		{
			UE_LOG(LogTemp, Error, TEXT("No PhysicsHandle attached"))
			return;
		}

		AActor *pActor2Grab = nullptr;
		if ((pActor2Grab = HitResult.GetActor()) == nullptr)
		{
			UE_LOG(LogTemp, Error, TEXT("Failed to get Hit Actor Pointer"))
			return;
		}

		auto pPrimitiveComponent = HitResult.GetComponent();

		// GrabComponentAtLocationWithRotation(class UPrimitiveComponent* Component, FName InBoneName, FVector Location,
		// FRotator Rotation);
		pPhysicsHandle->GrabComponentAtLocationWithRotation(pPrimitiveComponent, NAME_None,
		                                                    pActor2Grab->GetActorLocation(), FRotator(0, 0, 0));
	}
	catch (...)
	{
		UE_LOG(LogTemp, Error, TEXT("GrabComponent(): Exception"))
		return;
	}
}

