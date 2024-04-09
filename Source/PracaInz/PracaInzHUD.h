// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/HUD.h"
#include "Components/WidgetComponent.h"
#include "Planet.h"
#include "PracaInzHUD.generated.h"

/**
 * 
 */
UCLASS()
class PRACAINZ_API APracaInzHUD : public AHUD
{
	GENERATED_BODY()
public:
	APracaInzHUD();

	virtual void DrawHUD() override;

	virtual void BeginPlay() override;

	virtual void Tick(float DeltaTime) override;

	UPROPERTY(EditDefaultsOnly, Category = "Widgets")
		TSubclassOf<UUserWidget> PlanetInfoWidgetClass;

	class UPlanetInfoWidget* PlanetInfoWidget;

	void UpdatePlanetInfo(APlanet* Planet);
	//void UpdateRocketInfo(ARocket* Rocket);

	
};
