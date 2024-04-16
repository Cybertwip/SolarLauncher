#include "Rocket.h"
#include "PracaInzGameModeBase.h"
#include "PracaInzGameState.h"
#include "PracaInzHUD.h"

#include "GameFramework/ProjectileMovementComponent.h"
#include "Components/StaticMeshComponent.h"
#include "Components/WidgetComponent.h"
#include "DrawDebugHelpers.h"
#include "Kismet/GameplayStatics.h"
#include "Kismet/KismetMathLibrary.h"

// Sets default values
ARocket::ARocket()
{
	// Set this actor to call Tick() every frame
	PrimaryActorTick.bCanEverTick = false;
	
	// Create and set up the rocket mesh
	RocketMesh = CreateDefaultSubobject<UStaticMeshComponent>("RocketMesh");
	RocketMesh->SetAbsolute(true, true, true);
	
	// Set up rocket info widget
	RocketInfoWidget = CreateDefaultSubobject<UWidgetComponent>("RocketInfoWidget");

	SetRootComponent(RocketMesh);

	// Initialize rocket properties
	PlanetMass = 0.000004f;
	InitialVelocity = FVector::ZeroVector;
	MaxThrustForce = 10.0;
	
	ThrustForce = MaxThrustForce;
	MaxSpeed = 5000.f;
	RocketColor = FColor::Blue;
	
	// Set initial state
	bIsThrusterActive = false; // Start with thruster active
	
}

// Called when the game starts or when spawned
void ARocket::BeginPlay()
{
	Super::BeginPlay();
	
	//	PlanetMesh->OnComponentBeginOverlap.AddDynamic(this, &APlanet::OnOverlapBegin);
	//	PlanetMesh->OnComponentEndOverlap.AddDynamic(this, &APlanet::OnOverlapEnd);
//	RocketMesh->OnComponentHit.AddUniqueDynamic(this, &ARocket::OnHit);
	
	RocketInfoWidget->SetWorldScale3D(FVector(0.1, 0.1, 0.1));

	InitialSetup();
	
	if (APracaInzGameModeBase* PracaInzGameModeBase = Cast<APracaInzGameModeBase>(GetWorld()->GetAuthGameMode()))
	{
		PracaInzGameModeBase->OnRocketCreate(this);
	}
}

void ARocket::InitialSetup(){
	
	if (APracaInzGameState* PracaInzGameState = Cast<APracaInzGameState>(GetWorld()->GetGameState()))
	{
		InitialVelocity *= PracaInzGameState->BaseDistance;
		Velocity = InitialVelocity;
		
		p = InitialVelocity * PlanetMass;
	} else {
		p = InitialVelocity * PlanetMass;
	}
}

void ARocket::Launch(){
	bIsThrusterActive = true;
}


// Called every frame
void ARocket::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);  // Always call the base class tick
	
	// Obtain the game mode and check if we are in edit mode
	APracaInzGameModeBase* GameModeBase = Cast<APracaInzGameModeBase>(GetWorld()->GetAuthGameMode());
	if (!GameModeBase || GameModeBase->isEditMode)
	{
		return;  // Skip processing if we are in edit mode or game mode is not available
	}
	
	// Access the game state
	APracaInzGameState* GameState = Cast<APracaInzGameState>(GetWorld()->GetGameState());
	if (!GameState)
	{
		return;  // Early exit if game state is not accessible
	}
	
	
	FVector TotalForce = PrecomputedForce;
	FVector Thrust;
	
	if (bIsThrusterActive)
	{
//		// Primary forward thrust
//		Thrust = GetActorForwardVector() * ThrustForce;
//		
//		// Example control input or AI for lateral and vertical thrust:
//		float LateralThrustMultiplier = 1.0f; // Determine based on need to move right or left
//		float VerticalThrustMultiplier = 1.0f; // Determine based on need to move up or down
//		
//		// Adding lateral and vertical thrust based on right and up vectors
//		Thrust += GetActorRightVector() * (ThrustForce * LateralThrustMultiplier);
//		Thrust += GetActorUpVector() * (ThrustForce * VerticalThrustMultiplier);
//		
		
		FVector TargetLocation = Target->GetActorLocation();
		FVector CurrentPosition = GetActorLocation();
		FVector DirectionToTarget = (TargetLocation - CurrentPosition).GetSafeNormal();
		Thrust = DirectionToTarget * ThrustForce;

		// Apply gravitational scaling and time squared (as acceleration needs time squared)
//		Thrust *= GameState->G * DeltaTime * DeltaTime;
		
		TotalForce += Thrust;
	}

	// Update planet position if it's not being destroyed
	if (bIsBeingDestroyed)
	{
		DestroyRocket();  // Handle planet destruction
	} else {
		FVector currentPosition = GetActorLocation();
		
		FVector New_p = p + PrecomputedForce * DeltaTime * GameState->SecondsInSimulation;
		
		// Calculate the new velocity from the new momentum.
		Velocity = New_p / PlanetMass;
		
		// Update the planet's position based on the new velocity and the time delta.
		SetActorLocation(currentPosition + (Velocity * DeltaTime * GameState->SecondsInSimulation));
		
		// Remember the newly calculated momentum
		p = New_p;

		// Optional: Update rotation based on new position if necessary
//		FRotator NewRotation = GetActorRotation();
		
		// Debugging: Draw a line from the old position to the new position
		DrawDebugLine(GetWorld(), currentPosition, GetActorLocation(), RocketColor, false, 1);
	}
}
void ARocket::OnSelected(AActor* Target, FKey ButtonPressed)
{
	if (APracaInzGameState* PracaInzGameState = Cast<APracaInzGameState>(GetWorld()->GetGameState()))
	{
		PracaInzGameState->Camera->Focused = Cast<USceneComponent>(RocketMesh);
		PracaInzGameState->Rocket = this;
		if (APracaInzHUD* PracaInzHUD = Cast<APracaInzHUD>(GetWorld()->GetFirstPlayerController()->GetHUD()))
		{
			//PracaInzHUD->UpdateRocketInfo(this);
		}
		PracaInzGameState->Camera->bOnChangePlanet = true;
	}}

