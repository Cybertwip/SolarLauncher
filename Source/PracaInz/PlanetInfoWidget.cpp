// Fill out your copyright notice in the Description page of Project Settings.


#include "PlanetInfoWidget.h"
#include "PracaInzGameState.h"
#include "Engine.h"
#include "TimerManager.h"
#include "Planet.h"


UPlanetInfoWidget::UPlanetInfoWidget(const FObjectInitializer& ObjectInitializer) : Super(ObjectInitializer)
{

}

void UPlanetInfoWidget::NativeConstruct()
{
	Super::NativeConstruct();
	FScriptDelegate DelegateMass;
	DelegateMass.BindUFunction(this, "OnCommittedPlanetMass");
	PlanetMassTextBox->OnTextCommitted.Add(DelegateMass);

	FScriptDelegate DelegateTime;
	DelegateTime.BindUFunction(this, "OnCommittedTime");
	SecondsTextBox->OnTextCommitted.Add(DelegateTime);

	ResetButton->OnClicked.AddDynamic(this, &UPlanetInfoWidget::OnReset);
}

void UPlanetInfoWidget::UpdatePlanetInfo(APlanet* Planet)
{
	if (TXTPlanet)
	{
		if (TXTPlanet->Visibility == ESlateVisibility::Hidden)
		{
			TXTPlanet->SetVisibility(ESlateVisibility::Visible);
		}
		TXTPlanet->SetText(FText::FromString("Focused planet: " + Planet->Name + "!"));
	}
	if (PlanetMassTextBox)
	{
		if (PlanetMassTextBox->Visibility == ESlateVisibility::Hidden)
		{
			PlanetMassTextBox->SetVisibility(ESlateVisibility::Visible);
		}
		PlanetMassTextBox->SetText(FText::FromString((FString::SanitizeFloat(Planet->PlanetMass))));
	}
	if (SecondsTextBox)
	{
		if (SecondsTextBox->Visibility == ESlateVisibility::Hidden)
		{
			SecondsTextBox->SetVisibility(ESlateVisibility::Visible);
		}
		if (APracaInzGameState* PracaInzGameState = Cast<APracaInzGameState>(GetWorld()->GetGameState()))
		{
			SecondsTextBox->SetText(FText::FromString((FString::SanitizeFloat(PracaInzGameState->SecondsInSimulation))));
		}
	}
	if (ResetButton)
	{
		if (ResetButton->Visibility == ESlateVisibility::Hidden)
		{
			ResetButton->SetVisibility(ESlateVisibility::Visible);
		}
	}
}
	

void UPlanetInfoWidget::OnCommittedPlanetMass(const FText& Text, ETextCommit::Type CommitMethod) const
{
	if (APracaInzGameState* PracaInzGameState = Cast<APracaInzGameState>(GetWorld()->GetGameState()))
	{
		
		APlanet* Planet = PracaInzGameState->CurrentPlanet;
		Planet->PlanetMass = FCString::Atof(*PlanetMassTextBox->GetText().ToString());
		Planet->p = Planet->PlanetMass * Planet->Velocity;
	}
}

void UPlanetInfoWidget::OnCommittedTime(const FText& Text, ETextCommit::Type CommitMethod) const
{
	if (APracaInzGameState* PracaInzGameState = Cast<APracaInzGameState>(GetWorld()->GetGameState()))
	{
		PracaInzGameState->SecondsInSimulation = FCString::Atoi64(*SecondsTextBox->GetText().ToString());
		SecondsTextBox->SetText(FText::FromString((FString::SanitizeFloat(PracaInzGameState->SecondsInSimulation))));
	}
}

void UPlanetInfoWidget::OnReset()
{
	UGameplayStatics::OpenLevel(this, FName(*GetWorld()->GetName()), false);
}


