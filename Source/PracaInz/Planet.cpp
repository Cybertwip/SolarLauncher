// Fill out your copyright notice in the Description page of Project Settings.


#include "Planet.h"
#include "PracaInzGameModeBase.h"
#include "PracaInzGameState.h"
#include "GameFramework/SpringArmComponent.h"
#include "DrawDebugHelpers.h"
#include "PracaInzHUD.h"
#include "Camera/CameraComponent.h"
#include "Classes/Components/StaticMeshComponent.h"
#include "Kismet/GameplayStatics.h"
#include "Kismet/KismetMathLibrary.h"
#include "Components/WidgetComponent.h"
#include "UObject/ConstructorHelpers.h" 

// Sets default values
APlanet::APlanet()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	PlanetMesh = CreateDefaultSubobject<UStaticMeshComponent>("PlanetMesh");
	PlanetMesh->SetAbsolute(true, true, true);
	OnClicked.AddUniqueDynamic(this, &APlanet::OnSelected);
	
	SetRootComponent(PlanetMesh);
	
	static ConstructorHelpers::FObjectFinder<UStaticMesh> SphereMeshAsset(TEXT("/Engine/BasicShapes/Sphere"));
	if (SphereMeshAsset.Succeeded())
	{
		// Set the mesh for the PlanetMesh component
		PlanetMesh->SetStaticMesh(SphereMeshAsset.Object);
	}
	else
	{
		UE_LOG(LogTemp, Error, TEXT("Failed to load default sphere mesh for PlanetMesh."));
	}
	
	// Load and apply the Sun material
	static ConstructorHelpers::FObjectFinder<UMaterialInstance> SunMaterialAsset(TEXT("/Engine/TemplateResources/MI_Template_BaseOrange_Metal"));
	if (SunMaterialAsset.Succeeded())
	{
		// Set the material for the PlanetMesh component
		PlanetMesh->SetMaterial(0, SunMaterialAsset.Object);
	}
	else
	{
		UE_LOG(LogTemp, Error, TEXT("Failed to load Sun material for PlanetMesh."));
	}

	InitialVelocity.X = 0.f;
	InitialVelocity.Y = 0.f;
	InitialVelocity.Z = 0.f;
	p = FVector(0, 0, 0);
	PlanetMass = 1;
	Diameter = 1;
	Inclination = 0;
	
	OrbitColor = FColor(1.0, 1.0, 0.0, 1.0);
}

// Called when the game starts or when spawned
void APlanet::BeginPlay()
{
	Super::BeginPlay();


//	PlanetMesh->OnComponentBeginOverlap.AddDynamic(this, &APlanet::OnOverlapBegin);
//	PlanetMesh->OnComponentEndOverlap.AddDynamic(this, &APlanet::OnOverlapEnd);
	PlanetMesh->OnComponentHit.AddUniqueDynamic(this, &APlanet::OnHit);
	PlanetMesh->SetWorldScale3D(FVector(Diameter, Diameter, Diameter));


	if (APracaInzGameState* PracaInzGameState = Cast<APracaInzGameState>(GetWorld()->GetGameState()))
	{
		InitialVelocity *= PracaInzGameState->BaseDistance; 
		Velocity = InitialVelocity;
	}
	p = InitialVelocity * PlanetMass;
	if (APracaInzGameModeBase* PracaInzGameModeBase = Cast<APracaInzGameModeBase>(GetWorld()->GetAuthGameMode()))
	{
		PracaInzGameModeBase->OnPlanetCreate(this);
	}
	
}

// Called every frame
void APlanet::Tick(float DeltaTime)
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
	
	// Set current delta time in game state
	GameState->CurrentDeltaTime = DeltaTime;
	
	// Perform first-time calculations
	if (bFirstCalculations)
	{
		PerformInitialCalculations(DeltaTime, GameState);
		bFirstCalculations = false;  // Ensure this block runs only once
	}
	
	// Update planet position if it's not being destroyed
	if (!bIsBeingDestroyed)
	{
		UpdatePlanetPosition(DeltaTime);
	}
	else
	{
		DestroyPlanet();  // Handle planet destruction
	}
}

void APlanet::PerformInitialCalculations(float DeltaTime, APracaInzGameState* GameState)
{
	FVector currentPosition = GetActorLocation();
	FVector referenceDirection = FVector(0, 0, 1);  // Typically the 'up' direction in UE
	
	// Apply initial transformations and calculations
	currentPosition = currentPosition.RotateAngleAxis(0, referenceDirection);  // Rotate by 0 degrees, consider dynamic rotation later
	p *= DeltaTime;
	Velocity *= DeltaTime;
	p = p.RotateAngleAxis(0, referenceDirection);
	Velocity = Velocity.RotateAngleAxis(0, referenceDirection);
	SetActorLocation(currentPosition);
	
	// Calculate inclination effects
	FVector relativeDirection = (GameState->Planets.Num() > 0) ? (currentPosition - GameState->Planets[0]->GetActorLocation()) : FVector::UpVector;
	relativeDirection.Normalize();
	p = p.RotateAngleAxis(Inclination, relativeDirection);
	Velocity = Velocity.RotateAngleAxis(Inclination, relativeDirection);
}


