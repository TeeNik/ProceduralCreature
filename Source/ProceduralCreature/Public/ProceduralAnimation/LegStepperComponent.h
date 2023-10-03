#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "LegStepperComponent.generated.h"


USTRUCT()
struct FTargetInfo
{
	GENERATED_BODY()

	FVector TargetLocation;
	FVector TargetNormal;
};

UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class PROCEDURALCREATURE_API ULegStepperComponent : public USceneComponent
{
	GENERATED_BODY()

public:	
	ULegStepperComponent();

	FTargetInfo GetTargetInfo() const;

	bool UpdateTarget(bool UseOwnerLocation, float MinAngle, float MaxAngle);
	bool GetIsFarFromPoint() const;
	void SetIsLegColliding(bool Value);

protected:
	virtual void BeginPlay() override;
	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;

	float WantStepAtDistance = 250.0f;
	float StepOvershootFraction = 0.5f;
	float StepAtDistanceCollisionFactor = 1.0f;
	float MinDistFromHitToOrigin = 150.0f;
	float MinRadius = 225.0f;
	float MaxRadius = 240.0f;

private:
	FTargetInfo TargetInfo;
	bool IsFarFromPoint = false;
	bool IsLegColliding = false;
};
