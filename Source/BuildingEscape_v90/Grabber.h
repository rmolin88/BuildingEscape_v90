// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "Components/ActorComponent.h"
#include "Grabber.generated.h"

UCLASS(ClassGroup = (Custom), meta = (BlueprintSpawnableComponent))
class BUILDINGESCAPE_V90_API UGrabber : public UActorComponent
{
	GENERATED_BODY()

	AActor *pActorOwner = nullptr;
	UWorld *pWorld = nullptr;
	APlayerController *pPlayerController = nullptr;

	FRotator roViewPoint;
	FVector vePos;
	FVector veRayEnd;
	FColor colorRed;

	// Collisions stuff
	FHitResult hitResult;
	FCollisionQueryParams collisionQueryParams;
	FCollisionObjectQueryParams collisionObjectQueryParams;

	UPROPERTY(EditAnywhere)
	float fDrawLineReach = 60;

	UPhysicsHandleComponent *pPhysicsHandle = nullptr;

	UInputComponent *pInputComponent = nullptr;

	void Grab(void);
	void GrabReleased(void);
	void CustomDrawDebugLine(const FVector veStartPos, const FVector veEndPos);
	bool CheckActorHitResults(const FVector veStartPos, const FVector veEndPos, FHitResult &outHitResult);
	void CalculateVectorStartEndPos(FVector &veStartPos, FVector &veEndPos);
	void GrabComponent(const FHitResult HitResult) const;

	    public :
	    // Sets default values for this component's properties
	    UGrabber();

	// Called when the game starts
	virtual void BeginPlay() override;

	// Called every frame
	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction *ThisTickFunction) override;
};

