// Fill out your copyright notice in the Description page of Project Settings.


#include "PracaInzGameModeBase.h"
#include "PracaInzGameState.h"
#include "PracaInzPlayerController.h"
#include "Kismet/GameplayStatics.h"



APracaInzGameModeBase::APracaInzGameModeBase()
{
	PlayerControllerClass = APracaInzPlayerController::StaticClass();
}

void APracaInzGameModeBase::BeginPlay()
{
	Super::BeginPlay();

	UGameViewportClient* GameViewport = GetWorld()->GetGameViewport();
	GameViewport->ViewModeIndex = 2;

	APlayerController* PlayerController = UGameplayStatics::GetPlayerController(GetWorld(), 0);

	// The command being executed.
	FString Command = "ShowFlag.Tonemapper 0";
	PlayerController->ConsoleCommand(*Command);

	// The below commands are for the shipping
	// build.
	Command = "r.TonemapperGamma 0";
	PlayerController->ConsoleCommand(*Command);

	Command = "r.TonemapperFilm 0";
	PlayerController->ConsoleCommand(*Command);

	Command = "r.Tonemapper.Sharpen 0";
	PlayerController->ConsoleCommand(*Command);

	Command = "r.Tonemapper.GrainQuantization 0";
	PlayerController->ConsoleCommand(*Command);

	Command = "r.Tonemapper.Quality 0";
	PlayerController->ConsoleCommand(*Command);

}

void APracaInzGameModeBase::OnPlanetCreate(APlanet* Planet)
{
	if (APracaInzGameState* PracaInzGameState = Cast<APracaInzGameState>(GameState))
	{
		PracaInzGameState->Planets.Add(Planet);
	}
}

void APracaInzGameModeBase::OnRocketCreate(ARocket* Rocket)
{
	if (APracaInzGameState* PracaInzGameState = Cast<APracaInzGameState>(GameState))
	{
		PracaInzGameState->Rocket = Rocket;
	}
}



bool APracaInzGameModeBase::getIsEditMode() const
{
	return isEditMode;
}

void APracaInzGameModeBase::setIsEditMode(bool value)
{
	isEditMode = value;
	if (!isEditMode && isPlanetSelected)
	{
		if (APracaInzGameState* PracaInzGameState = Cast<APracaInzGameState>(GetWorld()->GetGameState()))
		{
			if (PracaInzGameState->CurrentPlanetVR)
			{
				PracaInzGameState->CurrentPlanetVR->PlanetMesh->SetWorldScale3D(PracaInzGameState->CurrentPlanetVRScale);
			}
			if (APracaInzGameModeBase* PracaInzGameModeBase = Cast<APracaInzGameModeBase>(GetWorld()->GetAuthGameMode()))
			{
				PracaInzGameModeBase->isPlanetSelected = false;
			}
		}
	}
}


bool APracaInzGameModeBase::getIsMenuOpen() const
{
	return isMenuOpen;
}

void APracaInzGameModeBase::setIsMenuOpen(bool value)
{
	isMenuOpen = value;
	if (isMenuOpen && isPlanetSelected)
	{
		if (APracaInzGameState* PracaInzGameState = Cast<APracaInzGameState>(GetWorld()->GetGameState()))
		{
			if (PracaInzGameState->CurrentPlanetVR)
			{
				PracaInzGameState->CurrentPlanetVR->PlanetMesh->SetWorldScale3D(PracaInzGameState->CurrentPlanetVRScale);
			}
			if (APracaInzGameModeBase* PracaInzGameModeBase = Cast<APracaInzGameModeBase>(GetWorld()->GetAuthGameMode()))
			{
				PracaInzGameModeBase->isPlanetSelected = false;
			}
		}
	}
}

