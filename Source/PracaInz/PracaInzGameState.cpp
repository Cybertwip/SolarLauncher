// Fill out your copyright notice in the Description page of Project Settings.


#include "PracaInzGameState.h"

#include "GameFramework/PlayerController.h"

#include "Kismet/GameplayStatics.h"

namespace {
FVector CalculateInitialPositionParsecs(float Distance)
{
	// Placeholder for a more complex calculation
	// Example: Convert parsec distance to game world units, assuming 1 parsec = 1000 units
	float GameWorldDistance = Distance * 47194.78; // Scaled parsecs
	return FVector(GameWorldDistance + 2770, -14390, 0.0f);
}

FVector CalculateInitialPositionAU(float Distance)
{
	// Placeholder for a more complex calculation
	// Example: Convert parsec distance to game world units, assuming 1 AU = 1000 units
	float GameWorldDistance = Distance * 1000; // Scaled parsecs
	return FVector(GameWorldDistance + 2770, -14390, 0.0f);
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
		G = 1.179E-10;
		BaseDistance = (1.0 / 1000.0) * 149597870700;
	}
	else
	{
		G = 49.004E-7;
		BaseDistance = 1 / (1E3);
	}
	
	FString FileName = FPaths::ProjectContentDir() + TEXT("Data/systems.xml");
	FString XmlData;
	if (FFileHelper::LoadFileToString(XmlData, *FileName))
	{
		//ProcessXmlData(XmlData);
	}
	else
	{
		UE_LOG(LogTemp, Error, TEXT("Failed to read XML file at: %s"), *FileName);
	}
	
	FileName = FPaths::ProjectContentDir() + TEXT("Data/nea_extended.json");
	FString JsonData;
	if (FFileHelper::LoadFileToString(JsonData, *FileName))
	{
		ParseJsonData(JsonData);
	}
	else
	{
		UE_LOG(LogTemp, Error, TEXT("Failed to read Json file at: %s"), *FileName);
	}
	
	Planets.Sort([](APlanet& A, APlanet& B) {
		return A.PlanetMass < B.PlanetMass; // Ascending order
	});
}


void APracaInzGameState::Tick(float DeltaSeconds)
{
	Super::Tick(DeltaSeconds);
	
	// Update the total elapsed time since the start of the simulation
	TimeSinceStart += DeltaSeconds;
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
			double Diameter = 1329 / FMath::Sqrt(TypicalAlbedo) * FMath::Pow(10, -0.2 * H);
			PlanetData.Radius = Diameter / 2.0;  // Radius in kilometers, convert to Earth radii later
			
			// Assuming density for mass calculation
			double Density = 2500;  // kg/m³, assuming a rocky composition
			double Volume = (4.0 / 3.0) * PI * FMath::Pow(PlanetData.Radius * 1000, 3); // Convert radius from km to meters
			PlanetData.Mass = Volume * Density / 5.972e24; // Convert kg to Earth masses
			
			if(PlanetData.Mass == 0){
				continue;
			}
		}
		else
		{
			// Optionally handle planets without 'H' differently or skip
			continue;
		}
		
		// Check capacity to avoid exceeding limits
		if (Planets.Num() >= 1000)
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
					
					// Check capacity to avoid exceeding limits
					if (Planets.Num() >= 500)
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
		NewPlanet->SetActorScale3D(FVector(1)); // Assuming a uniform scale
		NewPlanet->Inclination = 0.0f;
		NewPlanet->OrbitColor = FColor(255, 255, 0, 255); // Setting a default color, adjust as needed
	}
}
