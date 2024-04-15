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
	PrimaryActorTick.bCanEverTick = true;
	
	// Create and set up the rocket mesh
	RocketMesh = CreateDefaultSubobject<UStaticMeshComponent>("RocketMesh");
	RocketMesh->SetAbsolute(true, true, true);
	
	// Set up rocket info widget
	RocketInfoWidget = CreateDefaultSubobject<UWidgetComponent>("RocketInfoWidget");

	SetRootComponent(RocketMesh);

	// Initialize rocket properties
	RocketMass = 1.f;
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
	if (APracaInzGameState* PracaInzGameState = Cast<APracaInzGameState>(GetWorld()->GetGameState()))
	{
		InitialVelocity *= PracaInzGameState->BaseDistance;
		Velocity = InitialVelocity;
	}
	p = InitialVelocity * RocketMass;
	if (APracaInzGameModeBase* PracaInzGameModeBase = Cast<APracaInzGameModeBase>(GetWorld()->GetAuthGameMode()))
	{
		PracaInzGameModeBase->OnRocketCreate(this);
	}
	
	RocketInfoWidget->SetWorldScale3D(FVector(0.1, 0.1, 0.1));
	RocketInfoWidget->SetHiddenInGame(true);
	RocketInfoWidget->SetVisibility(false);

	
}

