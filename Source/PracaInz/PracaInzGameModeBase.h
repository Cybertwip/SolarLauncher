// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "Planet.h"
#include "CoreMinimal.h"
#include "GameFramework/GameModeBase.h"
#include "PracaInzGameModeBase.generated.h"

/**
 * 
 */
UCLASS()
class PRACAINZ_API APracaInzGameModeBase : public AGameModeBase
{
	GENERATED_BODY()

	public:
		APracaInzGameModeBase();
		void OnPlanetCreate(APlanet* Planet);
		bool isPlanetSelected = false;

		virtual void BeginPlay() override;

		UPROPERTY(EditAnywhere, Category = "Components")
			bool isEditMode = false;
		UFUNCTION(BlueprintCallable, Category = "Components")
			virtual bool getIsEditMode() const;
		UFUNCTION(BlueprintCallable, Category = "Components")
			virtual void setIsEditMode(bool value);

		UPROPERTY(EditAnywhere, Category = "Components")
			bool isMenuOpen = false;
		UFUNCTION(BlueprintCallable, Category = "Components")
			virtual bool getIsMenuOpen() const;
		UFUNCTION(BlueprintCallable, Category = "Components")
			virtual void setIsMenuOpen(bool value);
};
