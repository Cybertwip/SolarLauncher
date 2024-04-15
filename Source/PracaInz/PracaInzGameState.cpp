// Fill out your copyright notice in the Description page of Project Settings.


#include "PracaInzGameState.h"

#include "GameFramework/PlayerController.h"
#include "Kismet/GameplayStatics.h"

namespace {
FVector CalculateInitialPosition(float Distance)
{
	// Placeholder for a more complex calculation
	// Example: Convert parsec distance to game world units, assuming 1 parsec = 1000 units
	float GameWorldDistance = Distance * 47194.78; // Scaled parsecs
	return FVector(GameWorldDistance, 0.0f, 0.0f);
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
		ProcessXmlData(XmlData);
	}
	else
	{
		UE_LOG(LogTemp, Error, TEXT("Failed to read XML file at: %s"), *FileName);
	}
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
	FVector InitialPosition = CalculateInitialPosition(Distance); // Modify this method to use distance
	
	// Spawn the planet actor
	APlanet* NewPlanet = GetWorld()->SpawnActor<APlanet>(APlanet::StaticClass(), InitialPosition, FRotator::ZeroRotator, SpawnParams);
	if (NewPlanet)
	{
		NewPlanet->PlanetMass = Mass * 332886.8125; // Sun mass
		NewPlanet->Diameter = Diameter;
		NewPlanet->Name = Name;
		NewPlanet->Inclination = Inclination;
		NewPlanet->SetActorScale3D(FVector(1));
		// Set other properties as needed
	}
}
