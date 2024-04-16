// Fill out your copyright notice in the Description page of Project Settings.

//#pragma once

#include "Planet.h"
#include "Rocket.h"
#include "CameraPawn.h"
#include "CoreMinimal.h"
#include "HttpModule.h"
#include "Interfaces/IHttpRequest.h"
#include "Interfaces/IHttpResponse.h"
#include "XmlParser.h"
#include "JsonObjectConverter.h"

#include "GameFramework/GameStateBase.h"
#include "PracaInzGameState.generated.h"

struct FPlanetData
{
	FString Name;
	
	double Mass; // Placeholder since mass is not given
	
	double Radius; // Placeholder since radius is not given
	
	double Distance; // Using Perihelion distance as a proxy for distance
};

/**
 *
 */
UCLASS()
class PRACAINZ_API APracaInzGameState : public AGameStateBase
{
	GENERATED_BODY()
public:
	APracaInzGameState();
	virtual void BeginPlay() override;
	virtual void Tick(float DeltaSeconds) override; // Override the Tick function
	void SelectNextPlanet();
	void SelectPreviousPlanet();
	void SelectRocket();

	TArray<APlanet*> Planets;
	TArray<AstralObject*> Objects;
	ACameraPawn* Camera;
	APlanet* CurrentPlanet;
	ARocket* Rocket;
	//base mass = moon mass
	//base distance = 100 000 km
	UPROPERTY(EditAnywhere, Category = "Components")
	double BaseDistance = 1/(1.5E5 * 1.766);
	const double G = 6.67430e-11 * 1.766;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Orbit")
	FVector CentralOrbitPoint = FVector(0.0f, 0.0f, 0.0f);
	
	//		int64 SecondsInSimulation = 86400;
	int64 SecondsInSimulation = 16640000;
	//int64 SecondsInSimulation = 166400;
	double CurrentDeltaTime = 1/60;
	
	
	//VR
	APlanet* CurrentPlanetVR;
	FVector CurrentPlanetVRScale;
	
	double TimeSinceStart = 0.0f;
	
private:
	void ParseJsonData(const FString& JsonData);
	void FetchPlanetData();
	void OnHttpResponseReceived(FHttpRequestPtr Request, FHttpResponsePtr Response, bool bWasSuccessful);
	void ProcessXmlData(const FString& XmlData);
	void SpawnPlanetFromJsonData(const FPlanetData& PlanetData);
	void SpawnPlanetFromXmlData(const FString& Name, double Mass, double Radius, double Inclination, double distance);
	
	APlanet* earth = nullptr; // Variable to store the planet with the biggest gravity pull
	
	APlanet* star = nullptr; // Variable to store the planet with the biggest gravity pull
	
	TMap<AstralObject*, FVector> AstralForces;


};