void APlanet::OnSelected(AActor* Target, FKey ButtonPressed)
{
	if (APracaInzGameState* PracaInzGameState = Cast<APracaInzGameState>(GetWorld()->GetGameState()))
	{
		PracaInzGameState->Camera->Focused = Cast<USceneComponent>(PlanetMesh);
		PracaInzGameState->CurrentPlanet = this;
		if (APracaInzHUD* PracaInzHUD = Cast<APracaInzHUD>(GetWorld()->GetFirstPlayerController()->GetHUD()))
		{
			PracaInzHUD->UpdatePlanetInfo(this);
		}
		PracaInzGameState->Camera->bOnChangePlanet = true;
	}
}
//
//void APlanet::OnOverlapEnd(UPrimitiveComponent* OverlappedComp, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex)
//{
//	bIsBindedToPlayerInput = false;
//	bIsOverLapped = false;
//	if(inputActionBindingLeft)
//		InputComponent->RemoveActionBindingForHandle(inputActionBindingLeft->GetHandle());
//	if (inputActionBindingRight)
//		InputComponent->RemoveActionBindingForHandle(inputActionBindingRight->GetHandle());
//	/*if (GEngine)
//		GEngine->AddOnScreenDebugMessage(-1, 5, FColor::Yellow, TEXT("Overlap end"));*/
//}
//
//void APlanet::OnOverlapBegin(UPrimitiveComponent* OverlappedComp, AActor* OtherActor, UPrimitiveComponent* OtherComp,
//	int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
//{
//	if (APracaInzGameModeBase* PracaInzGameModeBase = Cast<APracaInzGameModeBase>(GetWorld()->GetAuthGameMode()))
//	{
//		if (OtherActor->ActorHasTag("VR") && PracaInzGameModeBase->isEditMode)
//		{
//			bIsOverLapped = true;
//			//if (GEngine)
//			//	GEngine->AddOnScreenDebugMessage(-1, 5, FColor::Green, FString::Printf(TEXT("Hello %s"), *OtherActor->GetActorLabel()));
//
//			if (!bIsBindedToPlayerInput)
//			{
//				bIsBindedToPlayerInput = true;
//				EnableInput(GetWorld()->GetFirstPlayerController());
//				if (InputComponent)
//				{
//					// Use BindAction or BindAxis on the InputComponent to establish the bindings
//					inputActionBindingLeft = &InputComponent->BindAction(FName("TriggerLeft"), IE_Pressed, this, &APlanet::OnTriggerPressed);
//					inputActionBindingRight = &InputComponent->BindAction(FName("TriggerRight"), IE_Pressed, this, &APlanet::OnTriggerPressed);
//				}
//			}
//			//PlanetMesh->SetRenderCustomDepth(true); // - for glow effect
//		}
//	}
//	if (APracaInzGameState* PracaInzGameState = Cast<APracaInzGameState>(GetWorld()->GetGameState()))
//	{
//		if (APlanet* Planet = Cast<APlanet>(OtherActor))
//		{
//			/*...*/
//			if (Planet->PlanetMass > PlanetMass)
//			{
//				if(PracaInzGameState->CurrentPlanet==this)
//				{ 
//					PracaInzGameState->Camera->Focused = Cast<USceneComponent>(Planet->PlanetMesh);
//					PracaInzGameState->CurrentPlanet = Planet;
//					if (APracaInzHUD* PracaInzHUD = Cast<APracaInzHUD>(GetWorld()->GetFirstPlayerController()->GetHUD()))
//					{
//						PracaInzHUD->UpdatePlanetInfo(Planet);
//					}
//					PracaInzGameState->Camera->bOnChangePlanet = true;
//				}
//				PracaInzGameState->Planets.Remove(this);
//				bIsBeingDestroyed = true;
//			}
//			/*...*/
//			else if(Planet->PlanetMass < PlanetMass)
//			{
//				p += Planet->p;
//				double mass = PlanetMass + Planet->PlanetMass;
//				for (int i = 0; i != PracaInzGameState->Planets.Num(); i++)
//				{
//					APlanet* x = PracaInzGameState->Planets[i];
//					if (fabs(x->PlanetMass - mass) <= 0.0000001 * fabs(x->PlanetMass) && x != Planet)
//					{
//						mass = mass + 0.0001;
//						i = -1;
//					}
//				}
//				PlanetMass = mass;
//				if (PracaInzGameState->CurrentPlanet == this)
//				{
//					if (APracaInzHUD* PracaInzHUD = Cast<APracaInzHUD>(GetWorld()->GetFirstPlayerController()->GetHUD()))
//					{
//						PracaInzHUD->UpdatePlanetInfo(this);
//					}
//				}
//			}
//		}
//	}
//}
void APlanet::UpdatePlanetPosition(float DeltaTime)
{
	APracaInzGameState* PracaInzGameState = Cast<APracaInzGameState>(GetWorld()->GetGameState());
	if (!PracaInzGameState)
	{
		return; // Early exit if the cast fails or game state is not accessible
	}
	
	FVector totalForce = FVector(0, 0, 0);
	FVector currentPosition = GetActorLocation();
	
	const double G = 6.67430e-11 * 1.766;

	// Iterate over all planets to calculate gravitational forces
	for (APlanet* otherPlanet : PracaInzGameState->Planets)
	{
		if (otherPlanet == this)
		{
			continue; // Skip self in force calculations
		}

		FVector r = otherPlanet->GetActorLocation() - GetActorLocation();
		
		float distanceSquared = r.SizeSquared();
		
		if (distanceSquared < KINDA_SMALL_NUMBER) // Check to avoid division by zero
		{
			continue; // Skip this iteration to avoid infinite forces
		}

		// Calculate the force acting on this planet from the central anchor

//		double distance = FMath::Sqrt(distanceSquared);
//		FVector forceDirection = r / distance;
//		double forceMagnitude = (G * PlanetMass * otherPlanet->PlanetMass) / distanceSquared;
//		FVector gravitationalForce = forceDirection * forceMagnitude;
		
		FVector F = (PlanetMass * otherPlanet->PlanetMass) / distanceSquared * r.GetSafeNormal();

		F *= G * DeltaTime * DeltaTime;

		totalForce += F; // Accumulate forces from all planets
	}
	
	// Calculate the force acting on this planet from the central anchor
	FVector F = totalForce;
		
	// Calculate the new momentum based on the current one and the time jump
	FVector New_p = p + (F * PracaInzGameState->SecondsInSimulation);
	
	// Calculate the new velocity
	Velocity = New_p / PlanetMass;
	
	// Calculate and set the planet in the new position based on the velocity and the time jump
	SetActorLocation(GetActorLocation() + (Velocity * PracaInzGameState->SecondsInSimulation));
	
	// Remember the newly calculated momentum
	p = New_p;
	
	// Optional: Update rotation based on new position if necessary
	FRotator NewRotation = GetActorRotation();
	float DeltaRotation = DeltaTime * RotationSpeed * PracaInzGameState->SecondsInSimulation;
	NewRotation.Yaw += DeltaRotation;
	SetActorRotation(NewRotation);

	// Debugging: Draw a line from the old position to the new position
	DrawDebugLine(GetWorld(), currentPosition, GetActorLocation(), FColor::Green, false, 3);
}

