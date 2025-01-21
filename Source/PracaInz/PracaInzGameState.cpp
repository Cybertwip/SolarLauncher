// Fill out your copyright notice in the Description page of Project Settings.


#include "PracaInzGameState.h"

#include "GameFramework/PlayerController.h"

#include "Kismet/GameplayStatics.h"

// Casimir force constants
constexpr double PLANCK_CONSTANT = 1.0545718e-34;   // ħ (J·s)
constexpr double SPEED_OF_LIGHT = 3e8;              // c (m/s)
constexpr double CASIMIR_COEFFICIENT = (M_PI * M_PI * PLANCK_CONSTANT * SPEED_OF_LIGHT) / 240.0;


namespace {
FVector CalculateInitialPositionParsecs(double Distance)
{
	// Placeholder for a more complex calculation
	// Example: Convert parsec distance to game world units, assuming 1 parsec = 1000 units
	double GameWorldDistance = Distance * 206265; // Scaled parsecs
	return FVector(2770 + GameWorldDistance, -14390, 0.0);
}

FVector CalculateInitialPositionAU(double Distance) {
	// Example: Convert parsec distance to game world units, assuming 1 AU = 1000 units
	double GameWorldDistance = Distance * 1000; // Scaled AUs
	
	// Return the FVector with randomized position
	return FVector(2770 + GameWorldDistance, -14390, 0.0);
}
}

APracaInzGameState::APracaInzGameState()
{
	PrimaryActorTick.bCanEverTick = true;
}

void APracaInzGameState::BeginPlay()
{	
	FString FileName = FPaths::ProjectContentDir() + TEXT("Data/nea_extended.json");
	FString JsonData;
	if (FFileHelper::LoadFileToString(JsonData, *FileName))
	{
		ParseJsonData(JsonData);
	}
	else
	{
		UE_LOG(LogTemp, Error, TEXT("Failed to read Json file at: %s"), *FileName);
	}
	FileName = FPaths::ProjectContentDir() + TEXT("Data/systems.xml");
	FString XmlData;
	if (FFileHelper::LoadFileToString(XmlData, *FileName))
	{
		ProcessXmlData(XmlData);
	}
	else
	{
		UE_LOG(LogTemp, Error, TEXT("Failed to read XML file at: %s"), *FileName);
	}
	
	Planets.Sort([](APlanet& A, APlanet& B) {
		return A.PlanetMass < B.PlanetMass; // Ascending order
	});
	
	
	Objects.Add(Rocket);
	
	AstralForces.Add(Rocket, FVector(0, 0, 0));

	for (APlanet* planet : Planets)
	{
		AstralForces.Add(planet, FVector(0, 0, 0));
		
		Objects.Add(planet);
		
		if(planet->Name == "Mars"){
			Rocket->Target = planet;
		}
	}
}

// PracaInzGameState.cpp
void APracaInzGameState::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
	
	const int32 NumObjects = Objects.Num();
	if(NumObjects == 0) return;
	
	// Quantum constants (SI units)
	constexpr double HydrogenDensity = 1408.0; // kg/m³
	
	// Reset quantum force fields
	for(auto& Pair : AstralForces) {
		Pair.Value = FVector::ZeroVector;
	}
	
	// Calculate quantum entanglement forces
	ParallelFor(NumObjects, [&](int32 i) {
		AAstralObject* Object1 = Objects[i];
		const FVector Position1 = Object1->GetActorLocation();
		const double Mass1 = Object1->PlanetMass;
		
		// Compute radius using cube root approximation
		const double Radius1 = FMath::Pow(
										  3.0 * Mass1 / (4.0 * PI * HydrogenDensity + 1e-20),
										  1.0/3.0
										  );
		
		for(int32 j = i+1; j < NumObjects; j++) {
			AAstralObject* Object2 = Objects[j];
			const FVector Position2 = Object2->GetActorLocation();
			const double Mass2 = Object2->PlanetMass;
			
			FVector Delta = Position2 - Position1;
			double Distance = Delta.Size() * UnitConversion;
			if(Distance < 1e-10) continue;
			
			FVector Direction = Delta.GetSafeNormal();
			
			// 1. Neutrino-Antineutrino Attraction (Always active)
			const double NuAttraction = (Mass1 * NeutrinoCoupling) *
			(Mass2 * AntineutrinoCoupling);
			const double F_attract = NuAttraction / FMath::Square(Distance);
			
			// 2. Casimir Repulsion (Always active)
			const double Radius2 = FMath::Pow(
											  3.0 * Mass2 / (4.0 * PI * HydrogenDensity + 1e-20),
											  1.0/3.0
											  );
			const double EffectiveArea = PI * Radius1 * Radius2;
			const double F_repel = (CasimirCoefficient * EffectiveArea) /
			FMath::Pow(Distance + 1e-5, 4); // +epsilon for stability
			
			// Net quantum force
			const double TotalForce = (F_attract - F_repel);
			
			// Apply opposing quantum forces
			AstralForces[Object1] += TotalForce * Direction;
			AstralForces[Object2] -= TotalForce * Direction;
		}
	}, NumObjects > 1000);
	
	// Update orbital dynamics
	for (int32 i = 0; i < NumObjects; ++i) {
		AAstralObject* astralObject = Objects[i];
		if(AstralForces.Contains(astralObject)) {
			astralObject->UpdatePrecomputedForce(AstralForces[astralObject]);
			astralObject->Tick(DeltaTime);
		}
	}
}

