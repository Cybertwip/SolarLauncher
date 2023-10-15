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
	p = FVector(0, 0, 0);
	PlanetMass = 1;
	Diameter = 1;
	Inclination = 0;
	
}

// Called when the game starts or when spawned
void APlanet::BeginPlay()
{
	Super::BeginPlay();
	PlanetMesh->OnComponentBeginOverlap.AddDynamic(this, &APlanet::OnOverlapBegin);
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
	Super::Tick(DeltaTime);
	if (APracaInzGameState* PracaInzGameState = Cast<APracaInzGameState>(GetWorld()->GetGameState()))
	{
		PracaInzGameState->CurrentDeltaTime = DeltaTime;
	}	  
	if (bFirstCalculations)
	{
		bFirstCalculations = false;	
		p *=DeltaTime;
		Velocity *= DeltaTime;
		FVector position = GetActorLocation();
		int degree = FMath::FRandRange(0, 360);
		position = position.RotateAngleAxis(degree, FVector(0, 0, 1));
		p = p.RotateAngleAxis(degree, FVector(0, 0, 1));
		Velocity = Velocity.RotateAngleAxis(degree, FVector(0, 0, 1));
		SetActorLocation(position);
		InitialCalculations(DeltaTime);
		startPosition = position;
		startP = p;
		startVelocity = Velocity;
		FVector x;
		if (APracaInzGameState* PracaInzGameState = Cast<APracaInzGameState>(GetWorld()->GetGameState()))
		{
			x = GetActorLocation() - PracaInzGameState->Planets[0]->GetActorLocation();
		}
		x.Normalize();
		p = p.RotateAngleAxis(Inclination, x);
		Velocity = Velocity.RotateAngleAxis(Inclination, x);
	}
	if (bIsBeingDestroyed)
	{
		DestroyPlanet();
	}
	UpdatePlanetPosition(DeltaTime);
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


void APlanet::OnOverlapBegin(UPrimitiveComponent* OverlappedComp, AActor* OtherActor, UPrimitiveComponent* OtherComp,
	int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	if (APracaInzGameState* PracaInzGameState = Cast<APracaInzGameState>(GetWorld()->GetGameState()))
	{
		if (APlanet* Planet = Cast<APlanet>(OtherActor))
		{
			/*...*/
			if (Planet->PlanetMass > PlanetMass)
			{
				if(PracaInzGameState->CurrentPlanet==this)
				{ 
					PracaInzGameState->Camera->Focused = Cast<USceneComponent>(Planet->PlanetMesh);
					PracaInzGameState->CurrentPlanet = Planet;
					if (APracaInzHUD* PracaInzHUD = Cast<APracaInzHUD>(GetWorld()->GetFirstPlayerController()->GetHUD()))
					{
						PracaInzHUD->UpdatePlanetInfo(Planet);
					}
					PracaInzGameState->Camera->bOnChangePlanet = true;
				}
				PracaInzGameState->Planets.Remove(this);
				bIsBeingDestroyed = true;
			}
			/*...*/
			else if(Planet->PlanetMass < PlanetMass)
			{
				p += Planet->p;
				double mass = PlanetMass + Planet->PlanetMass;
				for (int i = 0; i != PracaInzGameState->Planets.Num(); i++)
				{
					APlanet* x = PracaInzGameState->Planets[i];
					if (fabs(x->PlanetMass - mass) <= 0.0000001 * fabs(x->PlanetMass) && x != Planet)
					{
						mass = mass + 0.0001;
						i = -1;
					}
				}
				PlanetMass = mass;
				if (PracaInzGameState->CurrentPlanet == this)
				{
					if (APracaInzHUD* PracaInzHUD = Cast<APracaInzHUD>(GetWorld()->GetFirstPlayerController()->GetHUD()))
					{
						PracaInzHUD->UpdatePlanetInfo(this);
					}
				}
			}
		}
	}
}

void APlanet::UpdatePlanetPosition(float DeltaTime)
{
	if (APracaInzGameState* PracaInzGameState = Cast<APracaInzGameState>(GetWorld()->GetGameState()))
	{
		FVector r;
		FVector F = FVector(0,0,0);
		double distance;
		FVector oldLocation = GetActorLocation();
		/*
		petla dla kazdego obiektu, ktory znajduje sie w ukladzie wspolrzednych
		*/
		for (APlanet* x : PracaInzGameState->Planets)
		{
			/*
			pominiecie planety, na rzecz ktorej wykonywane sa obliczenia
			*/
			if (x == this)
			{
				continue;
			}
			else
			{
				/*
				wyznaczenie wektora odleglosci
				*/
				r = x->GetActorLocation() - GetActorLocation();
				/*
				wyznaczenie odleglosci pomiedzy obiektami
				*/
				distance = r.Size() * r.Size() * r.Size();
				/*
				wyznaczenie sily dzialajecej na dany obiekt od znajdujacego sie w ukladzie wspolrzednych
				*/
				F += (((PlanetMass) * (x->PlanetMass)) / (distance)) * r;
			}
		}
		/*
		pomnozenie wyliczonej sily przez stala grawitacji, w tym miejscu nastepuje mnozenie przez 
		kwadrat czasu, ktory uplywa mniedzy klatkami sekund, poniewaz jednostki czasu w stalej grawitacji sa 
		w kwadracie
		*/
		F *= PracaInzGameState->G * DeltaTime * DeltaTime;
		/*
		obliczenie nowego pedu na podstawie aktualnego oraz skoku czasowego
		*/
		FVector New_p = p + (F * PracaInzGameState->SecondsInSimulation);
		/*
		obliczenie nowej predkosci
		*/
		Velocity = New_p / PlanetMass;
		/*
		obliczenie i ustawienie planety w nowej pozycji na podstawie predkosci i skoku czasowego
		*/
		SetActorLocation(GetActorLocation() + (Velocity * PracaInzGameState->SecondsInSimulation));
		/*
		zapamietanie nowo obliczonego pedu
		*/
		p = New_p;
		FRotator NewRotation = GetActorRotation();
		float DeltaRotation = DeltaTime * RotationSpeed * PracaInzGameState->SecondsInSimulation;
		NewRotation.Yaw += DeltaRotation;
		SetActorRotation(NewRotation);
		DrawDebugLine(GetWorld(), oldLocation, GetActorLocation(), OrbitColor, false, 3);
	}
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
		a = F / PlanetMass;
	}
}


