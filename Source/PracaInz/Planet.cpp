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
	/*
	sprawdzenie, czy wykonac pierwsze obliczenia
	*/
	if (bFirstCalculations)
	{
		/*
		ustawienie zmiennej sprawdzajacej na false, aby nie wykonywac tych obliczen wiele razy
		*/
		bFirstCalculations = false;
		/*
		aby zachowac skale 1s nalezy pomnozyc wszytkie wartosci, w ktorych jest 
		jednostka czasu przez ilosc czasu uplywajaca miedzy klatkami,
		w tym przypadku jest to ped
		*/
		p *=DeltaTime;		/*		jak wyzej		*/		Velocity *= DeltaTime;
		InitialCalculations(DeltaTime);
		FVector x;
		if (APracaInzGameState* PracaInzGameState = Cast<APracaInzGameState>(GetWorld()->GetGameState()))
		{
			x = GetActorLocation() - PracaInzGameState->Planets[0]->GetActorLocation();
		}
		UE_LOG(LogTemp, Warning, TEXT("Location before: %s!"), *x.ToString());
		x = x.GetSafeNormal();
		p = p.RotateAngleAxis(45, FVector(1, 0, 0));
		Velocity = Velocity.RotateAngleAxis(45, FVector(1, 0, 0));
		UE_LOG(LogTemp, Warning, TEXT("Location after: %s!"), *x.ToString());
		//SetActorLocation(GetActorLocation().RotateAngleAxis(90, FVector(0, 0, 1)));
		//p = p.RotateAngleAxis(90, FVector(0, 0, 1));
		//Velocity = Velocity.RotateAngleAxis(90, FVector(0, 0, 1));
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
			if (Planet->PlanetMass > PlanetMass)
			{
				PracaInzGameState->CurrentPlanet = nullptr;
				PracaInzGameState->Planets.Remove(this);
				PracaInzGameState->Camera->Focused = Cast<USceneComponent>(Planet->PlanetMesh);
				PracaInzGameState->CurrentPlanet = Planet;
				if (APracaInzHUD* PracaInzHUD = Cast<APracaInzHUD>(GetWorld()->GetFirstPlayerController()->GetHUD()))
				{
					PracaInzHUD->UpdatePlanetInfo(Planet);
				}
				PracaInzGameState->Camera->bOnChangePlanet = true;
				bIsBeingDestroyed = true;
			}
			else if(Planet->PlanetMass < PlanetMass)
			{
				p += Planet->p;
				PlanetMass += Planet->PlanetMass;
				if (APracaInzHUD* PracaInzHUD = Cast<APracaInzHUD>(GetWorld()->GetFirstPlayerController()->GetHUD()))
				{
					PracaInzHUD->UpdatePlanetInfo(this);
				}
			}
			else
			{
				PracaInzGameState->Planets.Remove(this);
				PracaInzGameState->Planets.Remove(Planet);
				Planet->bIsBeingDestroyed = true;
				bIsBeingDestroyed = true;
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
		
#define _EULER_
#ifndef _EULER_		
		/*	LeapFrog */
		F *= PracaInzGameState->G * DeltaTime * DeltaTime;
		FVector New_a = F / PlanetMass;
		SetActorLocation(GetActorLocation() + Velocity * PracaInzGameState->SecondsInSimulation + a *
			PracaInzGameState->SecondsInSimulation * PracaInzGameState->SecondsInSimulation * 0.5);
		Velocity = Velocity + (a + New_a)*PracaInzGameState->SecondsInSimulation*0.5;
		a = New_a;
#else
		/*Euler*/
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
#endif
		/*
		UE_LOG(LogTemp, Warning, TEXT("Name: %s!"), *Name);
		UE_LOG(LogTemp, Warning, TEXT("Location: %s!"), *GetActorLocation().ToString());
		UE_LOG(LogTemp, Warning, TEXT("Velocity: %s!"), *Velocity.ToString());
		UE_LOG(LogTemp, Warning, TEXT("Force: %s!"), *F.ToString());
		UE_LOG(LogTemp, Warning, TEXT("Rotation: %s!"), *PlanetMesh->GetComponentRotation().ToString());
		UE_LOG(LogTemp, Warning, TEXT("DeltaTime: %s!"), *FString::SanitizeFloat(DeltaTime));
		*/
		FRotator NewRotation = GetActorRotation();
		float DeltaRotation = DeltaTime * RotationSpeed;
		NewRotation.Yaw += DeltaRotation;
		SetActorRotation(NewRotation);
		DrawDebugPoint(GetWorld(), GetActorLocation(), 2, FColor(255, 255, 255), false, 3);
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


