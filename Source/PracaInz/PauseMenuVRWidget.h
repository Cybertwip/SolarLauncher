// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "Runtime\UMG\Public\UMG.h"
#include "Components/EditableTextBox.h"
#include "Planet.h"
#include "PauseMenuVRWidget.generated.h"

/**
 * 
 */
UCLASS()
class PRACAINZ_API UPauseMenuVRWidget : public UUserWidget
{
	GENERATED_BODY()

public:

	UPauseMenuVRWidget(const FObjectInitializer& ObjectInitializer);

	virtual void NativeConstruct() override;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta = (BindWidget))
		UEditableTextBox* Delta_Time_Textbox;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta = (BindWidget))
		UButton* Zero_Button;
	UFUNCTION()
		void OnZero_Button();
	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta = (BindWidget))
		UButton* One_Button;
	UFUNCTION()
		void OnOne_Button();
	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta = (BindWidget))
		UButton* Two_Button;
	UFUNCTION()
		void OnTwo_Button();
	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta = (BindWidget))
		UButton* Three_Button;
	UFUNCTION()
		void OnThree_Button();
	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta = (BindWidget))
		UButton* Four_Button;
	UFUNCTION()
		void OnFour_Button();
	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta = (BindWidget))
		UButton* Five_Button;
	UFUNCTION()
		void OnFive_Button();
	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta = (BindWidget))
		UButton* Six_Button;
	UFUNCTION()
		void OnSix_Button();
	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta = (BindWidget))
		UButton* Seven_Button;
	UFUNCTION()
		void OnSeven_Button();
	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta = (BindWidget))
		UButton* Eight_Button;
	UFUNCTION()
		void OnEight_Button();
	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta = (BindWidget))
		UButton* Nine_Button;
	UFUNCTION()
		void OnNine_Button();
	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta = (BindWidget))
		UButton* dot_Button;
	UFUNCTION()
		void Ondot_Button();
	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta = (BindWidget))
		UButton* C_Button;
	UFUNCTION()
		void OnC_Button();

	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta = (BindWidget))
		UButton* Load_Current_Delta_Time_Button;
	UFUNCTION()
		void OnLoadCurrentDeltaTime_Button();

	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta = (BindWidget))
		UButton* Save_Delta_Time_Button;
	UFUNCTION()
		void OnSaveDeltaTime_Button();

	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta = (BindWidget))
		UButton* Reset_Button;
	UFUNCTION()
		void OnReset_Button();

	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta = (BindWidget))
		UButton* Exit_Level_Button;
	UFUNCTION()
		void OnExitLevel_Button();

	void ModifyText(FString text);
	
};
