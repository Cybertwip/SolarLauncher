#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "Planet.h" // Include the Planet header file
#include "Components/StaticMeshComponent.h"
#include "Components/WidgetComponent.h"
#include "Rocket.generated.h"

UCLASS()
class PRACAINZ_API ARocket : public AActor
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
	
	// Rocket properties
	UPROPERTY(EditAnywhere)
	float RocketMass;
	
	UPROPERTY(EditAnywhere)
	FVector InitialVelocity;
	
	UPROPERTY(EditAnywhere)
	float ThrustForce;
	
	UPROPERTY(EditAnywhere)
	float MaxSpeed;
	
	UPROPERTY(EditAnywhere)
	FColor RocketColor;
	
	// State
	bool bIsThrusterActive;
	
	FVector Velocity;
	
	UPROPERTY(EditAnywhere, Category = "Components")
	FColor OrbitColor;


private:
	// State
	bool bIsOverLapped;
	FVector p;
	FVector startPosition;
	bool bIsBeingDestroyed;
	
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
	
	// Update rocket's position based on gravitational forces and thruster
	void UpdateRocketPosition(float DeltaTime);
	
	// Destroy the rocket
	void DestroyRocket();
	
	// Fire the thruster
	void FireThruster();
	
	// Action binding function to toggle thruster
	void ToggleThruster();
};
