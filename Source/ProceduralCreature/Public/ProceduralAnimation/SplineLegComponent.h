#pragma once

#include "CoreMinimal.h"
#include "Components/SceneComponent.h"
#include "LegStepperComponent.h"
#include "SplineLegComponent.generated.h"


UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class PROCEDURALCREATURE_API USplineLegComponent : public USceneComponent
{
	GENERATED_BODY()

public:	
	USplineLegComponent();

	void SetIsLegActive(bool InIsLegActive);
	FORCEINLINE bool GetIsLegActive() { return IsLegActive; }
	bool IsPlayingSwapAnimation() const;
	void SetLegAnimationProgress(float Value) { LegAnimationProgress = Value; }

	void SetTargetInfo(const FTargetInfo& InTargetInfo);
	void SetIsLegColliding(bool InIsColliding, bool ChangeImmediately);


protected:
	virtual void BeginPlay() override;
	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Spline", Meta = (MakeEditWidget = true))
	float StartScale = 0.25f;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Spline", Meta = (MakeEditWidget = true))
	float EndScale = 0.025f;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Spline", Meta = (MakeEditWidget = true))
	float MeshScale = 0.05f;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Spline", Meta = (MakeEditWidget = true))
	float ControlHeight = 120.0f;

	UPROPERTY(EditDefaultsOnly, Category = "Spline")
	UStaticMesh* StaticMesh;
	UPROPERTY(EditDefaultsOnly, Category = "Spline")
	UMaterialInterface* Material;

private:	
	void CreateSplineMeshes();
	void UpdateSpline();

	UPROPERTY(Transient)
	TArray<class USplineMeshComponent*> SplineMeshes;
	UPROPERTY(Transient)
	class USplineComponent* SplineComponent;
	UPROPERTY()
	int32 NumOfPointsInSpline = 10;

	void PlayLegAnimation(float DeltaTime);
	void PlayReachAnimation();
	void PlayHideAnimation();

	FTargetInfo TargetInfo;
	FVector ControlA;
	FVector ControlB;

	bool IsPlayingHideAnimation = false;
	bool IsPlayingReachAnimation = false;
	bool IsLegActive = false;
	float LegAnimationProgress = 1.0f;

	bool IsLegColliding = false;
	float CurrentLegCollisionFactor = 1.0f;
	float TargetLegCollisionFactor = 1.0f;

	const float IncreasedLegCollisionFactor = 1.35f;
	const float NormalLegCollisionFactor = 1.0f;

	float LegIdleAnimation = 0.0f;
	FVector ControlOffsetA;
	FVector ControlOffsetB;

	FVector2D ControlRadiusRange = { 25, 35 };
	FVector2D ControlSpeedRange = { 1, 2 };
	
	float ControlRadiusA;
	float ControlSpeedA;
	float ControlRadiusB;
	float ControlSpeedB;

	float ScaleModifier;
};