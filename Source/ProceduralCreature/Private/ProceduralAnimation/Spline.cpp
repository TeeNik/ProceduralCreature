#include "ProceduralAnimation/Spline.h"
#include "Components/SplineComponent.h"
#include "Components/SplineMeshComponent.h"

ASpline::ASpline()
{
	PrimaryActorTick.bCanEverTick = true;

	SplineComponent = CreateDefaultSubobject<USplineComponent>(TEXT("Spline"));
	SetRootComponent(SplineComponent);

}

void ASpline::BeginPlay()
{
	Super::BeginPlay();
	
}

void ASpline::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

void ASpline::OnConstruction(const FTransform& Transform)
{
	Super::OnConstruction(Transform);

	FVector current = GetActorLocation();
	const int NumOfPoint = 10;
	const FTransform transform = GetActorTransform();

	const FVector start = transform.TransformPosition(Start);
	const FVector control = transform.TransformPosition(Control);
	const FVector end = transform.TransformPosition(End);

	TArray<FVector> points;


	for (int i = 0; i <= NumOfPoint; ++i)
	{
		float value = i / (float)NumOfPoint;
		FVector worldPoint = GetBezierLocation(start, control, end, value);
		FVector localPoint = GetBezierLocation(Start, Control, End, value);
		points.Add(localPoint);
		DrawDebugLine(GetWorld(), current, worldPoint, FColor::Blue, false, 20, 0, 2.5);
		current = worldPoint;
	}

	SplineComponent->SetSplinePoints(points, ESplineCoordinateSpace::Local, true);
	const int splinePoints = SplineComponent->GetNumberOfSplinePoints();

	for (int i = 0; i < splinePoints - 1; ++i)
	{
		USplineMeshComponent* splineMesh = NewObject<USplineMeshComponent>(this, USplineMeshComponent::StaticClass());
		splineMesh->SetStaticMesh(StaticMesh);
		splineMesh->SetForwardAxis(ESplineMeshAxis::X);
		splineMesh->SetMaterial(0, Material);

		splineMesh->RegisterComponentWithWorld(GetWorld());
		splineMesh->CreationMethod = EComponentCreationMethod::UserConstructionScript;
		splineMesh->SetMobility(EComponentMobility::Movable);
		splineMesh->AttachToComponent(SplineComponent, FAttachmentTransformRules::KeepRelativeTransform);

		const FVector startPoint = SplineComponent->GetLocationAtSplinePoint(i, ESplineCoordinateSpace::Local);
		const FVector startTangent = SplineComponent->GetTangentAtSplinePoint(i, ESplineCoordinateSpace::Local);
		const FVector endPoint = SplineComponent->GetLocationAtSplinePoint(i+1, ESplineCoordinateSpace::Local);
		const FVector endTangent = SplineComponent->GetTangentAtSplinePoint(i+1, ESplineCoordinateSpace::Local);

		splineMesh->SetStartAndEnd(startPoint, startTangent, endPoint, endTangent);
	}
}

void ASpline::DrawCurve()
{
	
}

FVector ASpline::GetBezierLocation(FVector start, FVector control, FVector end, float t) const
{
	FVector a = FMath::Lerp(start, control, t);
	FVector b = FMath::Lerp(control, end, t);
	return FMath::Lerp(a, b, t);
}
