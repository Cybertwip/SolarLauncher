// Fill out your copyright notice in the Description page of Project Settings.


#include "CameraPawn.h"
#include "GameFramework/SpringArmComponent.h"
#include "Classes/Components/StaticMeshComponent.h"
#include "Camera/CameraComponent.h"
#include "PracaInzGameModeBase.h"
#include "PracaInzGameState.h"
#include "Planet.h"

// Sets default values
ACameraPawn::ACameraPawn()
{
	// Set this pawn to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;
	
	CameraArm = CreateDefaultSubobject<USpringArmComponent>(TEXT("CameraArm"));
	Camera = CreateDefaultSubobject<UCameraComponent>(TEXT("Camera"));
	Focused = CreateDefaultSubobject<USceneComponent>("RootComponent");
	RootComponent = Focused;
	RootComponent->SetAbsolute(true, true, true);
	
	CameraArm->SetupAttachment(RootComponent);
	CameraArm->TargetArmLength = 1000.0f;
	CameraArm->SetWorldRotation(FRotator(-45.f, 0.f, 0.f));
	CameraArm->bDoCollisionTest = false;
	//	CameraArm->AttachToComponent(RootComponent, FAttachmentTransformRules::KeepRelativeTransform);
	Camera->SetupAttachment(CameraArm);
	CameraArm->SetUsingAbsoluteRotation(true);
	
	
	AutoPossessPlayer = EAutoReceiveInput::Player0;
}

// Called when the game starts or when spawned
void ACameraPawn::BeginPlay()
{
	Super::BeginPlay();
	if (APracaInzGameState* PracaInzGameState = Cast<APracaInzGameState>(GetWorld()->GetGameState()))
	{
		PracaInzGameState->Camera = this;
		Focused = Cast<USceneComponent>(PracaInzGameState->Planets[0]->PlanetMesh);
		PracaInzGameState->CurrentPlanet = PracaInzGameState->Planets[0];
		RootComponent = Focused;
		CameraArm->SetWorldRotation(FRotator(-45.f, 0.f, 0.f));
		CameraArm->AttachToComponent(RootComponent, FAttachmentTransformRules::KeepRelativeTransform);
		CameraArm->TargetArmLength = 600.0f;
		//CameraArm->TargetOffset = FVector(100.0f, 0.f, 100.0f);
	}
}

// Called every frame
void ACameraPawn::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
	if (bOnChangePlanet)
	{
		RootComponent = Focused;
		CameraArm->SetWorldRotation(CameraArm->GetComponentRotation());
		CameraArm->AttachToComponent(RootComponent, FAttachmentTransformRules::KeepRelativeTransform);
		bOnChangePlanet = false;
	}
}

// Called to bind functionality to input
void ACameraPawn::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);
	
	PlayerInputComponent->BindAction("ZoomIn", IE_Pressed, this, &ACameraPawn::ZoomIn);
	PlayerInputComponent->BindAction("ZoomOut", IE_Pressed, this, &ACameraPawn::ZoomOut);
	PlayerInputComponent->BindAxis("RotateX", this, &ACameraPawn::RotateX);
	PlayerInputComponent->BindAxis("RotateY", this, &ACameraPawn::RotateY);
	
	
	if (APracaInzGameState* PracaInzGameState = Cast<APracaInzGameState>(GetWorld()->GetGameState()))
		
	{
		PlayerInputComponent->BindAction("RTrigger", IE_Pressed, PracaInzGameState, &APracaInzGameState::SelectNextPlanet);
		PlayerInputComponent->BindAction("LTrigger", IE_Pressed, PracaInzGameState, &APracaInzGameState::SelectPreviousPlanet);

		PlayerInputComponent->BindAction("Rocket", IE_Pressed, PracaInzGameState, &APracaInzGameState::SelectRocket);

		PlayerInputComponent->BindAction("Launch", IE_Pressed, PracaInzGameState, &APracaInzGameState::LaunchRocket);
	}
	
}

void ACameraPawn::ZoomIn()
{
	
	if(Camera){
		Camera->FieldOfView -= Camera->FieldOfView / 10.0f;
	}
//	if(CameraArm->TargetArmLength>300){
//		CameraArm->TargetArmLength -= CameraArm->TargetArmLength / 10;
//	} else
//	{
//		if (Camera)
//		{
//			Camera->FieldOfView -= 1.0f; // Adjust FOV increment as needed
//		}
//		
//	}
}

void ACameraPawn::ZoomOut()
{
//	if(CameraArm->TargetArmLength<60000){
//		CameraArm->TargetArmLength += CameraArm->TargetArmLength / 10;
//	} else {
//		
//		// Decrease FOV for zooming out
//		if (Camera)
//		{
//			Camera->FieldOfView += 1.0f; // Adjust FOV decrement as needed
//		}
//	}
	
	if(Camera){
		Camera->FieldOfView += Camera->FieldOfView / 10.0f;
	}

}

void ACameraPawn::RotateY(float Value)
{
	FRotator NewPitch = CameraArm->GetComponentRotation();
	NewPitch.Pitch = FMath::Clamp(NewPitch.Pitch + Value, -89.0f, 89.0f);
	CameraArm->SetWorldRotation(NewPitch);
}

void ACameraPawn::RotateX(float Value)
{
	FRotator NewYaw = CameraArm->GetComponentRotation();
	NewYaw.Yaw += Value;
	CameraArm->SetWorldRotation(NewYaw);
}

