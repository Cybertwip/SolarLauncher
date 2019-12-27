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

	FScriptDelegate DelegateVelocity;
	DelegateVelocity.BindUFunction(this, "OnCommittedVelocity");
	VelocityTextBox->OnTextCommitted.Add(DelegateVelocity);

	FScriptDelegate DelegateDiameter;
	DelegateDiameter.BindUFunction(this, "OnCommittedDiameter");
	DiameterTextBox->OnTextCommitted.Add(DelegateDiameter);

	FScriptDelegate DelegateInclination;
	DelegateInclination.BindUFunction(this, "OnCommittedInclination");
	InclinationTextBox->OnTextCommitted.Add(DelegateInclination);

	ResetButton->OnClicked.AddDynamic(this, &UPlanetInfoWidget::OnReset);

	ExitButton->OnClicked.AddDynamic(this, &UPlanetInfoWidget::OnExit);
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
	if (DiameterTextBox)
	{
		if (DiameterTextBox->Visibility == ESlateVisibility::Hidden)
		{
			DiameterTextBox->SetVisibility(ESlateVisibility::Visible);
		}
		DiameterTextBox->SetText(FText::FromString((FString::SanitizeFloat(Planet->Diameter))));
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

void UPlanetInfoWidget::OnCommittedVelocity(const FText& Text, ETextCommit::Type CommitMethod) const
{
	if (APracaInzGameState* PracaInzGameState = Cast<APracaInzGameState>(GetWorld()->GetGameState()))
	{
		APlanet* Planet = PracaInzGameState->CurrentPlanet;

		//switch units 
		Planet->Velocity = FCString::Atof(*VelocityTextBox->GetText().ToString()) * 
		PracaInzGameState -> CurrentDeltaTime * PracaInzGameState->BaseDistance * 
		//univector
		Planet->Velocity/Planet->Velocity.Size();

		Planet->p = Planet->PlanetMass * Planet->Velocity;
	}
}

void UPlanetInfoWidget::OnCommittedDiameter(const FText& Text, ETextCommit::Type CommitMethod) const
{	
	if (APracaInzGameState* PracaInzGameState = Cast<APracaInzGameState>(GetWorld()->GetGameState()))
	{
		APlanet* Planet = PracaInzGameState->CurrentPlanet;
		Planet->Diameter = FCString::Atof(*DiameterTextBox->GetText().ToString());
		Planet->PlanetMesh->SetWorldScale3D(FVector(Planet->Diameter, Planet->Diameter, Planet->Diameter));
	}
}

void UPlanetInfoWidget::OnCommittedInclination(const FText& Text, ETextCommit::Type CommitMethod) const
{
	if (APracaInzGameState* PracaInzGameState = Cast<APracaInzGameState>(GetWorld()->GetGameState()))
	{
		APlanet* Planet = PracaInzGameState->CurrentPlanet;
		Planet->Inclination = FCString::Atof(*InclinationTextBox->GetText().ToString());
		FVector R = Planet->GetActorLocation() - PracaInzGameState->Planets[0]->GetActorLocation();
		FVector Axis = R;
		Axis.Normalize();
		Axis = Axis.GetSafeNormal();
		Planet->p = Planet->p.RotateAngleAxis(Planet->Inclination,Axis);
		Planet->Velocity = Planet->Velocity.RotateAngleAxis(Planet->Inclination,Axis);
		//GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Red, FString::Printf(TEXT("Rotation: %s"), *NewR.ToString()));
		//Planet->SetActorLocation(NewR);
		UE_LOG(LogTemp, Warning, TEXT("Rotation: %s!"), *R.Rotation().ToString());
	}
}

void UPlanetInfoWidget::OnReset()
{
	UGameplayStatics::OpenLevel(this, FName(*GetWorld()->GetName()), false);
}

void UPlanetInfoWidget::OnExit()
{
	FGenericPlatformMisc::RequestExit(false);
}


