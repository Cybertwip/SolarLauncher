#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "Math/UnrealMath.h"
#include "Components/WidgetComponent.h"
#include "AstralObject.generated.h"

UCLASS()
class PRACAINZ_API AAstralObject : public AActor
{
	GENERATED_BODY()

	public:
	AAstralObject();
		
	UPROPERTY(EditAnywhere, Category = "Components")
	double Mass;

	FVector PrecomputedForce;
	
	virtual void Tick(float DeltaTime) override {}

	void UpdatePrecomputedForce(FVector TotalForce);
};
