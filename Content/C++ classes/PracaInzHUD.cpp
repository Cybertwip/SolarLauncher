// Fill out your copyright notice in the Description page of Project Settings.


#include "PracaInzHUD.h"
#include "PlanetInfoWidget.h"
#include "PracaInzGameState.h"
#include "Runtime/Engine/Classes/Engine/Engine.h"
#include "PracaInzPlayerController.h"


APracaInzHUD::APracaInzHUD()
{

}

void APracaInzHUD::DrawHUD()
{
	Super::DrawHUD();
}

void APracaInzHUD::BeginPlay()
{
	Super::BeginPlay();
	if (PlanetInfoWidgetClass)
	{
		PlanetInfoWidget = CreateWidget<UPlanetInfoWidget>(GetWorld(), PlanetInfoWidgetClass);
		if (PlanetInfoWidget)
		{
			PlanetInfoWidget->AddToViewport();
			if (APracaInzGameState* PracaInzGameState = Cast<APracaInzGameState>(GetWorld()->GetGameState()))
			{
				UpdatePlanetInfo(PracaInzGameState->Planets[0]);
			}
		}
	}
}

void APracaInzHUD::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
	if (APracaInzGameState* PracaInzGameState = Cast<APracaInzGameState>(GetWorld()->GetGameState()))
	{
		if(!PlanetInfoWidget->IsHovered() && PracaInzGameState->CurrentPlanet!=nullptr)
			UpdatePlanetInfo(PracaInzGameState->CurrentPlanet);
	}
}

void APracaInzHUD::UpdatePlanetInfo(APlanet* Planet)
{
	if(PlanetInfoWidget)
	{ 
		if (PlanetInfoWidget->Visibility == ESlateVisibility::Hidden)
		{
			PlanetInfoWidget->SetVisibility(ESlateVisibility::Visible);
		}
		PlanetInfoWidget->UpdatePlanetInfo(Planet);
	}
}
