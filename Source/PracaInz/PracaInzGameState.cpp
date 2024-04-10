// Fill out your copyright notice in the Description page of Project Settings.


#include "PracaInzGameState.h"

#include "GameFramework/PlayerController.h"
#include "Kismet/GameplayStatics.h"

APracaInzGameState::APracaInzGameState()
{
	PrimaryActorTick.bCanEverTick = true;
}

void APracaInzGameState::BeginPlay()
{
	FName levelName = FName(*GetWorld()->GetName());
	if (levelName == FName("SolarSystem"))
	{
		G = 1.179E-10;
		BaseDistance = (1.0 / 1000.0) * 149597870700;
	}
	else
	{
		G = 49.004E-7;
		BaseDistance = 1 / (1E3);
	}
}


void APracaInzGameState::Tick(float DeltaSeconds)
{
	Super::Tick(DeltaSeconds);
	
	// Update the total elapsed time since the start of the simulation
	TimeSinceStart += DeltaSeconds;
}

void APracaInzGameState::SelectNextPlanet()
{
	if (Planets.Num() == 0)
		return;
	
	int32 NextIndex = (Planets.IndexOfByKey(CurrentPlanet) + 1) % Planets.Num();
	CurrentPlanet = Planets[NextIndex];
	
	CurrentPlanet->OnSelected(nullptr, FKey("E"));
}

void APracaInzGameState::SelectPreviousPlanet()
{
	if (Planets.Num() == 0)
		return;
	
	int32 PreviousIndex = (Planets.IndexOfByKey(CurrentPlanet) - 1 + Planets.Num()) % Planets.Num();
	CurrentPlanet = Planets[PreviousIndex];
	
	CurrentPlanet->OnSelected(nullptr, FKey("Q"));

}


void APracaInzGameState::SelectRocket()
{
	Rocket->OnSelected(nullptr, FKey("Q"));	
}
