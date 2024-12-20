// Fill out your copyright notice in the Description page of Project Settings.


#include "PlanetInfoWidget.h"
#include "PracaInzGameState.h"
#include "Engine.h"
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

	FScriptDelegate DelegateVelocity;
	DelegateVelocity.BindUFunction(this, "OnCommittedVelocity");
	VelocityTextBox->OnTextCommitted.Add(DelegateVelocity);

	FScriptDelegate DelegateInclination;
	DelegateInclination.BindUFunction(this, "OnCommittedInclination");
	InclinationTextBox->OnTextCommitted.Add(DelegateInclination);

	ResetButton->OnClicked.AddDynamic(this, &UPlanetInfoWidget::OnReset);

	ExitButton->OnClicked.AddDynamic(this, &UPlanetInfoWidget::OnExit);

	StopButton->OnClicked.AddDynamic(this, &UPlanetInfoWidget::OnStop);
	StopButton->AddChild(StopButtonText);
}

void UPlanetInfoWidget::UpdatePlanetInfo(APlanet* Planet)
{
	if (PlanetTextInfo)
	{
		if (PlanetTextInfo->Visibility == ESlateVisibility::Hidden)
		{
			PlanetTextInfo->SetVisibility(ESlateVisibility::Visible);
		}	
		PlanetTextInfo->SetText(FText::FromString("Nazwa obiektu: " + Planet->Name));
	}
	if (PlanetMassTextBox)
	{
		if (PlanetMassTextBox->Visibility == ESlateVisibility::Hidden)
		{
			PlanetMassTextBox->SetVisibility(ESlateVisibility::Visible);
		}
		PlanetMassTextBox->SetText(FText::FromString((FString::SanitizeFloat(Planet->PlanetMass))));
	}
	if (InclinationTextBox)
	{
		if (InclinationTextBox->Visibility == ESlateVisibility::Hidden)
		{
			InclinationTextBox->SetVisibility(ESlateVisibility::Visible);
		}
		InclinationTextBox->SetText(FText::FromString((FString::SanitizeFloat(Planet->Inclination))));
	}
	if (VelocityTextBox)
	{
		if (VelocityTextBox->Visibility == ESlateVisibility::Hidden)
		{
			VelocityTextBox->SetVisibility(ESlateVisibility::Visible);
		}
		if (APracaInzGameState* PracaInzGameState = Cast<APracaInzGameState>(GetWorld()->GetGameState()))
		{
			VelocityTextBox->SetText(FText::FromString((FString::SanitizeFloat(Planet->Velocity.Size() 
				 /PracaInzGameState->CurrentDeltaTime  /PracaInzGameState->BaseDistance))));
		}
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
	if (ExitButton)
	{
		if (ExitButton->Visibility == ESlateVisibility::Hidden)
		{
			ExitButton->SetVisibility(ESlateVisibility::Visible);
		}
	}
	if (StopButton)
	{
		if (StopButton->Visibility == ESlateVisibility::Hidden)
		{
			StopButton->SetVisibility(ESlateVisibility::Visible);
		}
	}
	if (BaseMassText)
	{
		if (BaseMassText->Visibility == ESlateVisibility::Hidden)
		{
			BaseMassText->SetVisibility(ESlateVisibility::Visible);
		}
		if(FName(*GetWorld()->GetName()).IsEqual(FName("SolarSystem")))
			BaseMassText->SetText(FText::FromString("mas Ziemi"));
		else
			BaseMassText->SetText(FText::FromString("mas Ksiezyca"));
	}
}
	

void UPlanetInfoWidget::OnCommittedPlanetMass() 
{
	if (APracaInzGameState* PracaInzGameState = Cast<APracaInzGameState>(GetWorld()->GetGameState()))
	{
		APlanet* Planet = PracaInzGameState->CurrentPlanet;
		double mass = FCString::Atof(*PlanetMassTextBox->GetText().ToString());
		for (int i=0; i != PracaInzGameState->Planets.Num();i++)
		{
			APlanet* x = PracaInzGameState->Planets[i];
			if (fabs(x->PlanetMass - mass) <= 0.0000001 * fabs(x->PlanetMass) && x!=Planet)	
			{
				mass = mass + 0.0001;
				i = -1;
			}
		}
		Planet->PlanetMass = mass;
		Planet->p = Planet->PlanetMass * Planet->Velocity;
	}
}

void UPlanetInfoWidget::OnCommittedTime() 
{
	if (APracaInzGameState* PracaInzGameState = Cast<APracaInzGameState>(GetWorld()->GetGameState()))
	{
		PracaInzGameState->SecondsInSimulation = FCString::Atoi64(*SecondsTextBox->GetText().ToString());
		SecondsTextBox->SetText(FText::FromString((FString::SanitizeFloat(PracaInzGameState->SecondsInSimulation))));
	}
}

void UPlanetInfoWidget::OnCommittedVelocity() 
{
	if (APracaInzGameState* PracaInzGameState = Cast<APracaInzGameState>(GetWorld()->GetGameState()))
	{
		APlanet* Planet = PracaInzGameState->CurrentPlanet;

		if(FCString::Atof(*VelocityTextBox->GetText().ToString())==0)
		{ 
			Planet->Velocity = 0.00001 *
			PracaInzGameState->CurrentDeltaTime * PracaInzGameState->BaseDistance *
			Planet->Velocity / Planet->Velocity.Size();
		}
		else
		{ 
			Planet->Velocity = FCString::Atof(*VelocityTextBox->GetText().ToString()) * 
			PracaInzGameState -> CurrentDeltaTime * PracaInzGameState->BaseDistance * 
			Planet->Velocity/Planet->Velocity.Size();
		}
		Planet->p = Planet->PlanetMass * Planet->Velocity;
	}
}

void UPlanetInfoWidget::OnCommittedInclination() 
{
	if (APracaInzGameState* PracaInzGameState = Cast<APracaInzGameState>(GetWorld()->GetGameState()))
	{
		APlanet* Planet = PracaInzGameState->CurrentPlanet;
		if (Planet == PracaInzGameState->Planets[0])
			return;
		float oldInclination = Planet->Inclination;
		Planet->Inclination = FCString::Atof(*InclinationTextBox->GetText().ToString());
		Planet->p = Planet->startP;
		Planet->Velocity = Planet->startVelocity;
		Planet->SetActorLocation(Planet->startPosition);
		FVector R = Planet->startPosition - PracaInzGameState->Planets[0]->GetActorLocation();
		if (R.Normalize())
		{
		
			Planet->p = Planet->p.RotateAngleAxis(Planet->Inclination, R);
			Planet->Velocity = Planet->Velocity.RotateAngleAxis(Planet->Inclination, R);
		}
	}
}

void UPlanetInfoWidget::OnReset()
{
	UGameplayStatics::OpenLevel(this, FName(*GetWorld()->GetName()), false);
}

void UPlanetInfoWidget::OnExit()
{
	UGameplayStatics::OpenLevel(this, FName("MainMenu"), false);
}

void UPlanetInfoWidget::OnStop()
{
	isStopped = !isStopped;
	UGameplayStatics::SetGamePaused(GetWorld(), isStopped);
}


