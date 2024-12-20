// Fill out your copyright notice in the Description page of Project Settings.


#include "PracaInzGameState.h"

#include "GameFramework/PlayerController.h"

#include "Kismet/GameplayStatics.h"

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
//	FName levelName = FName(*GetWorld()->GetName());
//	if (levelName == FName("SolarSystem"))
//	{
////		BaseDistance = (1.0 / 1000.0) * 149597870700;
//	}
//	else
//	{
//		BaseDistance = 1 / (1E3);
//	}
	
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


void APracaInzGameState::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
	
	// Update the total elapsed time since the start of the simulation
	TimeSinceStart += DeltaTime;
	
	// Calculate gravitational forces between all pairs of planets using ParallelFor
	ParallelFor(Objects.Num(), [this, DeltaTime](int32 index) {
		AAstralObject* astralObject = Objects[index];
		FVector TotalForce = FVector(0, 0, 0);
		
		// Calculate gravitational forces between celestial objects
		for (int32 j = index + 1; j < Objects.Num(); ++j) {
			AAstralObject* otherObject = Objects[j];
			FVector r = otherObject->GetActorLocation() - astralObject->GetActorLocation();
			double distanceSquared = r.SizeSquared();
			
			if (distanceSquared < KINDA_SMALL_NUMBER) continue; // Skip to avoid division by zero
			
			FVector F = G * (astralObject->PlanetMass * otherObject->PlanetMass) / distanceSquared * r.GetSafeNormal();
			
			TotalForce += F;
			
			// Symmetry: Add force to the other object
			AstralForces[otherObject] -= F;
		}
		
		// Calculate gravitational forces from Lagrange points dynamically
		for (int32 j = index + 1; j < Objects.Num(); ++j) {
			AAstralObject* otherObject = Objects[j];
			FVector r = otherObject->GetActorLocation() - astralObject->GetActorLocation();
			double distanceSquared = r.SizeSquared();
			
			if (distanceSquared < KINDA_SMALL_NUMBER) continue; // Skip to avoid division by zero
			
			// Calculate Lagrange point mass based on the other object's mass
			float lagrangePointMass = otherObject->PlanetMass; // No scaling needed
			
			// Calculate gravitational force from Lagrange point
			FVector F = G * (astralObject->PlanetMass * lagrangePointMass) / distanceSquared * r.GetSafeNormal();
			
			TotalForce += F;
			
			// Symmetry: Add force to the other object
			AstralForces[otherObject] -= F;
		}
		
		AstralForces[astralObject] = TotalForce;
	}, false);
	
	// Update forces and simulate objects
	ParallelFor(Objects.Num(), [this, DeltaTime](int32 index) {
		AAstralObject* astralObject = Objects[index];
		astralObject->UpdatePrecomputedForce(AstralForces[astralObject]);
	}, false);
	
	for (AAstralObject* astralObject : Objects)
	{
		astralObject->Tick(DeltaTime);
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
			
			double Distance = FCString::Atof(*DistanceStr);
			
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
					
					double StarRadius = FCString::Atof(*StarRadiusStr);

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


void APracaInzGameState::SpawnPlanetFromXmlData(const FString& Name, double PlanetMass, double Radius, double Inclination, double Distance)
{
	// Convert radius to diameter
	double Diameter = Radius * 2.0f;
	
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
		NewPlanet->PlanetMass = PlanetMass * 332886.8125; // Sun PlanetMass
		NewPlanet->Diameter = Diameter;
		NewPlanet->Name = Name;
		NewPlanet->Inclination = Inclination;
		NewPlanet->SetActorScale3D(FVector(1));
		NewPlanet->OrbitColor = FColor(255, 255, 0, 255); // Setting a default color, adjust as needed

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
		NewPlanet->InitialVelocity = FVector(0, 13, 0);
		NewPlanet->InitialSetup();
		NewPlanet->SetActorScale3D(FVector(0.1)); // Assuming a uniform scale
		NewPlanet->OrbitColor = FColor(0, 255, 0, 255); // Setting a default color, adjust as needed
	}
}
