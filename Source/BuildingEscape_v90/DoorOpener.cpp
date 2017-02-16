// Fill out your copyright notice in the Description page of Project Settings.

#include "BuildingEscape_v90.h"
#include "DoorOpener.h"

// Sets default values for this component's properties
UDoorOpener::UDoorOpener()
{
	// Set this component to be initialized when the game starts, and to be ticked every frame.  You can turn these
	// features
	// off to improve performance if you don't need them.
	PrimaryComponentTick.bCanEverTick = true;
	// ...
}

// Called when the game starts
void UDoorOpener::BeginPlay()
{
	Super::BeginPlay();
	pActorParent = GetOwner();
}

// Called every frame
void UDoorOpener::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

	if (GetMassOfOverlappingActors() < fTriggerMass)
	{
		if (fDoorAngle > 0)
			SetDoorAngle(--fDoorAngle);
	}
	else
	{
		if (fDoorAngle < fDoorOpenAngle)
			SetDoorAngle(++fDoorAngle);
	}
}

void UDoorOpener::SetDoorAngle(float fAngle)
{
	if (pActorParent == nullptr)
	{
		UE_LOG(LogTemp, Error, TEXT("SetDoorAngle(): Failed to load pActorParent"))
		return;
	}

	pActorParent->SetActorRotation(FRotator(0, fAngle, 0));
}

bool UDoorOpener::CheckSpotlight(void)
{
	try
	{
		/// Here is where you need to fix to include also the Default_Pawn_BluePrint
		if (pPressurePlate == nullptr)
		{
			UE_LOG(LogTemp, Error, TEXT("CheckSpotlight(): Failed to load pActorDoorOpener"))
			return false;
		}

		// If we enter the TriggerVolume Area
		return pPressurePlate->EncompassesPoint(pActorParent->GetActorLocation(), 0, &fDistanceToPoint);
	}
	catch (...)
	{
		UE_LOG(LogTemp, Error, TEXT("CheckSpotlight(): Exception"))
		return false;
	}
}

float UDoorOpener::GetMassOfOverlappingActors(void)
{
	if (pPressurePlate == nullptr)
	{
		UE_LOG(LogTemp, Error, TEXT("CheckSpotlight(): Failed to load pActorDoorOpener"))
		return 0;
	}

	// Get all overlapping actors
	pPressurePlate->GetOverlappingActors(OverlappingActors);
	float fTotalMass = 0;
	for (const auto* pActor : OverlappingActors)
	{
		if (pActor == nullptr)
		{
			UE_LOG(LogTemp, Error, TEXT("Failed to pointer to actor in array"))
			continue;
		}

		UE_LOG(LogTemp, Display, TEXT("%s inside spotlight"), *pActor->GetName())

		UPrimitiveComponent* pActorPrimComp = nullptr;
		pActorPrimComp = Cast<UPrimitiveComponent>(pActor->GetRootComponent());
		if (pActorPrimComp == nullptr)
		{
			UE_LOG(LogTemp, Error, TEXT("Failed to get mass"))
			continue;
		}

		UE_LOG(LogTemp, Display, TEXT("Mass = %f"), pActorPrimComp->GetMass())
		fTotalMass += pActorPrimComp->GetMass();
	}

	return fTotalMass;
}

