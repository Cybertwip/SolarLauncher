// Fill out your copyright notice in the Description page of Project Settings.


#include "Planet.h"
#include "PracaInzGameModeBase.h"
#include "PracaInzGameState.h"
#include "GameFramework/SpringArmComponent.h"
#include "DrawDebugHelpers.h"
#include "PracaInzHUD.h"
#include "Camera\CameraComponent.h"
#include "Classes/Components/StaticMeshComponent.h"

// Sets default values
APlanet::APlanet()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	PlanetMesh = CreateDefaultSubobject<UStaticMeshComponent>("PlanetMesh");
	PlanetMesh->SetAbsolute(true, true, true);
	OnClicked.AddUniqueDynamic(this, &APlanet::OnSelected);
	SetRootComponent(PlanetMesh);
	InitialVelocity.X = 0.f;
	InitialVelocity.Y = 0.f;
	InitialVelocity.Z = 0.f;
	PlanetMass = 1;

}

// Called when the game starts or when spawned
void APlanet::BeginPlay()
{
	Super::BeginPlay();
	PlanetMesh->OnComponentBeginOverlap.AddDynamic(this, &APlanet::OnOverlapBegin);
	p = InitialVelocity * PlanetMass/60;
	if (APracaInzGameModeBase* PracaInzGameModeBase = Cast<APracaInzGameModeBase>(GetWorld()->GetAuthGameMode()))
	{
		PracaInzGameModeBase->OnPlanetCreate(this);
	}
	
}

// Called every frame
void APlanet::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
	UpdatePlanetPosition(DeltaTime);	
}

void APlanet::OnSelected(AActor* Target, FKey ButtonPressed)
{
	UE_LOG(LogTemp, Warning, TEXT("Name: %s!"), *Name);
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


void APlanet::OnOverlapBegin(UPrimitiveComponent* OverlappedComp, AActor* OtherActor, UPrimitiveComponent* OtherComp,
	int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	UE_LOG(LogTemp, Warning, TEXT("Overlap!"));
	if (APracaInzGameState* PracaInzGameState = Cast<APracaInzGameState>(GetWorld()->GetGameState()))
	{
		if (APlanet* Planet = Cast<APlanet>(OtherActor))
		{
			if (Planet->PlanetMass >= PlanetMass)
			{
				PracaInzGameState->CurrentPlanet = nullptr;
				PracaInzGameState->Planets.Remove(this);
				this->Destroy();
			}
			else
			{
				p += Planet->p;
			}
		}
	}
}

void APlanet::UpdatePlanetPosition(float DeltaTime)
{
	if (APracaInzGameState* PracaInzGameState = Cast<APracaInzGameState>(GetWorld()->GetGameState()))
	{
		FVector r;
		FVector F;
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
		FVector New_p1 = p + (F*PracaInzGameState->SecondsInSimulation);
		Velocity = New_p1 / PlanetMass;
		SetActorLocation(GetActorLocation() + Velocity*PracaInzGameState->SecondsInSimulation);
		p = New_p1;
		/*
		UE_LOG(LogTemp, Warning, TEXT("Name: %s!"), *Name);
		UE_LOG(LogTemp, Warning, TEXT("Location: %s!"), *GetActorLocation().ToString());
		UE_LOG(LogTemp, Warning, TEXT("Velocity: %s!"), *Velocity.ToString());
		UE_LOG(LogTemp, Warning, TEXT("Force: %s!"), *F.ToString());
		UE_LOG(LogTemp, Warning, TEXT("Rotation: %s!"), *PlanetMesh->GetComponentRotation().ToString());
		UE_LOG(LogTemp, Warning, TEXT("DeltaTime: %s!"), *FString::SanitizeFloat(DeltaTime));
		*/
		FRotator NewRotation =GetActorRotation();
		float DeltaRotation = DeltaTime * RotationSpeed;
		NewRotation.Yaw += DeltaRotation;
		SetActorRotation(NewRotation);
		DrawDebugPoint(GetWorld(), GetActorLocation(), 2, FColor(255, 255, 255), false, 3);
	}
}


