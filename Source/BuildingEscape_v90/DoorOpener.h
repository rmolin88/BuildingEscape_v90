// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "BuildingEscape_v90.h"
#include "Components/ActorComponent.h"
#include "DoorOpener.generated.h"

UCLASS(ClassGroup = (Custom), meta = (BlueprintSpawnableComponent))
class BUILDINGESCAPE_V90_API UDoorOpener : public UActorComponent
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere)                    // This pointer gets loaded from the editor with the TriggerVolume class
	ATriggerVolume *pPressurePlate = nullptr;  // That is the main reason why you would do this.
	                                           // So that it can be loaded from
	                                           // editor. It also easier to adjust from editor than in code.
	UPROPERTY(EditAnywhere)
	float fDoorOpenAngle = 100;  // How far out will the door open

	UPROPERTY(EditAnywhere)
	float fTriggerMass = 100;  // Total mass that will trigger the doors to open

	AActor *pActorParent = nullptr;

	float fDistanceToPoint;
	float fDoorAngle;
	float fMassAtSpotlight = 0;
	TArray<AActor *> OverlappingActors;

	void SetDoorAngle(float fAngle);
	bool CheckSpotlight(void);
	float GetMassOfOverlappingActors(void);

  public:
	// Sets default values for this component's properties
	UDoorOpener();

	// Called when the game starts
	virtual void BeginPlay() override;

	// Called every frame
	virtual void TickComponent(float DeltaTime, ELevelTick TickType,
	                           FActorComponentTickFunction *ThisTickFunction) override;
};

