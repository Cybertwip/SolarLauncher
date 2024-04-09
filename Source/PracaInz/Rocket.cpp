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
	SetRootComponent(RocketMesh);
	
	// Set up rocket info widget
	RocketInfoWidget = CreateDefaultSubobject<UWidgetComponent>("RocketInfoWidget");
	RocketInfoWidget->SetupAttachment(RootComponent);
	
	// Initialize rocket properties
	RocketMass = 1.f;
	InitialVelocity = FVector::ZeroVector;
	ThrustForce = 1000.f;
	MaxSpeed = 5000.f;
	RocketColor = FColor::White;
	
	// Set initial state
	bIsThrusterActive = true; // Start with thruster active
	
	// Set rocket mesh
	static ConstructorHelpers::FObjectFinder<UStaticMesh> RocketMeshAsset(TEXT("/Game/Path/To/Your/RocketMesh"));
	if (RocketMeshAsset.Succeeded())
	{
		RocketMesh->SetStaticMesh(RocketMeshAsset.Object);
	}
}

// Called when the game starts or when spawned
void ARocket::BeginPlay()
{
	Super::BeginPlay();
	
	// Set initial velocity
	Velocity = InitialVelocity;
	
	// Calculate initial momentum
	p = RocketMass * InitialVelocity;
	
	// Set initial position
	startPosition = GetActorLocation();
	
	if (APracaInzGameModeBase* PracaInzGameModeBase = Cast<APracaInzGameModeBase>(GetWorld()->GetAuthGameMode()))
	{
		PracaInzGameModeBase->OnRocketCreate(this);
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
		FireThruster();
	}
	
	UpdateRocketPosition(DeltaTime);
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
void ARocket::UpdateRocketPosition(float DeltaTime)
{
	// Calculate the force acting on the rocket
	FVector F = FVector::ZeroVector;
	
	// Get the list of planets from the game state
	APracaInzGameState* GameState = Cast<APracaInzGameState>(GetWorld()->GetGameState());
	if (GameState)
	{
		// Access the planets array
		TArray<APlanet*> PlanetsArray = GameState->Planets;
		
		// Calculate the total gravitational force from all planets
		for (APlanet* Planet : PlanetsArray)
		{
			if (Planet)
			{
				// Calculate the distance vector to the planet
				FVector r = Planet->GetActorLocation() - GetActorLocation();
				
				// Calculate the distance between the rocket and the planet
				float distance = r.SizeSquared();
				
				// Calculate the force acting on the rocket from the planet
				FVector forceDirection = r.GetSafeNormal();
				
				// Adjust the force direction based on inclination between Earth and Mars
				if (Planet->Name == "Mars")
				{
					FVector inclinationDirection = FVector(0.0f, FMath::Cos(0.032f), FMath::Sin(0.032f)); // Assuming the inclination angle is 0.032 radians
					forceDirection = FMath::Lerp(forceDirection, inclinationDirection, 0.5f); // Adjusting force direction based on inclination
				}
				
				F += (Planet->PlanetMass * RocketMass) / distance * forceDirection;
			}
		}
		
		// Apply thrust force to rocket if thruster is active
		if (bIsThrusterActive)
		{
			F += GetActorForwardVector() * ThrustForce * GameState->BaseDistance;
		}
		
		F *= GameState->SecondsInSimulation;
	}
	
	// Calculate the new momentum based on the current one and the time jump
	FVector New_p = p + (F * DeltaTime);
	
	// Calculate the new velocity
	Velocity = New_p / RocketMass;
	
	// Calculate and set the rocket in the new position based on the velocity and the time jump
	FVector NewLocation = GetActorLocation() + (Velocity * DeltaTime);
	SetActorLocation(NewLocation);
	
	// Remember the newly calculated momentum
	p = New_p;
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