void APlanet::DestroyPlanet()
{
	Destroy();
}

void APlanet::InitialCalculations(float DeltaTime)
{
	if (APracaInzGameState* PracaInzGameState = Cast<APracaInzGameState>(GetWorld()->GetGameState()))
	{
		FVector r;
		FVector F = FVector(0, 0, 0);
		double distance;
		for (APlanet* x : PracaInzGameState->Planets)
		{
			if (x == this)
			{
				continue;
			}
			else
			{
				r = x->GetActorLocation() - GetActorLocation();
				distance = r.Size() * r.Size() * r.Size();
				F += (((PlanetMass) * (x->PlanetMass)) / (distance)) * r;
			}
		}
		F *= PracaInzGameState->G * DeltaTime * DeltaTime;
	}
}
FRotator MyLookRotation(FVector forward, FVector upDirection)
{

	FVector up = upDirection;


	forward = forward.GetSafeNormal();
	up = up - (forward * FVector::DotProduct(up, forward));
	up = up.GetSafeNormal();

	///////////////////////


	FVector vector = forward.GetSafeNormal();
	FVector vector2 = FVector::CrossProduct(up, vector);
	FVector vector3 = FVector::CrossProduct(vector, vector2);
	float m00 = vector.X;
	float m01 = vector.Y;
	float m02 = vector.Z;
	float m10 = vector2.X;
	float m11 = vector2.Y;
	float m12 = vector2.Z;
	float m20 = vector3.X;
	float m21 = vector3.Y;
	float m22 = vector3.Z;

	float num8 = (m00 + m11) + m22;
	FQuat quaternion = FQuat();

	if (num8 > 0.0f)
	{
		float num = (float)FMath::Sqrt(num8 + 1.0f);
		quaternion.W = num * 0.5f;
		num = 0.5f / num;
		quaternion.X = (m12 - m21) * num;
		quaternion.Y = (m20 - m02) * num;
		quaternion.Z = (m01 - m10) * num;
		return FRotator(quaternion);
	}

	if ((m00 >= m11) && (m00 >= m22))
	{
		float num7 = (float)FMath::Sqrt(((1.0f + m00) - m11) - m22);
		float num4 = 0.5f / num7;
		quaternion.X = 0.5f * num7;
		quaternion.Y = (m01 + m10) * num4;
		quaternion.Z = (m02 + m20) * num4;
		quaternion.W = (m12 - m21) * num4;
		return FRotator(quaternion);
	}

	if (m11 > m22)
	{
		float num6 = (float)FMath::Sqrt(((1.0f + m11) - m00) - m22);
		float num3 = 0.5f / num6;
		quaternion.X = (m10 + m01) * num3;
		quaternion.Y = 0.5f * num6;
		quaternion.Z = (m21 + m12) * num3;
		quaternion.W = (m20 - m02) * num3;
		return FRotator(quaternion);
	}

	float num5 = (float)FMath::Sqrt(((1.0f + m22) - m00) - m11);
	float num2 = 0.5f / num5;
	quaternion.X = (m20 + m02) * num2;
	quaternion.Y = (m21 + m12) * num2;
	quaternion.Z = 0.5f * num5;
	quaternion.W = (m01 - m10) * num2;


	return FRotator(quaternion);
}
void APlanet::OnTriggerPressed()
{
	if (APracaInzGameModeBase* PracaInzGameModeBase = Cast<APracaInzGameModeBase>(GetWorld()->GetAuthGameMode()))
	{
		if (bIsOverLapped && PracaInzGameModeBase->isEditMode && !PracaInzGameModeBase->isPlanetSelected
			&& !PracaInzGameModeBase->isMenuOpen)
		{
			PracaInzGameModeBase->isPlanetSelected = true;
			if (APracaInzGameState* PracaInzGameState = Cast<APracaInzGameState>(GetWorld()->GetGameState()))
			{
				PracaInzGameState->CurrentPlanetVR = this;
				PracaInzGameState->CurrentPlanetVRScale = PlanetMesh->GetComponentScale();
				for (auto planet : PracaInzGameState->Planets)
				{
					planet->bIsPaused = true;
				}
			}
			//PlanetMesh->SetVisibility(false);
			UWorld* world = GetWorld();
			FVector Direction = world->GetFirstPlayerController()->GetPawn()->GetActorLocation() - GetActorLocation();
			float scale = Direction.Size() * 0.001;
			PlanetMesh->SetWorldScale3D(FVector(0.1, 0.1, 0.1));
			/*if (GEngine)
				GEngine->AddOnScreenDebugMessage(-1, 10, FColor::Green, FString::Printf(TEXT("Hello %s"), *world->GetFirstPlayerController()->GetPawn()->GetActorLocation().ToString()));*/
			//planetInfoWidget->SetWorldRotation(FRotator((0, 0, 0)));
			//FRotator newRot = UKismetMathLibrary::FindLookAtRotation(GetActorLocation(), world->GetFirstPlayerController()->GetPawn()->GetActorLocation());
			//FRotator newRot = (world->GetFirstPlayerController()->GetPawn()->GetActorLocation() - GetActorLocation()).Rotation();
			//FQuat newRot = MyLookAt(world->GetFirstPlayerController()->GetPawn()->GetActorLocation(), FVector((0,0,1)));
			
			//FRotator Rot = FRotationMatrix::MakeFromX(world->GetFirstPlayerController()->GetPawn()->GetActorLocation() - GetActorLocation()).Rotator();
			//planetInfoWidget->AddWorldRotation(newRot);
			//MyLookAt()
			FVector Forward = world->GetFirstPlayerController()->GetPawn()->GetActorLocation() - GetActorLocation();
			FRotator Rot = UKismetMathLibrary::MakeRotFromXZ(Forward, FVector::UpVector);
			auto x = MyLookRotation(Direction, world->GetFirstPlayerController()->GetPawn()->GetActorUpVector());
			//SetActorRotation(x);
		}
	}
}

void APlanet::OnXButtonPressed()
{
	/*if (GEngine)
		GEngine->AddOnScreenDebugMessage(-1, 15.0f, FColor::Yellow, TEXT("On Hit"));*/
}

void APlanet::OnHit(UPrimitiveComponent* HitComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, FVector normalImpulse, const FHitResult& Hit)
{
	/*if (GEngine)
		GEngine->AddOnScreenDebugMessage(-1, 10.0f, FColor::Yellow, TEXT("On XButton pressed"));*/
}

