#include "ProceduralAnimation/SplineLegComponent.h"
#include "Components/SplineComponent.h"
#include "Components/SplineMeshComponent.h"
#include "ProceduralAnimation/LegStepperComponent.h"
#include "Creature/Creature.h"
#include <ProceduralAnimation/Utils.h>

USplineLegComponent::USplineLegComponent()
{
	PrimaryComponentTick.bCanEverTick = true;

	SplineComponent = CreateDefaultSubobject<USplineComponent>(TEXT("Spline"));
	static ConstructorHelpers::FObjectFinder<UStaticMesh> StaticMeshObj(TEXT("/Game/Creature/cylinder.cylinder"));
	StaticMesh = StaticMeshObj.Object;

	static ConstructorHelpers::FObjectFinder<UMaterialInterface> MaterialObj(TEXT("/Game/Creature/Black_M.Black_M"));
	//static ConstructorHelpers::FObjectFinder<UMaterialInterface> MaterialObj(TEXT("/Game/Creature/Liquid_M.Liquid_M"));
	Material = MaterialObj.Object;

	ControlRadiusA = FMath::RandRange(ControlRadiusRange.X, ControlRadiusRange.Y);
	ControlRadiusA *= FMath::RandBool() ? -1 : 1;
	ControlRadiusB = FMath::RandRange(ControlRadiusRange.X, ControlRadiusRange.Y);
	ControlRadiusB *= FMath::RandBool() ? -1 : 1;

	ControlSpeedA = FMath::RandRange(ControlSpeedRange.X, ControlSpeedRange.Y);
	ControlSpeedB = FMath::RandRange(ControlSpeedRange.X, ControlSpeedRange.Y);

	ScaleModifier = FMath::RandRange(1.05f, 1.2f);
}

void USplineLegComponent::SetIsLegActive(bool InIsLegActive)
{
	IsLegActive = InIsLegActive;
	if (IsLegActive)
	{
		PlayReachAnimation();
	}
	else
	{
		PlayHideAnimation();
	}
}

bool USplineLegComponent::IsPlayingSwapAnimation() const
{
	return IsPlayingHideAnimation || IsPlayingReachAnimation;
}

void USplineLegComponent::SetTargetInfo(const FTargetInfo& InTargetInfo)
{
	TargetInfo = InTargetInfo;
}

void USplineLegComponent::SetIsLegColliding(bool InIsColliding, bool ChangeImmediately)
{
	IsLegColliding = InIsColliding;
	TargetLegCollisionFactor = InIsColliding ? IncreasedLegCollisionFactor : NormalLegCollisionFactor;
	if (ChangeImmediately)
	{
		CurrentLegCollisionFactor = TargetLegCollisionFactor;
	}
}

void USplineLegComponent::PlayReachAnimation()
{
	LegIdleAnimation = 0.0f;
	LegAnimationProgress = 0.0f;
	IsPlayingReachAnimation = true;
}

void USplineLegComponent::PlayHideAnimation()
{
	LegAnimationProgress = 1.0f;
	IsPlayingHideAnimation = true;
}

void USplineLegComponent::BeginPlay()
{
	Super::BeginPlay();

	CreateSplineMeshes();
}

void USplineLegComponent::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

	const float speed = 2.0f;
	const float diff = TargetLegCollisionFactor - CurrentLegCollisionFactor;
	if (FMath::Abs(diff) > 0.01f)
	{
		CurrentLegCollisionFactor += FMath::Sign(diff) * speed * DeltaTime;
		CurrentLegCollisionFactor = FMath::Clamp(CurrentLegCollisionFactor, NormalLegCollisionFactor, IncreasedLegCollisionFactor);
	}

	PlayLegAnimation(DeltaTime);
	UpdateSpline();
}

void USplineLegComponent::CreateSplineMeshes()
{
	for (int i = 0; i < NumOfPointsInSpline; ++i)
	{
		USplineMeshComponent* splineMesh = NewObject<USplineMeshComponent>(this, USplineMeshComponent::StaticClass());
		splineMesh->SetStaticMesh(StaticMesh);
		splineMesh->SetForwardAxis(ESplineMeshAxis::X);
		splineMesh->SetMaterial(0, Material);
		splineMesh->SetReceivesDecals(false);

		splineMesh->RegisterComponentWithWorld(GetWorld());
		splineMesh->CreationMethod = EComponentCreationMethod::UserConstructionScript;
		splineMesh->SetMobility(EComponentMobility::Movable);
		splineMesh->AttachToComponent(SplineComponent, FAttachmentTransformRules::KeepRelativeTransform);
		SplineMeshes.Add(splineMesh);
	}
}

