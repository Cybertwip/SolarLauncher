// PracaInzGameState.h
#pragma once

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
	double Mass;
	double Radius;
	double Distance;
};

UCLASS()
class PRACAINZ_API APracaInzGameState : public AGameStateBase
{
	GENERATED_BODY()
	
public:
	APracaInzGameState();
	virtual void BeginPlay() override;
	virtual void Tick(float DeltaSeconds) override;
	
	void SelectNextPlanet();
	void SelectPreviousPlanet();
	void SelectRocket();
	void LaunchRocket();
	
	// Quantum physics constants
	UPROPERTY(EditAnywhere, Category = "Quantum Physics")
	double CasimirCoefficient = (M_PI * M_PI * 1.0545718e-34 * 3e8) / 240.0; // ħcπ²/240
	
	// In PracaInzGameState.h
	UPROPERTY(EditAnywhere, Category = "Quantum Physics")
	double NeutrinoCoupling = 1e-10;      // Increased for stronger attraction
	
	UPROPERTY(EditAnywhere, Category = "Quantum Physics")
	double AntineutrinoCoupling = 1e-10;  // Same as neutrino for symmetry

	UPROPERTY(EditAnywhere, Category = "Simulation")
	double UnitConversion = 0.00001; // 1uu (cm) = 1km
	
	UPROPERTY(EditAnywhere, Category = "Simulation")
	double BaseDistance = 1000.0 / 149600000.0; // Correct scaling: 1 km = 6.68e-6 game units (1 AU = 1000 units)
	
	int64 SecondsInSimulation = 86400;
	double CurrentDeltaTime = 1.0/60.0; // Instead of 1/60

	
	TArray<APlanet*> Planets;
	TArray<AAstralObject*> Objects;
	ACameraPawn* Camera;
	APlanet* CurrentPlanet;
	ARocket* Rocket;

	//VR
	APlanet* CurrentPlanetVR;
	FVector CurrentPlanetVRScale;

private:
	void ParseJsonData(const FString& JsonData);
	void FetchPlanetData();
	void OnHttpResponseReceived(FHttpRequestPtr Request, FHttpResponsePtr Response, bool bWasSuccessful);
	void ProcessXmlData(const FString& XmlData);
	void SpawnPlanetFromJsonData(const FPlanetData& PlanetData);
	void SpawnPlanetFromXmlData(const FString& Name, double Mass, double Radius, double Inclination, double Distance);
	
	TMap<AAstralObject*, FVector> AstralForces;
};
