// Fill out your copyright notice in the Description page of Project Settings.


#include "PlanetInfoVRWidget.h"
#include "PracaInzGameModeBase.h"
#include "PracaInzGameState.h"




UPlanetInfoVRWidget::UPlanetInfoVRWidget(const FObjectInitializer& ObjectInitializer) : Super(ObjectInitializer)
{

}



void UPlanetInfoVRWidget::NativeConstruct()
{

	Exit_Button->OnClicked.AddDynamic(this, &UPlanetInfoVRWidget::OnExit_Button);
	Save_Button->OnClicked.AddDynamic(this, &UPlanetInfoVRWidget::OnSave_Button);
	Load_Current_Params->OnClicked.AddDynamic(this, &UPlanetInfoVRWidget::OnLoadCurrentParrams_Button);

	Zero_Button->OnClicked.AddDynamic(this, &UPlanetInfoVRWidget::OnZero_Button);
	One_Button->OnClicked.AddDynamic(this, &UPlanetInfoVRWidget::OnOne_Button);
	Two_Button->OnClicked.AddDynamic(this, &UPlanetInfoVRWidget::OnTwo_Button);
	Three_Button->OnClicked.AddDynamic(this, &UPlanetInfoVRWidget::OnThree_Button);
	Four_Button->OnClicked.AddDynamic(this, &UPlanetInfoVRWidget::OnFour_Button);
	Five_Button->OnClicked.AddDynamic(this, &UPlanetInfoVRWidget::OnFive_Button);
	Six_Button->OnClicked.AddDynamic(this, &UPlanetInfoVRWidget::OnSix_Button);
	Seven_Button->OnClicked.AddDynamic(this, &UPlanetInfoVRWidget::OnSeven_Button);
	Eight_Button->OnClicked.AddDynamic(this, &UPlanetInfoVRWidget::OnEight_Button);
	Nine_Button->OnClicked.AddDynamic(this, &UPlanetInfoVRWidget::OnNine_Button);
	dot_Button->OnClicked.AddDynamic(this, &UPlanetInfoVRWidget::Ondot_Button);
	C_Button->OnClicked.AddDynamic(this, &UPlanetInfoVRWidget::OnC_Button);
	
	FScriptDelegate DelegateMass;
	DelegateMass.BindUFunction(this, "OnMass_Textbox_Committed");
	Mass_Textbox->OnTextCommitted.Add(DelegateMass);

	FScriptDelegate DelegateVelocity;
	DelegateVelocity.BindUFunction(this, "OnVelocity_Textbox_Committed");
	Velocity_Textbox->OnTextCommitted.Add(DelegateVelocity);
}

void UPlanetInfoVRWidget::OnMass_Textbox_Committed()
{
	/*if (GEngine)
		GEngine->AddOnScreenDebugMessage(-1, 10.0f, FColor::Yellow, TEXT("On PlanetMass"));*/
	Selected_Textbox = Mass_Textbox;
}

void UPlanetInfoVRWidget::OnVelocity_Textbox_Committed()
{
	/*if (GEngine)
		GEngine->AddOnScreenDebugMessage(-1, 10.0f, FColor::Yellow, TEXT("On Velocity"));*/
	Selected_Textbox = Velocity_Textbox;
}

void UPlanetInfoVRWidget::OnZero_Button()
{
	ModifyText("0");
}

void UPlanetInfoVRWidget::OnOne_Button()
{
	ModifyText("1");
}

void UPlanetInfoVRWidget::OnTwo_Button()
{
	ModifyText("2");
}

void UPlanetInfoVRWidget::OnThree_Button()
{
	ModifyText("3");
}

void UPlanetInfoVRWidget::OnFour_Button()
{
	ModifyText("4");
}

void UPlanetInfoVRWidget::OnFive_Button()
{
	ModifyText("5");
}

void UPlanetInfoVRWidget::OnSix_Button()
{
	ModifyText("6");
}

void UPlanetInfoVRWidget::OnSeven_Button()
{
	ModifyText("7");
}

void UPlanetInfoVRWidget::OnEight_Button()
{
	ModifyText("8");
}

void UPlanetInfoVRWidget::OnNine_Button()
{
	ModifyText("9");
}

void UPlanetInfoVRWidget::Ondot_Button()
{
	ModifyText(".");
}