void APracaInzGameState::SpawnPlanetFromXmlData(const FString& Name, double Mass, double RadiusKm, double Inclination, double DistanceAU)
{
	FActorSpawnParameters SpawnParams;
	SpawnParams.Owner = this;
	SpawnParams.Instigator = GetInstigator();
	
	// Convert AU to game units (1 AU = 1000 units)
	FVector InitialPosition = FVector(DistanceAU * 1000.0, 0, 0);
	
	// Convert radius from km to game units
	double RadiusGameUnits = RadiusKm * BaseDistance;
	double DiameterGameUnits = RadiusGameUnits * 2.0;
	
	APlanet* NewPlanet = GetWorld()->SpawnActor<APlanet>(APlanet::StaticClass(), InitialPosition, FRotator::ZeroRotator, SpawnParams);
	if (NewPlanet)
	{
		NewPlanet->PlanetMass = Mass * 1.9885e30; // Convert solar masses to kg
		NewPlanet->Diameter = DiameterGameUnits;
		NewPlanet->Name = Name;
	}
}


void APracaInzGameState::SelectNextPlanet()
{
	if (Planets.Num() == 0)
		return;
	
	int32 NextIndex = (Planets.IndexOfByKey(CurrentPlanet) + 1) % Planets.Num();
	CurrentPlanet = Planets[NextIndex];
	
	CurrentPlanet->OnSelected(nullptr, FKey("E"));
}

void APracaInzGameState::SelectPreviousPlanet()
{
	if (Planets.Num() == 0)
		return;
	
	int32 PreviousIndex = (Planets.IndexOfByKey(CurrentPlanet) - 1 + Planets.Num()) % Planets.Num();
	CurrentPlanet = Planets[PreviousIndex];
	
	CurrentPlanet->OnSelected(nullptr, FKey("Q"));
	
}


void APracaInzGameState::SelectRocket()
{
	Rocket->OnSelected(nullptr, FKey("Q"));
}

void APracaInzGameState::LaunchRocket(){
	Rocket->Launch();
}

void APracaInzGameState::FetchPlanetData()
{
	TSharedRef<IHttpRequest, ESPMode::ThreadSafe> HttpRequest = FHttpModule::Get().CreateRequest();
	HttpRequest->OnProcessRequestComplete().BindUObject(this, &APracaInzGameState::OnHttpResponseReceived);
	HttpRequest->SetURL(TEXT("https://github.com/OpenExoplanetCatalogue/oec_gzip/raw/master/systems.xml.gz"));
	HttpRequest->SetVerb(TEXT("GET"));
	HttpRequest->ProcessRequest();
}

void APracaInzGameState::OnHttpResponseReceived(FHttpRequestPtr Request, FHttpResponsePtr Response, bool bWasSuccessful)
{
	if (bWasSuccessful && Response.IsValid() && Response->GetContentLength() > 0)
	{
		FString XmlData = FString(UTF8_TO_TCHAR(Response->GetContent().GetData()));
		
		// Here you should add the code to decompress if dealing with GZIP
		ProcessXmlData(Response->GetContentAsString());
	}
	else
	{
		UE_LOG(LogTemp, Error, TEXT("Failed to download or invalid response"));
	}
}
void APracaInzGameState::ParseJsonData(const FString& JsonData)
{
	TArray<TSharedPtr<FJsonValue>> JsonArray;
	TSharedRef<TJsonReader<>> Reader = TJsonReaderFactory<>::Create(JsonData);
	
	if (!FJsonSerializer::Deserialize(Reader, JsonArray))
	{
		UE_LOG(LogTemp, Error, TEXT("Failed to parse JSON data"));
		return;
	}
	
	for (const TSharedPtr<FJsonValue>& Value : JsonArray)
	{
		TSharedPtr<FJsonObject> PlanetObject = Value->AsObject();
		if (!PlanetObject.IsValid())
		{
			continue;
		}
		
		FPlanetData PlanetData;
		FString Name;
		if (!PlanetObject->TryGetStringField("Name", Name))
		{
			Name = TEXT("Unknown");  // Use default name if none is provided
		}
		PlanetData.Name = Name;
		
		double PerihelionDist;
		if (!PlanetObject->TryGetNumberField("Perihelion_dist", PerihelionDist))
		{
			continue;  // Skip this planet if essential data is missing
		}
		PlanetData.Distance = PerihelionDist;
		
		double H;
		if (PlanetObject->TryGetNumberField("H", H))
		{
			// Calculate Diameter and PlanetMass based on H
			const double TypicalAlbedo = 0.15;
			double Diameter = std::pow(10, (3.123 - H / 5));

			PlanetData.Radius = Diameter / 2.0;  // Radius in kilometers, convert to Earth radii later
						
			// Assuming density for PlanetMass calculation
			double Density = 2500;  // kg/m³, assuming a rocky composition
			
			PlanetData.Radius *= BaseDistance;

			double Volume = (4.0 / 3.0) * PI * FMath::Pow(PlanetData.Radius * 1000, 3); // Convert radius from km to meters
			PlanetData.Mass = (Volume * Density);
			

		}
		else
		{
			// Optionally handle planets without 'H' differently or skip
			continue;
		}
		
		// Check capacity to avoid exceeding limits
		if (Planets.Num() >= 1250)
		{
			break;  // Stop processing if too many planets
		}
		
		// Spawn or store the planet data
		SpawnPlanetFromJsonData(PlanetData);
	}
}



