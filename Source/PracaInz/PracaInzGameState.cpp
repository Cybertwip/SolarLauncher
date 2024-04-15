// Fill out your copyright notice in the Description page of Project Settings.


#include "PracaInzGameState.h"

#include "GameFramework/PlayerController.h"
#include "Kismet/GameplayStatics.h"

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
	
	FetchPlanetData();
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
		const FXmlNode* rootNode = XmlFile.GetRootNode(); // Assuming rootNode is "systems"
		for (const FXmlNode* systemNode : rootNode->GetChildrenNodes()) // Iterate over each "system"
		{
			FString SystemDistanceStr = systemNode->FindChildNode("distance") ? systemNode->FindChildNode("distance")->GetAttribute("value") : TEXT("");
			float SystemDistance = FCString::Atof(*SystemDistanceStr);  // Convert to float using FCString::Atof
			
			if (SystemDistance <= 0) // Check for valid, positive distance
			{
				continue; // Skip systems with invalid or missing distance
			}
			UE_LOG(LogTemp, Log, TEXT("System Distance: %f parsecs"), SystemDistance);
			
			// Iterate over each "star" and "planet" similarly, ensuring you use valid distance
			// The following is a conceptual example for iterating stars
			for (const FXmlNode* starNode : systemNode->GetChildrenNodes())
			{
				if (starNode->GetTag() == "star")
				{
					FString StarName = starNode->GetAttribute("name");
					float StarMass = FCString::Atof(*(starNode->FindChildNode("mass") ? starNode->FindChildNode("mass")->GetAttribute("value") : TEXT("0")));
					float StarRadius = FCString::Atof(*(starNode->FindChildNode("radius") ? starNode->FindChildNode("radius")->GetAttribute("value") : TEXT("0")));
					
					if (StarMass > 0 && StarRadius > 0)
					{
						UE_LOG(LogTemp, Log, TEXT("Star: %s, Mass: %f, Radius: %f"),
							   *StarName, StarMass, StarRadius);
						
						SpawnPlanetFromXmlData(StarName, StarMass, StarRadius, 0.0f, SystemDistance);
					}
				}
			}
		}
	}
	else
	{
		UE_LOG(LogTemp, Error, TEXT("Failed to parse XML"));
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
		NewPlanet->PlanetMass = Mass;
		NewPlanet->Diameter = Diameter;
		NewPlanet->Name = Name;
		NewPlanet->Inclination = Inclination;
		// Set other properties as needed
	}
}
