#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "LegComponent.generated.h"


class USplineLegComponent;
class ULegStepperComponent;

DECLARE_MULTICAST_DELEGATE_OneParam(FOnLegAnimationFinished, const struct FTargetInfo&);

UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class PROCEDURALCREATURE_API ULegComponent : public UActorComponent
{
	GENERATED_BODY()

public:	
	ULegComponent();
	void Init(float MinAngle, float MaxAngle);

	bool IsFarFromPoint() const;
	bool SwapLegs();
	bool IsMoving() const;

	FOnLegAnimationFinished OnLegAnimationFinished;

protected:
	virtual void BeginPlay() override;
	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;

	int32 NumOfBunches = 2;
	int32 NumOfLegsInBunch = 3;

	UPROPERTY()
	TArray<USplineLegComponent*> SplineLegs;
	UPROPERTY()
	ULegStepperComponent* LegStepper;

	static bool PreviousLegSide;

private:
	int32 ActiveLegIndex = 0;

	void ActivateBunch();
	void DeactivateBunch();
	void SetIsLegColliding(bool IsColliding, bool ChangeImmediately = false);
	void UpdateObstacleCollision(bool ChangeImmediately);
	bool CheckObstacleCollision();
	void PlayMovementAnimation();
	void ProcessMovementAnimation(float DeltaTime);

	bool IsPlayingMovementAnimation = false;
	bool IsLegActive = false;
	float LegAnimationProgress = 1.0f;
	float AnimationSpeed = 3.0f;

};
