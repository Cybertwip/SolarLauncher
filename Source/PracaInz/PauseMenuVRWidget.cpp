// Fill out your copyright notice in the Description page of Project Settings.


#include "PauseMenuVRWidget.h"
#include "PracaInzGameModeBase.h"
#include "PracaInzGameState.h"


UPauseMenuVRWidget::UPauseMenuVRWidget(const FObjectInitializer& ObjectInitializer) : Super(ObjectInitializer)
{

}

void UPauseMenuVRWidget::NativeConstruct()
{

	Load_Current_Delta_Time_Button->OnClicked.AddDynamic(this, &UPauseMenuVRWidget::OnLoadCurrentDeltaTime_Button);
	Save_Delta_Time_Button->OnClicked.AddDynamic(this, &UPauseMenuVRWidget::OnSaveDeltaTime_Button);
	Reset_Button->OnClicked.AddDynamic(this, &UPauseMenuVRWidget::OnReset_Button);
	Exit_Level_Button->OnClicked.AddDynamic(this, &UPauseMenuVRWidget::OnExitLevel_Button);

	Zero_Button->OnClicked.AddDynamic(this, &UPauseMenuVRWidget::OnZero_Button);
	One_Button->OnClicked.AddDynamic(this, &UPauseMenuVRWidget::OnOne_Button);
	Two_Button->OnClicked.AddDynamic(this, &UPauseMenuVRWidget::OnTwo_Button);
	Three_Button->OnClicked.AddDynamic(this, &UPauseMenuVRWidget::OnThree_Button);
	Four_Button->OnClicked.AddDynamic(this, &UPauseMenuVRWidget::OnFour_Button);
	Five_Button->OnClicked.AddDynamic(this, &UPauseMenuVRWidget::OnFive_Button);
	Six_Button->OnClicked.AddDynamic(this, &UPauseMenuVRWidget::OnSix_Button);
	Seven_Button->OnClicked.AddDynamic(this, &UPauseMenuVRWidget::OnSeven_Button);
	Eight_Button->OnClicked.AddDynamic(this, &UPauseMenuVRWidget::OnEight_Button);
	Nine_Button->OnClicked.AddDynamic(this, &UPauseMenuVRWidget::OnNine_Button);
	dot_Button->OnClicked.AddDynamic(this, &UPauseMenuVRWidget::Ondot_Button);
	C_Button->OnClicked.AddDynamic(this, &UPauseMenuVRWidget::OnC_Button);

}


void UPauseMenuVRWidget::ModifyText(FString text)
{
	if (Delta_Time_Textbox)
	{
		FString textboxText = Delta_Time_Textbox->GetText().ToString();
		textboxText += text;
		Delta_Time_Textbox->SetText(FText::FromString((textboxText)));
	}
}


void UPauseMenuVRWidget::OnZero_Button()
{
	ModifyText("0");
}

void UPauseMenuVRWidget::OnOne_Button()
{
	ModifyText("1");
}

void UPauseMenuVRWidget::OnTwo_Button()
{
	ModifyText("2");
}

void UPauseMenuVRWidget::OnThree_Button()
{
	ModifyText("3");
}

void UPauseMenuVRWidget::OnFour_Button()
{
	ModifyText("4");
}

void UPauseMenuVRWidget::OnFive_Button()
{
	ModifyText("5");
}

void UPauseMenuVRWidget::OnSix_Button()
{
	ModifyText("6");
}

void UPauseMenuVRWidget::OnSeven_Button()
{
	ModifyText("7");
}

void UPauseMenuVRWidget::OnEight_Button()
{
	ModifyText("8");
}

void UPauseMenuVRWidget::OnNine_Button()
{
	ModifyText("9");
}

void UPauseMenuVRWidget::Ondot_Button()
{
	ModifyText(".");
}

void UPauseMenuVRWidget::OnC_Button()
{
	if (Delta_Time_Textbox)
		Delta_Time_Textbox->SetText(FText::FromString(""));
}

void UPauseMenuVRWidget::OnLoadCurrentDeltaTime_Button()
{
	if (APracaInzGameState* PracaInzGameState = Cast<APracaInzGameState>(GetWorld()->GetGameState()))
	{
		if (Delta_Time_Textbox)
		{
			if (Delta_Time_Textbox->Visibility == ESlateVisibility::Hidden)
			{
				Delta_Time_Textbox->SetVisibility(ESlateVisibility::Visible);
			}
			Delta_Time_Textbox->SetText(FText::FromString((FString::SanitizeFloat(PracaInzGameState->SecondsInSimulation))));

		}
	}
}

void UPauseMenuVRWidget::OnSaveDeltaTime_Button()
{
	if (APracaInzGameState* PracaInzGameState = Cast<APracaInzGameState>(GetWorld()->GetGameState()))
	{
		PracaInzGameState->SecondsInSimulation = FCString::Atoi64(*Delta_Time_Textbox->GetText().ToString());
		Delta_Time_Textbox->SetText(FText::FromString((FString::SanitizeFloat(PracaInzGameState->SecondsInSimulation))));
	}
}

void UPauseMenuVRWidget::OnReset_Button()
{
	UGameplayStatics::OpenLevel(this, FName(*GetWorld()->GetName()), false);
}

void UPauseMenuVRWidget::OnExitLevel_Button()
{
	UGameplayStatics::OpenLevel(this, FName("TestMainMenu"), false);
}

