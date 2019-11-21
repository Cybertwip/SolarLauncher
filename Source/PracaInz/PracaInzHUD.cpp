// Fill out your copyright notice in the Description page of Project Settings.


#include "PracaInzHUD.h"
#include "PlanetInfoWidget.h"
#include "SStandardSlateWidget.h"
#include "Widgets/SWeakWidget.h"
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
	/*
	MyUIWidget = SNew(SStandardSlateWidget).OwnerHUDArg(this);

	GEngine->GameViewport->AddViewportWidgetContent(
		SNew(SWeakWidget)
		.PossiblyNullContent(MyUIWidget.ToSharedRef())
	);
	MyUIWidget->SetVisibility(EVisibility::Visible);
	*/
	if (PlanetInfoWidgetClass)
	{
		PlanetInfoWidget = CreateWidget<UPlanetInfoWidget>(GetWorld(), PlanetInfoWidgetClass);
		if (PlanetInfoWidget)
		{
			PlanetInfoWidget->AddToViewport();
			PlanetInfoWidget->SetVisibility(ESlateVisibility::Hidden);
		}
	}
}

void APracaInzHUD::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
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
