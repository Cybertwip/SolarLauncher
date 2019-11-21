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
		TArray<APlanet*> Planets;
		ACameraPawn* Camera;
		APlanet* CurrentPlanet;
		double G = 49.004e-7;
		double BaseMass;
		double BaseDistance;
		int64 SecondsInSimulation = 86400;
};
