#include "AstralObject.h"
// Sets default values
AstralObject::AstralObject()
{
	// Set this actor to call Tick() every frame
	PrimaryActorTick.bCanEverTick = false;
}

void AstralObject::UpdatePrecomputedForce(FVector TotalForce)
{
	PrecomputedForce = TotalForce;
}
