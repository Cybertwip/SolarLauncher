// Fill out your copyright notice in the Description page of Project Settings.


#include "CameraPawn.h"
#include "GameFramework/SpringArmComponent.h"
#include "Classes/Components/StaticMeshComponent.h"
#include "Camera/CameraComponent.h"
#include "PracaInzGameModeBase.h"
#include "HeadMountedDisplayFunctionLibrary.h"
#include "PracaInzGameState.h"

#include "Planet.h"

// Sets default values
ACameraPawn::ACameraPawn()
{
 	// Set this pawn to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	CameraArm = CreateDefaultSubobject<USpringArmComponent>(TEXT("CameraArm"));
	Camera = CreateDefaultSubobject<UCameraComponent>(TEXT("Camera"));	Focused = CreateDefaultSubobject<USceneComponent>("RootComponent");	RootComponent = Focused;	RootComponent->SetAbsolute(true, true, true);	CameraArm->SetupAttachment(RootComponent);	CameraArm->TargetArmLength = 600.0f;	CameraArm->SetWorldRotation(FRotator(-45.f, 0.f, 0.f));	CameraArm->bDoCollisionTest = false;	CameraArm->AttachTo(RootComponent);
	Camera->SetupAttachment(CameraArm);
	Camera->AttachTo(CameraArm, USpringArmComponent::SocketName); 	CameraArm->bAbsoluteRotation = true;
   	
	AutoPossessPlayer = EAutoReceiveInput::Player0;
}

// Called when the game starts or when spawned
void ACameraPawn::BeginPlay()
{
	Super::BeginPlay();	
	if (APracaInzGameState* PracaInzGameState = Cast<APracaInzGameState>(GetWorld()->GetGameState()))
	{
		PracaInzGameState->Camera = this;
		Focused = Cast<USceneComponent>(PracaInzGameState->Planets[1]->PlanetMesh);
		RootComponent = Focused;		CameraArm->SetWorldRotation(FRotator(-45.f, 0.f, 0.f));		CameraArm->AttachTo(RootComponent);
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
		RootComponent = Focused;		CameraArm->SetWorldRotation(CameraArm->GetComponentRotation());		CameraArm->AttachTo(RootComponent);
		//CameraArm->TargetArmLength = 600.0f;
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

}

void ACameraPawn::ZoomIn()
{
	if(CameraArm->TargetArmLength>100)
		CameraArm->TargetArmLength -= CameraArm->TargetArmLength / 10;
}

void ACameraPawn::ZoomOut()
{
	if(CameraArm->TargetArmLength<2000)
		CameraArm->TargetArmLength += CameraArm->TargetArmLength / 10;
}

//pitch
void ACameraPawn::RotateY(float Value)
{
	FRotator NewPitch = CameraArm->GetComponentRotation();
	///89.0f value is here to prevent camera roll
	NewPitch.Pitch = FMath::Clamp(NewPitch.Pitch + Value, -89.0f, 89.0f);
	CameraArm->SetWorldRotation(NewPitch);
}


//yaw 
void ACameraPawn::RotateX(float Value)
{
	FRotator NewYaw = CameraArm->GetComponentRotation();
	NewYaw.Yaw += Value;
	CameraArm->SetWorldRotation(NewYaw);    
}

