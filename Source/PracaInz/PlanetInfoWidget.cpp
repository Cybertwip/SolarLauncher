// Fill out your copyright notice in the Description page of Project Settings.


#include "PlanetInfoWidget.h"
#include "PracaInzGameState.h"
#include "Engine.h"
#include "Planet.h"
#include "Rocket.h"

#include <string>

UPlanetInfoWidget::UPlanetInfoWidget(const FObjectInitializer& ObjectInitializer) : Super(ObjectInitializer)
{

}

void UPlanetInfoWidget::NativeConstruct()
{
	Super::NativeConstruct();
	FScriptDelegate DelegateMass;
	DelegateMass.BindUFunction(this, "OnCommittedMass");
	MassTextBox->OnTextCommitted.Add(DelegateMass);

	FScriptDelegate DelegateTime;
	DelegateTime.BindUFunction(this, "OnSliderValueChanged");
	SecondsSlider->OnValueChanged.Add(DelegateTime);

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
		PlanetTextInfo->SetText(FText::FromString("Object Name: " + Planet->Name));
	}
	if (PlanetDaysInfo)
	{
		if (PlanetDaysInfo->Visibility == ESlateVisibility::Hidden)
		{
			PlanetDaysInfo->SetVisibility(ESlateVisibility::Visible);
		}
		auto days = std::to_string(Planet->OrbitalPeriodDays);
		PlanetDaysInfo->SetText(FText::FromString(days.c_str()));
	}
	if (MassTextBox)
	{
		if (MassTextBox->Visibility == ESlateVisibility::Hidden)
		{
			MassTextBox->SetVisibility(ESlateVisibility::Visible);
		}
		MassTextBox->SetText(FText::FromString((FString::SanitizeFloat(Planet->Mass))));
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
	if (SecondsSlider)
	{
		if (SecondsSlider->Visibility == ESlateVisibility::Hidden)
		{
			SecondsSlider->SetVisibility(ESlateVisibility::Visible);
		}
		if (APracaInzGameState* PracaInzGameState = Cast<APracaInzGameState>(GetWorld()->GetGameState()))
		{
			SecondsSlider->SetValue(PracaInzGameState->SecondsInSimulation);
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
			BaseMassText->SetText(FText::FromString(""));
		else
			BaseMassText->SetText(FText::FromString(""));
	}
}

void UPlanetInfoWidget::UpdateRocketInfo(ARocket* Rocket)
{
	if (PlanetTextInfo)
	{
		if (PlanetTextInfo->Visibility == ESlateVisibility::Hidden)
		{
			PlanetTextInfo->SetVisibility(ESlateVisibility::Visible);
		}
		PlanetTextInfo->SetText(FText::FromString("Object Name: Falcon 9"));
	}
	if (MassTextBox)
	{
		if (MassTextBox->Visibility == ESlateVisibility::Hidden)
		{
			MassTextBox->SetVisibility(ESlateVisibility::Visible);
		}
		MassTextBox->SetText(FText::FromString((FString::SanitizeFloat(Rocket->Mass))));
	}
	if (InclinationTextBox)
	{
		if (InclinationTextBox->Visibility == ESlateVisibility::Hidden)
		{
			InclinationTextBox->SetVisibility(ESlateVisibility::Visible);
		}
		//InclinationTextBox->SetText(FText::FromString((FString::SanitizeFloat(Rocket->Inclination))));
		
		InclinationTextBox->SetText(FText::FromString("N/A"));
	}
	if (VelocityTextBox)
	{
		if (VelocityTextBox->Visibility == ESlateVisibility::Hidden)
		{
			VelocityTextBox->SetVisibility(ESlateVisibility::Visible);
		}
		if (APracaInzGameState* PracaInzGameState = Cast<APracaInzGameState>(GetWorld()->GetGameState()))
		{
			VelocityTextBox->SetText(FText::FromString((FString::SanitizeFloat(Rocket->Velocity.Size()
																			   /PracaInzGameState->CurrentDeltaTime  /PracaInzGameState->BaseDistance))));
		}
	}
	if (SecondsSlider)
	{
		if (SecondsSlider->Visibility == ESlateVisibility::Hidden)
		{
			SecondsSlider->SetVisibility(ESlateVisibility::Visible);
		}
		if (APracaInzGameState* PracaInzGameState = Cast<APracaInzGameState>(GetWorld()->GetGameState()))
		{
			SecondsSlider->SetValue(PracaInzGameState->SecondsInSimulation);
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
			BaseMassText->SetText(FText::FromString(""));
		else
			BaseMassText->SetText(FText::FromString(""));
	}
}


void UPlanetInfoWidget::OnCommittedMass() 
{
	if (APracaInzGameState* PracaInzGameState = Cast<APracaInzGameState>(GetWorld()->GetGameState()))
	{
		APlanet* Planet = PracaInzGameState->CurrentPlanet;
		double mass = FCString::Atof(*MassTextBox->GetText().ToString());
		for (int i=0; i != PracaInzGameState->Planets.Num();i++)
		{
			APlanet* x = PracaInzGameState->Planets[i];
			if (fabs(x->Mass - mass) <= 0.0000001 * fabs(x->Mass) && x!=Planet)	
			{
				mass = mass + 0.0001;
				i = -1;
			}
		}
		Planet->Mass = mass;
		Planet->p = Planet->Mass * Planet->Velocity;
	}
}

void UPlanetInfoWidget::OnSliderValueChanged(float Value)
{
	// Assuming the slider value directly represents seconds in simulation,
	// and you want to update the simulation time accordingly.
	if (APracaInzGameState* PracaInzGameState = Cast<APracaInzGameState>(GetWorld()->GetGameState()))
	{
		PracaInzGameState->SecondsInSimulation = Value;
		// Optionally, update any UI elements or perform additional actions based on the new value
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
		Planet->p = Planet->Mass * Planet->Velocity;
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
		FVector R = PracaInzGameState->Planets[0]->GetActorLocation();
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
	UGameplayStatics::OpenLevel(this, FName("TestMainMenu"), false);
}

void UPlanetInfoWidget::OnStop()
{
	isStopped = !isStopped;
	UGameplayStatics::SetGamePaused(GetWorld(), isStopped);
}


