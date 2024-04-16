#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "Math/UnrealMath.h"
#include "Components/WidgetComponent.h"
#include "AstralObject.generated.h"

UCLASS()
class PRACAINZ_API AstralObject : public AActor
{
	GENERATED_BODY()

	public:
	AstralObject();
		
	UPROPERTY(EditAnywhere, Category = "Components")
	double Mass;

	FVector PrecomputedForce;

	void UpdatePrecomputedForce(FVector TotalForce);
};
