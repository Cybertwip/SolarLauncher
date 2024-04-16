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
	PlanetMass = 1.f;
	InitialVelocity = FVector::ZeroVector;
	ThrustForce = 10.0f;
	MaxSpeed = 5000.f;
	RocketColor = FColor::White;
	
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
		
		PerformInitialCalculations(0.016, PracaInzGameState);
	} else {
		p = InitialVelocity * PlanetMass;
	}
}

void ARocket::PerformInitialCalculations(float DeltaTime, APracaInzGameState* GameState)
{
	FVector currentPosition = GetActorLocation();
	FVector referenceDirection = FVector(0, 0, 1);
	// Apply initial transformations and calculations
	currentPosition += FVector(0, 1, 0) * Inclination;  // Adjust position slightly by inclination along the Z-axis if needed
	p *= DeltaTime;
	Velocity *= DeltaTime;
	p = p.RotateAngleAxis(0, referenceDirection);
	Velocity = Velocity.RotateAngleAxis(0, referenceDirection);
	SetActorLocation(currentPosition);
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
	
	
	CalculateLaunchReadiness();
	
	FVector TotalForce = PrecomputedForce;
	FVector Thrust;
	
	if (bIsThrusterActive)
	{
		// Primary forward thrust
		Thrust = GetActorForwardVector() * ThrustForce;
		
		// Example control input or AI for lateral and vertical thrust:
		float LateralThrustMultiplier = 1.0f; // Determine based on need to move right or left
		float VerticalThrustMultiplier = 1.0f; // Determine based on need to move up or down
		
		// Adding lateral and vertical thrust based on right and up vectors
		Thrust += GetActorRightVector() * (ThrustForce * LateralThrustMultiplier);
		Thrust += GetActorUpVector() * (ThrustForce * VerticalThrustMultiplier);
		
		// Apply gravitational scaling and time squared (as acceleration needs time squared)
		Thrust *= GameState->G * DeltaTime * DeltaTime;
		
		TotalForce += Thrust;
	}

	// Update planet position if it's not being destroyed
	if (bIsBeingDestroyed)
	{
		DestroyRocket();  // Handle planet destruction
	} else {
		
		FVector currentPosition = GetActorLocation();
		
		// Calculate the new momentum based on the current one and the time jump
		FVector New_p = p + (TotalForce * GameState->SecondsInSimulation);
		
		// Calculate the new velocity
		Velocity = New_p / PlanetMass;
		
		SetActorLocation(GetActorLocation() + (Velocity * GameState->SecondsInSimulation));
		
		AdjustOrientationTowardsTarget();
		
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
	
	// Create a rotation that aims the 'Z' vector of the rocket towards the target
	FRotator DesiredRotation = FRotationMatrix::MakeFromZ(DirectionToTarget).Rotator();
	
	// Smoothly interpolate the rocket's current rotation towards the desired rotation
	SetActorRotation(FMath::Lerp(GetActorRotation(), DesiredRotation, 0.1f)); // Tune the lerp factor (0.1f here) based on desired responsiveness
}

void ARocket::CalculateLaunchReadiness()
{
	if (!Target)
	{
		UE_LOG(LogTemp, Warning, TEXT("No target set for the rocket."));
		return;
	}
	
	FVector TargetLocation = Target->GetActorLocation();
	FVector RocketLocation = GetActorLocation();
	float DistanceToTarget = FVector::Dist(TargetLocation, RocketLocation);
	
	// Calculate time to reach the target with current thrust settings
	float TimeToReachTarget = CalculateTimeToReachTarget(DistanceToTarget, ThrustForce, PlanetMass);
	
	// Calculate the phase angle at the time of potential arrival
	double PhaseAngleAtArrival = CalculatePhaseAngleAtFutureTime(TimeToReachTarget);
	
	// Check if the phase angle is within the launch window
	if (IsLaunchWindow(PhaseAngleAtArrival))
	{
		Launch();
	}
}

float ARocket::CalculateTimeToReachTarget(float Distance, float Thrust, float Mass)
{
	// Assuming a simplified constant acceleration formula: t = sqrt(2 * d / a)
	// where a = F / m (Thrust divided by Mass)
	float Acceleration = Thrust / Mass;
	return FMath::Sqrt(2 * Distance / Acceleration);
}

double ARocket::CalculatePhaseAngleAtFutureTime(float Time)
{
	// Placeholder for phase angle calculation
	// This should ideally account for the positions of Earth, target planet, and the rocket over time
	// Simplified example:
	return FMath::Fmod(360.0 * (Time / 3600.0), 360.0); // Assuming a simple circular orbit
}

void ARocket::Launch()
{
	bIsThrusterActive = true; // Activating the thrusters to start the launch
	AdjustOrientationTowardsTarget(); // Ensure the rocket is pointed towards the target
	UE_LOG(LogTemp, Log, TEXT("Rocket launched towards target."));
}

bool ARocket::IsLaunchWindow(double PhaseAngle)
{
	const double MinPhaseAngle = 4.0;
	const double MaxPhaseAngle = 10.0;
	return PhaseAngle >= MinPhaseAngle && PhaseAngle <= MaxPhaseAngle;
}

