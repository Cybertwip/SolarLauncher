// Fill out your copyright notice in the Description page of Project Settings.

//#pragma once

#include "Planet.h"
#include "CameraPawn.h"
#include "CoreMinimal.h"
#include "GameFramework/GameStateBase.h"
#include "PracaInzGameState.generated.h"

/**
 * 
 */
UCLASS()
class PRACAINZ_API APracaInzGameState : public AGameStateBase
{
	GENERATED_BODY()
	public:
		APracaInzGameState();
		virtual void BeginPlay() override;
		TArray<APlanet*> Planets;
		ACameraPawn* Camera;
		APlanet* CurrentPlanet;
		//base mass = moon mass
		//base distance = 100 000 km
		UPROPERTY(EditAnywhere, Category = "Components")
			double BaseDistance = 1/(1.5E5);
		UPROPERTY(EditAnywhere, Category = "Components")
			double G = 1.179E-10;
		int64 SecondsInSimulation = 86400;
		//int64 SecondsInSimulation = 166400;
		float CurrentDeltaTime = 1/60;


		//VR
		APlanet* CurrentPlanetVR;
		FVector CurrentPlanetVRScale;
};