void USplineLegComponent::UpdateSpline()
{
	if (!IsLegActive && !IsPlayingHideAnimation)
	{
		return;
	}

	ACreature* owner = Cast<ACreature>(GetOwner());
	FVector start = IsValid(owner) ? owner->GetVisualBodyLocation() : GetOwner()->GetActorLocation();
	const FVector end = TargetInfo.TargetLocation;
	FVector up = FMath::Lerp(TargetInfo.TargetNormal, GetOwner()->GetActorUpVector(), 0.5);
	up.Normalize();

	//TODO remove if unused
	FVector startOffset = FVector::VectorPlaneProject(end - start, up);
	startOffset.Normalize();
	start += startOffset * 0.0f;

	const float collidingFactor = CurrentLegCollisionFactor;

	FVector controlA = start + (end - start) * 0.33f + up * ControlHeight * collidingFactor;
	FVector controlB = start + (end - start) * 0.66f + up * ControlHeight * 0.35 * (collidingFactor * 1.5);

	controlA += ControlOffsetA;
	controlB += ControlOffsetB;

	//DrawDebugSphere(GetWorld(), controlA, 15, 12, FColor::Purple, false, -1.0f, 0, 1.5f);
	//DrawDebugSphere(GetWorld(), controlB, 15, 12, FColor::Blue, false, -1.0f, 0, 1.5f);

	TArray<FVector> points;
	for (int i = 0; i < NumOfPointsInSpline; ++i)
	{
		float value = i / static_cast<float>(NumOfPointsInSpline - 1) * LegAnimationProgress;
		FVector worldPoint = Utils::GetCubicBezier(start, controlA, controlB, end, value);
		points.Add(worldPoint);
	}

	SplineComponent->SetSplinePoints(points, ESplineCoordinateSpace::Local, true);
	const int splinePoints = SplineComponent->GetNumberOfSplinePoints();

	for (int i = 0; i < splinePoints; ++i)
	{
		USplineMeshComponent* splineMesh = SplineMeshes[i];

		const FVector startPoint = SplineComponent->GetLocationAtSplinePoint(i, ESplineCoordinateSpace::Local);
		const FVector startTangent = SplineComponent->GetTangentAtSplinePoint(i, ESplineCoordinateSpace::Local);
		const FVector endPoint = SplineComponent->GetLocationAtSplinePoint(i + 1, ESplineCoordinateSpace::Local);
		const FVector endTangent = SplineComponent->GetTangentAtSplinePoint(i + 1, ESplineCoordinateSpace::Local);

		const int startIndex = splinePoints - i;
		const int endIndex = splinePoints - (i + 1);

		float startValue = startIndex / static_cast<float>(splinePoints);
		startValue = FMath::Pow(startValue, 3);
		float endValue = endIndex / static_cast<float>(splinePoints);
		endValue = FMath::Pow(endValue, 3);

		const float startScaleOverTime = FMath::Abs(FMath::Sin(LegIdleAnimation + startIndex * PI / 8));
		const float endScaleOverTime = FMath::Abs(FMath::Sin(LegIdleAnimation + endIndex * PI / 8));

		const float startLengthOverTime = FMath::Abs(FMath::Sin(startIndex / static_cast<float>(splinePoints) * PI));
		const float endLengthOverTime = FMath::Abs(FMath::Sin(endIndex / static_cast<float>(splinePoints) * PI));

		float startScale = FMath::Lerp(EndScale, StartScale, startValue);
		float endScale = FMath::Lerp(EndScale, StartScale, endValue);

		startScale *= (1.0 + ScaleModifier * startScaleOverTime * startLengthOverTime);
		endScale *= (1.0 + ScaleModifier * endScaleOverTime * endLengthOverTime);

		splineMesh->SetStartAndEnd(startPoint, startTangent, endPoint, endTangent);
		splineMesh->SetStartScale(FVector2D(startScale, startScale));
		splineMesh->SetEndScale(FVector2D(endScale, endScale));
	}
}

void USplineLegComponent::PlayLegAnimation(float DeltaTime)
{
	LegIdleAnimation += DeltaTime;
	ControlOffsetA = FVector(0, ControlRadiusA * FMath::Sin(LegIdleAnimation * ControlSpeedA), ControlRadiusA * FMath::Cos(LegIdleAnimation * ControlSpeedA));
	ControlOffsetB = FVector(0, ControlRadiusB * FMath::Sin(LegIdleAnimation * ControlSpeedB), ControlRadiusB * FMath::Cos(LegIdleAnimation * ControlSpeedB));
}

