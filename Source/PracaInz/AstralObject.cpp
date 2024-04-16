#include "AstralObject.h"
// Sets default values
AAstralObject::AAstralObject()
{
	// Set this actor to call Tick() every frame
	PrimaryActorTick.bCanEverTick = false;
}

void AAstralObject::UpdatePrecomputedForce(FVector TotalForce)
{
	PrecomputedForce = TotalForce;
}