void ARocket::InitialCalculations(float DeltaTime)
{
	if (APracaInzGameState* PracaInzGameState = Cast<APracaInzGameState>(GetWorld()->GetGameState()))
	{
		FVector r;
		FVector F = FVector(0, 0, 0);
		double distance;
		for (APlanet* x : PracaInzGameState->Planets)
		{
			r = x->GetActorLocation() - GetActorLocation();
			distance = r.Size() * r.Size() * r.Size();
			F += (((RocketMass) * (x->PlanetMass)) / (distance)) * r;
		}
		F *= PracaInzGameState->G * DeltaTime * DeltaTime;
	}
}
// Called every frame
void ARocket::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
	
	if (bIsBeingDestroyed)
	{
		DestroyRocket();
		return;
	}
	
	if (bIsOverLapped && bIsThrusterActive)
	{
		//FireThruster();
	}
	
	if (APracaInzGameModeBase* PracaInzGameModeBase = Cast<APracaInzGameModeBase>(GetWorld()->GetAuthGameMode()))
	{
		if (!PracaInzGameModeBase->isEditMode)
		{
			if (bFirstCalculations)
			{
				bFirstCalculations = false;
				p *= DeltaTime;
				Velocity *= DeltaTime;
				FVector position = GetActorLocation();
				int degree = 0;
				position = position.RotateAngleAxis(degree, FVector(0, 0, 1));
				p = p.RotateAngleAxis(degree, FVector(0, 0, 1));
				Velocity = Velocity.RotateAngleAxis(degree, FVector(0, 0, 1));
				SetActorLocation(position);
				InitialCalculations(DeltaTime);
				startPosition = position;
				FVector x;
				if (APracaInzGameState* PracaInzGameState = Cast<APracaInzGameState>(GetWorld()->GetGameState()))
				{
					x = GetActorLocation() - PracaInzGameState->Planets[0]->GetActorLocation();
				}
				x.Normalize();
				p = p.RotateAngleAxis(Inclination, x);
				Velocity = Velocity.RotateAngleAxis(Inclination, x);
			}
			UpdateRocketPosition(DeltaTime);
		}
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


void ARocket::UpdateRocketPosition(float DeltaTime)
{
	// Access the game state
	APracaInzGameState* PracaInzGameState = Cast<APracaInzGameState>(GetWorld()->GetGameState());
	if (!PracaInzGameState)
	{
		return; // Early return if GameState is not found
	}
	
	// Find Earth and Mars in the planet list
	APlanet* Earth = nullptr;
	APlanet* Mars = nullptr;
	for (APlanet* Planet : PracaInzGameState->Planets) {
		if (Planet->Name == "Earth") {
			Earth = Planet;
		} else if (Planet->Name == "Mars") {
			Mars = Planet;
		}
	}
	
	if (!Earth || !Mars) {
		return; // Earth or Mars not found, cannot proceed with calculation
	}
	
	// Calculate the current positions of Earth and Mars
	FVector EarthPosition = Earth->GetActorLocation();
	FVector MarsPosition = Mars->GetActorLocation();
	
	// Calculate the vector from Earth to Mars
	FVector EarthToMars = MarsPosition - EarthPosition;
	
	// Calculate the phase angle between Earth and Mars
	double PhaseAngle = FMath::Acos(FVector::DotProduct(EarthToMars.GetSafeNormal(), -EarthPosition.GetSafeNormal())) * (180.0 / PI);

	
	if(!IsLaunchWindow(PhaseAngle) && !bLaunching){
		
		SetActorLocation(EarthPosition + FVector(-50, 0, 0));

		bLaunching = true;
		return;
	}
	
	FVector r;
	FVector F = FVector(0, 0, 0);
	double distance;
	FVector oldLocation = GetActorLocation();
	
	
	APlanet* centralPlanet = nullptr; // Variable to store the planet with the biggest gravity pull
	
	APlanet* star = nullptr; // Variable to store the planet with the biggest gravity pull
	
	// Find the planet with the biggest mass (central anchor)
	double maxMass = this->RocketMass;
	for (APlanet* x : PracaInzGameState->Planets)
	{
		if (x->Name == "Sun")
		{
			maxMass = x->PlanetMass;
			centralPlanet = x;
			break;
		}
	}
	
	for (APlanet* x : PracaInzGameState->Planets)
	{
		if (x->Name == "Sun")
		{
			star = x;
			break;
		}
	}
	
	if (centralPlanet == nullptr)
	{
		return;
	}
	
	const double G = 6.67430e-11 * 1.766;
	const double earthMass = 5.972e24 * 1.766; // Mass of Earth in kilograms
	
	double G_scaled = G;
	
	// Calculate the distance vector to the central anchor
	r = centralPlanet->GetActorLocation() - GetActorLocation();
	
	// Calculate the distance between this planet and the central anchor
	distance = r.SizeSquared();
	
	// Calculate the force acting on this planet from the central anchor
	F = (RocketMass * centralPlanet->PlanetMass) / distance * r.GetSafeNormal();
	
	float thrustAngle = 32; // Assuming thrust angle is -0.032 radians
	FVector thrustDirection = FVector(FMath::Cos(thrustAngle), 0.0f, 0.0f);
	FVector thrust = thrustDirection; // Calculate thrust vector
	
	// Add thrust to the force vector
	F += thrust * DeltaTime * DeltaTime;

	// Multiply the calculated force by the gravitational constant
	F *= G_scaled * DeltaTime * DeltaTime;
	
	// Calculate the new momentum based on the current one and the time jump
	FVector New_p = p + (F * PracaInzGameState->SecondsInSimulation);
	
	// Calculate the new velocity
	Velocity = New_p / RocketMass;
	
	// Calculate and set the planet in the new position based on the velocity and the time jump
	SetActorLocation(GetActorLocation() + (Velocity * PracaInzGameState->SecondsInSimulation));
	
	// Remember the newly calculated momentum
	p = New_p;
	
	// Update rotation
	//FRotator NewRotation = GetActorRotation();
	//	float DeltaRotation = DeltaTime * RotationSpeed * GameState->SecondsInSimulation;
	//	NewRotation.Yaw += DeltaRotation;
	//	SetActorRotation(NewRotation);
	
	// Draw debug line to visualize orbit
	DrawDebugLine(GetWorld(), oldLocation, GetActorLocation(), OrbitColor, false, 3);
	
	
	//	// Apply thrust if thruster is active
	//	if (bIsThrusterActive)
	//	{
	//		FVector Thrust = GetActorForwardVector() * ThrustForce * GameState->BaseDistance;
	//		F += Thrust / RocketMass; // Thrust divided by rocket mass gives acceleration
	//	}
	//
	//
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
		Velocity += Thrust / RocketMass;
	}
}

// Action binding function to toggle thruster
void ARocket::ToggleThruster()
{
	bIsThrusterActive = !bIsThrusterActive;
}
