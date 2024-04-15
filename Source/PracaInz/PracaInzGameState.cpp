// Fill out your copyright notice in the Description page of Project Settings.


#include "PracaInzGameState.h"

#include "GameFramework/PlayerController.h"

#include "Kismet/GameplayStatics.h"

namespace {
FVector CalculateInitialPositionParsecs(float Distance)
{
	// Placeholder for a more complex calculation
	// Example: Convert parsec distance to game world units, assuming 1 parsec = 1000 units
	float GameWorldDistance = Distance * 206265; // Scaled parsecs
	return FVector(2770 + GameWorldDistance, -14390, 0.0);
}

FVector CalculateInitialPositionAU(float Distance) {
	// Example: Convert parsec distance to game world units, assuming 1 AU = 1000 units
	float GameWorldDistance = Distance * 1000; // Scaled AUs
	
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
	FName levelName = FName(*GetWorld()->GetName());
	if (levelName == FName("SolarSystem"))
	{
		BaseDistance = (1.0 / 1000.0) * 149597870700;
	}
	else
	{
		BaseDistance = 1 / (1E3);
	}
	
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
	
	for (APlanet* planet : Planets)
	{
		if(planet->Name == "Sun"){
			star = planet;
		}
		
		if(planet->Name == "Earth"){
			earth = planet;
		}
		
		if(star && earth){
			break;
		}
	}
}

void APracaInzGameState::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
	
	// Update the total elapsed time since the start of the simulation
	TimeSinceStart += DeltaTime;
	
	TMap<APlanet*, FVector> PlanetForces;
	for (APlanet* planet : Planets)
	{
		PlanetForces.Add(planet, FVector(0, 0, 0));
	}
	
	// Calculate gravitational forces between all pairs of planets
	for (APlanet* planet : Planets)
	{
		for (APlanet* otherPlanet : Planets)
		{
			if (planet == otherPlanet) continue;

			FVector r = otherPlanet->GetActorLocation() - planet->GetActorLocation();
			
			float distanceSquared = r.SizeSquared();
			
			if (distanceSquared < KINDA_SMALL_NUMBER) // Check to avoid division by zero
			{
				continue; // Skip this iteration to avoid infinite forces
			}
			
			FVector F = (planet->PlanetMass * otherPlanet->PlanetMass) / distanceSquared * r.GetSafeNormal();
			
			F *= G * DeltaTime * DeltaTime;
			
			PlanetForces[planet] += F;
		}
	}
	
	for (APlanet* planet : Planets)
	{
		planet->UpdatePlanetPosition(DeltaTime, PlanetForces[planet]);
	}

	
	if (earth == nullptr || star == nullptr)
	{
		return;
	}
	
//	const double G = 6.67430e-11 * 1.766;

	auto starLocation = star->GetActorLocation();
	auto planetLocation = earth->GetActorLocation();
	// Calculate the distance between the objects
	FVector relativePosition = planetLocation - starLocation;
	const double relativeDistance = relativePosition.Size();
	// Calculate the elevation angle between spins to get the elliptical
	const float inclinationAngle = FMath::RadiansToDegrees(FMath::Acos(relativePosition.Z / relativeDistance));
	
	// Apply Kepler's third law assuming circular orbit
	// Convert inclination angle to radians
	const float inclinationRadians = FMath::DegreesToRadians(earth->Inclination);
	
	// Adjust orbital speed for elliptical orbit
	const double orbitalSpeedElliptical = earth->Velocity.Size() * FMath::Cos(inclinationRadians);
	
	// Calculate semi-major axis (assuming a circular orbit for simplicity)
	const double timeScaled = TimeSinceStart * TimeSinceStart * SecondsInSimulation;
	
	const double semiMajorAxis = (G * star->PlanetMass * timeScaled) / (4 * PI * PI);
	
	// Calculate semi-minor axis for elliptical orbit
	const double semiMinorAxis = semiMajorAxis * FMath::Sin(inclinationRadians);
	
