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
	Mass = 1.f;
	InitialVelocity = FVector::ZeroVector;
	ThrustForce = 1.0f;
	MaxSpeed = 5000.f;
	RocketColor = FColor::White;
	
	// Set initial state
	bIsThrusterActive = true; // Start with thruster active
	
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
		
		p = InitialVelocity * Mass;
		
		PerformInitialCalculations(0.016, PracaInzGameState);
	} else {
		p = InitialVelocity * Mass;
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
	
	// Update planet position if it's not being destroyed
	if (bIsBeingDestroyed)
	{
		DestroyRocket();  // Handle planet destruction
	} else {
		
		FVector currentPosition = GetActorLocation();
		
		// Calculate the new momentum based on the current one and the time jump
		FVector New_p = p + (PrecomputedForce * GameState->SecondsInSimulation);
		
		// Calculate the new velocity
		Velocity = New_p / Mass;
		
		SetActorLocation(GetActorLocation() + (Velocity * GameState->SecondsInSimulation));
		
		// Remember the newly calculated momentum
		p = New_p;
		
		// Optional: Update rotation based on new position if necessary
		FRotator NewRotation = GetActorRotation();
//		float DeltaRotation = DeltaTime * RotationSpeed * GameState->SecondsInSimulation;
//		NewRotation.Yaw += DeltaRotation;
//		SetActorRotation(NewRotation);
		
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
		Velocity += Thrust / Mass;
	}
}

// Action binding function to toggle thruster
void ARocket::ToggleThruster()
{
	bIsThrusterActive = !bIsThrusterActive;
}
