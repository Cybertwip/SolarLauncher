// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "Math/UnrealMath.h"
#include "Components/WidgetComponent.h"

#include "AstralObject.h"
#include "Planet.generated.h"

UCLASS()
class PRACAINZ_API APlanet : public AAstralObject
{
	GENERATED_BODY()
	
public:
	// Sets default values for this actor's properties
	APlanet();
	
protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;
	
	
public:
	// Called every frame
	virtual void Tick(float DeltaTime) override;
	UPROPERTY(EditAnywhere, Category = "Components")
	UStaticMeshComponent* PlanetMesh;
	UPROPERTY(EditAnywhere, Category = "Components")
	FVector InitialVelocity;
	UPROPERTY(EditAnywhere, Category = "Components")
	float RotationSpeed;
	UPROPERTY(EditAnywhere, Category = "Components")
	double Diameter;
	UPROPERTY(EditAnywhere, Category = "Components")
	double Inclination;
	UPROPERTY(EditAnywhere, Category = "Components")
	FString Name;
	UPROPERTY(EditAnywhere, Category = "Components")
	FColor OrbitColor;
	FVector p;
	FVector Velocity;
	
	//VR
	bool bIsBindedToPlayerInput = false;
	bool bIsOverLapped = false;
	bool bIsPaused = false;
	
	FInputActionBinding* inputActionBindingLeft;
	FInputActionBinding* inputActionBindingRight;
		
	bool bIsBeingDestroyed = false;
	FTimerHandle DestroyTimer;
	
	UFUNCTION()
	void OnSelected(AActor* Target, FKey ButtonPressed);
//	UFUNCTION()
//	void OnOverlapBegin(UPrimitiveComponent* OverlappedComp, AActor* OtherActor, UPrimitiveComponent* OtherComp,
//						int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult);
//	UFUNCTION()
//	void OnOverlapEnd(class UPrimitiveComponent* OverlappedComp, class AActor* OtherActor,
//					  class UPrimitiveComponent* OtherComp, int32 OtherBodyIndex);
	UFUNCTION(BluePrintCallable)
	void OnHit(UPrimitiveComponent* HitComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, FVector normalImpulse, const FHitResult& Hit);

	void InitialSetup();
	
	void DestroyPlanet();
	void InitialCalculations(float DeltaTime);
	void OnTriggerPressed();
	void OnXButtonPressed();
	
	int OrbitalPeriodDays;
};