void UPlanetInfoVRWidget::OnC_Button()
{
	if(Selected_Textbox)
		Selected_Textbox->SetText(FText::FromString(""));
}

void UPlanetInfoVRWidget::OnExit_Button()
{
	/*if (GEngine)
		GEngine->AddOnScreenDebugMessage(-1, 10.0f, FColor::Yellow, TEXT("On Exit_Button pressed"));*/
	if (APracaInzGameState* PracaInzGameState = Cast<APracaInzGameState>(GetWorld()->GetGameState()))
	{
		PracaInzGameState->CurrentPlanetVR->PlanetMesh->SetWorldScale3D(PracaInzGameState->CurrentPlanetVRScale);
		if (APracaInzGameModeBase* PracaInzGameModeBase = Cast<APracaInzGameModeBase>(GetWorld()->GetAuthGameMode()))
		{
			PracaInzGameModeBase->isPlanetSelected = false;
		}
	}
}

void UPlanetInfoVRWidget::OnSave_Button()
{
	/*if (GEngine)
		GEngine->AddOnScreenDebugMessage(-1, 10.0f, FColor::Yellow, TEXT("On Save_Button pressed"));*/
	if (APracaInzGameState* PracaInzGameState = Cast<APracaInzGameState>(GetWorld()->GetGameState()))
	{
		APlanet* Planet = PracaInzGameState->CurrentPlanetVR;

		if (FCString::Atof(*Velocity_Textbox->GetText().ToString()) == 0)
		{
			Planet->Velocity = 0.00001 *
				PracaInzGameState->CurrentDeltaTime * PracaInzGameState->BaseDistance *
				Planet->Velocity / Planet->Velocity.Size();
		}
		else
		{
			Planet->Velocity = FCString::Atof(*Velocity_Textbox->GetText().ToString()) *
				PracaInzGameState->CurrentDeltaTime * PracaInzGameState->BaseDistance *
				Planet->Velocity / Planet->Velocity.Size();
		}

		double PlanetMass = FCString::Atof(*Mass_Textbox->GetText().ToString());
		for (int i = 0; i != PracaInzGameState->Planets.Num(); i++)
		{
			APlanet* x = PracaInzGameState->Planets[i];
			if (fabs(x->PlanetMass - PlanetMass) <= 0.0000001 * fabs(x->PlanetMass) && x != Planet)
			{
				PlanetMass = PlanetMass + 0.0001;
				i = -1;
			}
		}
		Planet->PlanetMass = PlanetMass;

		Planet->p = Planet->PlanetMass * Planet->Velocity;
	}
}

void UPlanetInfoVRWidget::OnLoadCurrentParrams_Button()
{
	if (APracaInzGameState* PracaInzGameState = Cast<APracaInzGameState>(GetWorld()->GetGameState()))
	{
		/*if (GEngine)
			GEngine->AddOnScreenDebugMessage(-1, 10.0f, FColor::Yellow, TEXT("Widget Updated"));*/
		if (Mass_Textbox)
		{
			if (Mass_Textbox->Visibility == ESlateVisibility::Hidden)
			{
				Mass_Textbox->SetVisibility(ESlateVisibility::Visible);
			}
			if (PracaInzGameState->CurrentPlanetVR)
			{
				Mass_Textbox->SetText(FText::FromString((FString::SanitizeFloat(PracaInzGameState->CurrentPlanetVR->PlanetMass))));
			}
		}

		if (Velocity_Textbox)
		{
			if (Velocity_Textbox->Visibility == ESlateVisibility::Hidden)
			{
				Velocity_Textbox->SetVisibility(ESlateVisibility::Visible);
			}
			if (PracaInzGameState->CurrentPlanetVR)
			{
				Velocity_Textbox->SetText(FText::FromString((FString::SanitizeFloat(PracaInzGameState->CurrentPlanetVR->Velocity.Size()
					/ PracaInzGameState->CurrentDeltaTime / PracaInzGameState->BaseDistance))));
			}
		}
	}
}

void UPlanetInfoVRWidget::ModifyText(FString text)
{
	if (Selected_Textbox)
	{
		FString textboxText = Selected_Textbox->GetText().ToString();
		textboxText += text;
		Selected_Textbox->SetText(FText::FromString((textboxText)));
	}
}