void ARocket::OnOverlapBegin(UPrimitiveComponent* OverlappedComp, AActor* OtherActor, UPrimitiveComponent* OtherComp,
							 int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	bIsOverLapped = true;
}

void ARocket::OnOverlapEnd(class UPrimitiveComponent* OverlappedComp, class AActor* OtherActor,
						   class UPrimitiveComponent* OtherComp, int32 OtherBodyIndex)
{
	bIsOverLapped = false;
}

bool ARocket::IsLaunchWindow(double PhaseAngle) {
	// Define the acceptable range of phase angles for launch window
	const double MinPhaseAngle = 4.20; // Minimum phase angle (degrees)
	const double MaxPhaseAngle = 46.0; // Maximum phase angle (degrees)
	
	// Check if the phase angle is within the launch window range
	return (PhaseAngle >= MinPhaseAngle && PhaseAngle <= MaxPhaseAngle);
}

void ARocket::DestroyRocket()
{
	Destroy();
}

void ARocket::FireThruster()
{
	APracaInzGameState* GameState = Cast<APracaInzGameState>(GetWorld()->GetGameState());
	
	if(GameState){
		// Apply thrust force to rocket
		FVector Thrust = GetActorForwardVector() * ThrustForce * GameState->BaseDistance;
		Velocity += Thrust / PlanetMass;
	}
}

// Action binding function to toggle thruster
void ARocket::ToggleThruster()
{
	bIsThrusterActive = !bIsThrusterActive;
}

void ARocket::AdjustOrientationTowardsTarget()
{
	if (!Target)
		return;
	
	FVector TargetLocation = Target->GetActorLocation();
	FVector CurrentLocation = GetActorLocation();
	FVector DirectionToTarget = (TargetLocation - CurrentLocation).GetSafeNormal();
	
	// Adjust the rotation to face the target
	FRotator DesiredRotation = FRotationMatrix::MakeFromZ(DirectionToTarget).Rotator();
	SetActorRotation(FMath::Lerp(GetActorRotation(), DesiredRotation, 0.1f)); // Smooth rotation towards the target with interpolation
}

void ARocket::AdjustOrientationTowardsMovingDirection()
{
	FVector CurrentVelocity = p / PlanetMass;
	if (!CurrentVelocity.IsNearlyZero())
	{
		FRotator DesiredRotation = CurrentVelocity.Rotation();
		SetActorRotation(FMath::Lerp(GetActorRotation(), DesiredRotation, 0.1f));
	}
}

void ARocket::AdjustThrustAsNearingTarget()
{
	if (!bIsThrusterActive) {
		return;
	}
	
	FVector TargetLocation = Target->GetActorLocation();
	float Distance = FVector::Dist(GetActorLocation(), TargetLocation);
	
	// Define the distance range within which to start reducing thrust
	float StartReduceDistance = 3500.0f; // Start reducing thrust at this distance
	float MinReduceDistance = 200.0f;    // Minimum distance where thrust should be at its minimum
	
	if (Distance < StartReduceDistance)
	{
		
		// Calculate the lerp factor based on the current distance
		float LerpFactor = (Distance - MinReduceDistance) / (StartReduceDistance - MinReduceDistance);
		LerpFactor = FMath::Clamp(LerpFactor, 0.0f, 1.0f); // Ensure the lerp factor stays within the valid range
		
		// Apply the Cubic Ease Out formula
		LerpFactor = 1 - FMath::Pow(1 - LerpFactor, 3);
		
		// Interpolate the thrust force between its current value and the minimum value
		ThrustForce = ThrustForce * LerpFactor * 0.5f;
	}
	
	if(Distance <= MinReduceDistance){
		bIsThrusterActive = false;
		
		ThrustForce = MaxThrustForce;
	}
}
