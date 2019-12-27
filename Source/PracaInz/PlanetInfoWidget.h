// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "Runtime\UMG\Public\UMG.h"
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
		UTextBlock* TXTPlanet;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta = (BindWidget))
		UEditableTextBox* PlanetMassTextBox;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta = (BindWidget))
		UEditableTextBox* SecondsTextBox;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta = (BindWidget))
		UEditableTextBox* VelocityTextBox;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta = (BindWidget))
		UEditableTextBox* DiameterTextBox;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta = (BindWidget))
		UEditableTextBox* InclinationTextBox;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta = (BindWidget))
		UButton* ResetButton;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta = (BindWidget))
		UButton* ExitButton;

	void UpdatePlanetInfo(APlanet* Planet);

	UFUNCTION()
		void OnCommittedPlanetMass(const FText& Text, ETextCommit::Type CommitMethod) const;

	UFUNCTION()
		void OnCommittedTime(const FText& Text, ETextCommit::Type CommitMethod) const;

	UFUNCTION()
		void OnCommittedVelocity(const FText& Text, ETextCommit::Type CommitMethod) const;

	UFUNCTION()
		void OnCommittedDiameter(const FText& Text, ETextCommit::Type CommitMethod) const;

	UFUNCTION()
		void OnCommittedInclination(const FText& Text, ETextCommit::Type CommitMethod) const;

	UFUNCTION()
		void OnReset();
	UFUNCTION()
		void OnExit();
};