void APracaInzGameState::ProcessXmlData(const FString& XmlData)
{
	FXmlFile XmlFile(XmlData, EConstructMethod::ConstructFromBuffer);
	if (XmlFile.IsValid())
	{
		const FXmlNode* RootNode = XmlFile.GetRootNode(); // "systems" node
		for (const FXmlNode* SystemNode : RootNode->GetChildrenNodes()) // Each "system"
		{
			FString SystemName;
			if (const FXmlNode* NameNode = SystemNode->FindChildNode("name"))
			{
				SystemName = NameNode->GetContent();
			}
			else
			{
				continue; // Skip to the next system
			}
			
			FString DistanceStr;
			if (const FXmlNode* DistanceNode = SystemNode->FindChildNode("distance"))
			{
				DistanceStr = DistanceNode->GetContent();
			}
			else
			{
				continue; // Skip to the next system
			}
			
			double DistanceParsecs = FCString::Atof(*DistanceStr);
			double DistanceAU = DistanceParsecs * 206265.0; // Convert parsecs to AU

			
			for (const FXmlNode* StarNode : SystemNode->GetChildrenNodes())
			{
				if (StarNode->GetTag() == "star")
				{
					FString StarName;
					if (const FXmlNode* NameNode = StarNode->FindChildNode("name"))
					{
						StarName = NameNode->GetContent();
					}
					else
					{
						continue; // Skip to the next star
					}
					
					FString StarMassStr;
					if (const FXmlNode* MassNode = StarNode->FindChildNode("PlanetMass"))
					{
						StarMassStr = MassNode->GetContent();
					}
					else
					{
						continue; // Skip to the next star
					}
					
					double StarMass = FCString::Atof(*StarMassStr);
					
					FString StarRadiusStr;
					if (const FXmlNode* RadiusNode = StarNode->FindChildNode("radius"))
					{
						StarRadiusStr = RadiusNode->GetContent();
					}
					else
					{
						continue; // Skip to the next star
					}
					
					double StarRadiusKm = FCString::Atof(*StarRadiusStr) * 695700.0; // Solar radii to km
					
					SpawnPlanetFromXmlData(StarName, StarMass, StarRadiusKm, 0.0f, DistanceAU);
				}
			}
		}
	}
	else
	{
		UE_LOG(LogTemp, Error, TEXT("Failed to parse XML data."));
	}
}



void APracaInzGameState::SpawnPlanetFromJsonData(const FPlanetData& PlanetData)
{
	double Diameter = PlanetData.Radius * 2.0f; // Convert radius to diameter
	FVector InitialPosition = CalculateInitialPositionAU(PlanetData.Distance); // Calculate initial position based on distance
	
	FActorSpawnParameters SpawnParams;
	SpawnParams.Owner = this;
	SpawnParams.Instigator = GetInstigator();
	
	APlanet* NewPlanet = GetWorld()->SpawnActor<APlanet>(APlanet::StaticClass(), InitialPosition, FRotator::ZeroRotator, SpawnParams);
	if (NewPlanet)
	{
		NewPlanet->PlanetMass = PlanetData.Mass;
		NewPlanet->Diameter = Diameter;
		NewPlanet->Name = PlanetData.Name;
		NewPlanet->InitialVelocity = FVector::ZeroVector;
		NewPlanet->InitialSetup();
		NewPlanet->SetActorScale3D(FVector(0.1)); // Assuming a uniform scale
		NewPlanet->OrbitColor = FColor(0, 255, 0, 255); // Setting a default color, adjust as needed
	}
}
