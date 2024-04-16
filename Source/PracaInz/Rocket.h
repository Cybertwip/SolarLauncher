#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "Planet.h" // Include the Planet header file
#include "Components/StaticMeshComponent.h"
#include "Components/WidgetComponent.h"

#include "AstralObject.h"

#include "Rocket.generated.h"


class APracaInzGameState;


UCLASS()
class PRACAINZ_API ARocket : public AAstralObject
{
	GENERATED_BODY()
	
public:
	// Sets default values for this actor's properties
	ARocket();
	
protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;
	
public:
	// Called every frame
	virtual void Tick(float DeltaTime) override;
	
	// Components
	UPROPERTY(VisibleAnywhere)
	UStaticMeshComponent* RocketMesh;
	
	UPROPERTY(VisibleAnywhere)
	UWidgetComponent* RocketInfoWidget;
		
	UPROPERTY(EditAnywhere)
	FVector InitialVelocity;
	
	UPROPERTY(EditAnywhere)
	float ThrustForce;

	UPROPERTY(EditAnywhere)
	float MaxThrustForce;

	UPROPERTY(EditAnywhere)
	float MaxSpeed;
	
	UPROPERTY(EditAnywhere)
	FColor RocketColor;

	// State
	bool bIsThrusterActive;
	
	FVector Velocity;
	
	UPROPERTY(EditAnywhere, Category = "Components")
	FColor OrbitColor;
	UPROPERTY(EditAnywhere, Category = "Components")
	float Inclination;
	
	AAstralObject* Target;

	// Calculates whether the rocket is ready for launch based on target distance and optimal launch window
	void CalculateLaunchReadiness();
	
	// Calculates the time required to reach the target based on current thrust and mass
	float CalculateTimeToReachTarget(float Distance, float Thrust, float Mass);
	
	// Calculates the phase angle at a specified time in the future to determine if it's within a launch window
	double CalculatePhaseAngleAtFutureTime(float Time);
	
	// Initiates the rocket's launch if conditions are favorable
	void Launch();
	
	// Determines if the current phase angle is within the acceptable range for a launch window
	bool IsLaunchWindow(double PhaseAngle);

	void AdjustOrientationTowardsTarget();

private:
	// State
	bool bIsOverLapped;
	FVector p;
	FVector startPosition;
	bool bIsBeingDestroyed;
	bool bFirstCalculations = true;
	
public:
	// Called when rocket is selected
	UFUNCTION()
	void OnSelected(AActor* Target, FKey ButtonPressed);
	
	// Called when overlap begins with another actor
	UFUNCTION()
	void OnOverlapBegin(UPrimitiveComponent* OverlappedComp, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult);
	
	// Called when overlap ends with another actor
	UFUNCTION()
	void OnOverlapEnd(class UPrimitiveComponent* OverlappedComp, class AActor* OtherActor, class UPrimitiveComponent* OtherComp, int32 OtherBodyIndex);
	
	
	void InitialSetup();
	
	void PerformInitialCalculations(float DeltaTime, APracaInzGameState* GameState);
	
	// Destroy the rocket
	void DestroyRocket();
	
	// Fire the thruster
	void FireThruster();
	
	// Action binding function to toggle thruster
	void ToggleThruster();
	
	void InitialCalculations(float DeltaTime);
	
	bool bLaunching = false;
	
	
	void AdjustOrientationTowardsMovingDirection();

	void AdjustThrustAsNearingTarget();

};