	// Calculate the orbital period for elliptical orbit using Kepler's third law
	const double orbitalPeriodSecondsElliptical = 2 * PI * FMath::Sqrt(FMath::Pow(semiMajorAxis, 3) / (G * (star->PlanetMass + earth->PlanetMass)));
	
	// Convert orbital period to days for elliptical orbit
	earth->OrbitalPeriodDays = orbitalPeriodSecondsElliptical / (24.0 * 60.0 * 60.0);
	
	if (earth->OrbitalPeriodDays > 365) {
		earth->OrbitalPeriodDays = fmod(earth->OrbitalPeriodDays, 365.0); // Reset the day count when it exceeds 365
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
			// Calculate Diameter and Mass based on H
			const float TypicalAlbedo = 0.15;
			double Diameter = std::pow(10, (3.123 - H / 5));

			PlanetData.Radius = Diameter / 2.0;  // Radius in kilometers, convert to Earth radii later
						
			// Assuming density for mass calculation
			double Density = 2500;  // kg/m³, assuming a rocky composition
			double Volume = (4.0 / 3.0) * PI * FMath::Pow(PlanetData.Radius * 1000, 3); // Convert radius from km to meters
			PlanetData.Mass = (Volume * Density) / 5.9722e24;
			
			PlanetData.Radius /= 6371.0;

		}
		else
		{
			// Optionally handle planets without 'H' differently or skip
			continue;
		}
		
		// Check capacity to avoid exceeding limits
		if (Planets.Num() >= 800)
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
			
			float Distance = FCString::Atof(*DistanceStr);
			
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
					if (const FXmlNode* MassNode = StarNode->FindChildNode("mass"))
					{
						StarMassStr = MassNode->GetContent();
					}
					else
					{
						continue; // Skip to the next star
					}
					
					float StarMass = FCString::Atof(*StarMassStr);
					
					FString StarRadiusStr;
					if (const FXmlNode* RadiusNode = StarNode->FindChildNode("radius"))
					{
						StarRadiusStr = RadiusNode->GetContent();
					}
					else
					{
						continue; // Skip to the next star
					}
					
					float StarRadius = FCString::Atof(*StarRadiusStr);
					
					if (Planets.Num() >= 1600)
					{
						break;  // Stop processing if too many planets
					}

					SpawnPlanetFromXmlData(StarName, StarMass, StarRadius, 0.0f, Distance);
				}
			}
		}
	}
	else
	{
		UE_LOG(LogTemp, Error, TEXT("Failed to parse XML data."));
	}
}


void APracaInzGameState::SpawnPlanetFromXmlData(const FString& Name, float Mass, float Radius, float Inclination, float Distance)
{
	// Convert radius to diameter
	float Diameter = Radius * 2.0f;
	
	// Configuration for the new planet actor
	FActorSpawnParameters SpawnParams;
	SpawnParams.Owner = this;
	SpawnParams.Instigator = GetInstigator();
	
	// Calculate initial position and other dynamics as needed
	FVector InitialPosition = CalculateInitialPositionParsecs(Distance); // Modify this method to use distance
	
	// Spawn the planet actor
	APlanet* NewPlanet = GetWorld()->SpawnActor<APlanet>(APlanet::StaticClass(), InitialPosition, FRotator::ZeroRotator, SpawnParams);
	if (NewPlanet)
	{
		NewPlanet->PlanetMass = Mass * 332886.8125; // Sun mass
		NewPlanet->Diameter = Diameter;
		NewPlanet->Name = Name;
		NewPlanet->Inclination = Inclination;
		NewPlanet->SetActorScale3D(FVector(1));
		NewPlanet->OrbitColor = FColor(255, 255, 0, 255); // Setting a default color, adjust as needed

	}
}

void APracaInzGameState::SpawnPlanetFromJsonData(const FPlanetData& PlanetData)
{
	float Diameter = PlanetData.Radius * 2.0f; // Convert radius to diameter
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
		NewPlanet->SetActorScale3D(FVector(0.1)); // Assuming a uniform scale
		NewPlanet->Inclination = 0.0039;
		NewPlanet->OrbitColor = FColor(255, 255, 255, 255); // Setting a default color, adjust as needed
	}
}
