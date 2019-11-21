// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "Planet.h"
#include "CoreMinimal.h"
#include "GameFramework/GameModeBase.h"
#include "PracaInzGameModeBase.generated.h"

/**
 * 
 */
UCLASS()
class PRACAINZ_API APracaInzGameModeBase : public AGameModeBase
{
	GENERATED_BODY()

	public:
		APracaInzGameModeBase();
		void OnPlanetCreate(APlanet* Planet);
};
