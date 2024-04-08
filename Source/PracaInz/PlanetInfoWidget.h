// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "Runtime/UMG/Public/UMG.h"
#include "Components/EditableTextBox.h"
#include "Planet.h"
#include "PlanetInfoWidget.generated.h"

/**
 * 
 */
UCLASS()
class PRACAINZ_API UPlanetInfoWidget : public UUserWidget
{
	GENERATED_BODY()

public:
	UPlanetInfoWidget(const FObjectInitializer& ObjectInitializer);

	virtual void NativeConstruct() override;				  
	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta = (BindWidget))
		UTextBlock* PlanetTextInfo;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta = (BindWidget))
		UEditableTextBox* PlanetMassTextBox;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta = (BindWidget))
		UEditableTextBox* SecondsTextBox;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta = (BindWidget))
		UEditableTextBox* VelocityTextBox;


	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta = (BindWidget))
		UEditableTextBox* InclinationTextBox;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta = (BindWidget))
		UButton* ResetButton;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta = (BindWidget))
		UButton* ExitButton;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta = (BindWidget))
		UButton* StopButton;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta = (BindWidget))
		UTextBlock* StopButtonText;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta = (BindWidget))
		UTextBlock* BaseMassText;
	bool isStopped = false;

	void UpdatePlanetInfo(APlanet* Planet);

	UFUNCTION()
		void OnCommittedPlanetMass();

	UFUNCTION()
		void OnCommittedTime() ;

	UFUNCTION()
		void OnCommittedVelocity() ;

	UFUNCTION()
		void OnCommittedInclination() ;

	UFUNCTION()
		void OnReset();
	UFUNCTION()
		void OnExit();
	UFUNCTION()
		void OnStop();
};
