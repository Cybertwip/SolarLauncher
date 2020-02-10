// Fill out your copyright notice in the Description page of Project Settings.


#include "PracaInzGameState.h"

APracaInzGameState::APracaInzGameState()
{
	
}

void APracaInzGameState::BeginPlay()
{
	FName levelName = FName(*GetWorld()->GetName());
	if (levelName == FName("SolarSystem"))
	{
		G = 1.179E-10;
		BaseDistance = 1 / (1.5E5);
	}
	else
	{
		G = 49.004E-7;
		BaseDistance = 1 / (1E3);
	}
}
