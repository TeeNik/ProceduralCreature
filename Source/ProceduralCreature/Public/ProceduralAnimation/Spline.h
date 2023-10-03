#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "Spline.generated.h"

class USplineComponent;

UCLASS()
class PROCEDURALCREATURE_API ASpline : public AActor
{
	GENERATED_BODY()
	
public:	
	ASpline();

protected:
	virtual void BeginPlay() override;
	virtual void Tick(float DeltaTime) override;
	virtual void OnConstruction(const FTransform& Transform) override;

	UFUNCTION(CallInEditor)
	void DrawCurve();

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Curve)
	FVector Start;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Curve, Meta = (MakeEditWidget = true))
	FVector Control;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Curve, Meta = (MakeEditWidget = true))
	FVector End;

	UPROPERTY(EditAnywhere, Category = "Spline")
	USplineComponent* SplineComponent;
	UPROPERTY(EditDefaultsOnly, Category = "Spline")
	UStaticMesh* StaticMesh;
	UPROPERTY(EditDefaultsOnly, Category = "Spline")
	UMaterialInterface* Material;
	
private:
	FVector GetBezierLocation(FVector start, FVector control, FVector end, float t) const;

};
