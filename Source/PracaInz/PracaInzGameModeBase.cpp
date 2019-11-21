// Fill out your copyright notice in the Description page of Project Settings.


#include "PracaInzGameModeBase.h"
#include "PracaInzGameState.h"
#include "PracaInzPlayerController.h"


APracaInzGameModeBase::APracaInzGameModeBase()
{
	PlayerControllerClass = APracaInzPlayerController::StaticClass();
}


void APracaInzGameModeBase::OnPlanetCreate(APlanet* Planet)
{
	if (APracaInzGameState* PracaInzGameState = Cast<APracaInzGameState>(GameState))
	{
		PracaInzGameState->Planets.Add(Planet);
		UE_LOG(LogTemp, Warning, TEXT("Planet added!"));
	}
}


