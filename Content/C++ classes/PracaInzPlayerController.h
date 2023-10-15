// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/PlayerController.h"
#include "PracaInzPlayerController.generated.h"

/**
 * 
 */
UCLASS()
class PRACAINZ_API APracaInzPlayerController : public APlayerController
{
public:
	GENERATED_BODY()
		APracaInzPlayerController();

	virtual void BeginPlay() override;

	virtual void SetupInputComponent() override;
	
};
