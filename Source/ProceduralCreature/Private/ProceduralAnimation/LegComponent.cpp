#include "ProceduralAnimation/LegComponent.h"
#include "ProceduralAnimation/SplineLegComponent.h"
#include "ProceduralAnimation/LegStepperComponent.h"

bool ULegComponent::PreviousLegSide = false;

ULegComponent::ULegComponent()
{
	PrimaryComponentTick.bCanEverTick = true;

	LegStepper = CreateDefaultSubobject<ULegStepperComponent>(TEXT("LegStepper"));
	for (int i = 0; i < NumOfBunches * NumOfLegsInBunch; ++i)
	{
		FName name(FString("SplineLeg_") + FString::FromInt(i));
		USplineLegComponent* splineLeg = CreateDefaultSubobject<USplineLegComponent>(name);
		SplineLegs.Add(splineLeg);
	}
}

void ULegComponent::Init(float MinAngle, float MaxAngle)
{
	LegStepper->UpdateTarget(true, MinAngle, MaxAngle);
	ActivateBunch();
	PlayMovementAnimation();
}

bool ULegComponent::IsFarFromPoint() const
{
	return LegStepper->GetIsFarFromPoint();
}

bool ULegComponent::SwapLegs()
{
	bool side = FMath::RandBool();
	float minAngle = PreviousLegSide ? -35 : 15;
	float maxAngle = PreviousLegSide ? -15 : 35;
	const bool isNewTargetFound = LegStepper->UpdateTarget(true, minAngle, maxAngle);

	if (isNewTargetFound)
	{
		PreviousLegSide = !PreviousLegSide;
		DeactivateBunch();
		ActiveLegIndex = (ActiveLegIndex + 1) % 2;
		ActivateBunch();

		PlayMovementAnimation();
	}

	return isNewTargetFound;
}

bool ULegComponent::IsMoving() const
{
	return SplineLegs.Num() > 0 && IsPlayingMovementAnimation;
}

void ULegComponent::BeginPlay()
{
	Super::BeginPlay();
}

void ULegComponent::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

	ProcessMovementAnimation(DeltaTime);
	UpdateObstacleCollision(false);
}

void ULegComponent::ActivateBunch()
{
	int index = ActiveLegIndex * NumOfLegsInBunch;
	const FTargetInfo info = LegStepper->GetTargetInfo();
	for (int i = index; i < index + NumOfLegsInBunch; ++i)
	{
		SplineLegs[i]->SetIsLegActive(true);
		SplineLegs[i]->SetTargetInfo(info);
	}
	UpdateObstacleCollision(true);
}

void ULegComponent::DeactivateBunch()
{
	int index = ActiveLegIndex * NumOfLegsInBunch;
	for (int i = index; i < index + NumOfLegsInBunch; ++i)
	{
		SplineLegs[i]->SetIsLegActive(false);
	}
}

void ULegComponent::UpdateObstacleCollision(bool ChangeImmediately)
{
	SetIsLegColliding(CheckObstacleCollision(), ChangeImmediately);
}

void ULegComponent::SetIsLegColliding(bool IsColliding, bool ChangeImmediately)
{
	int index = ActiveLegIndex * NumOfLegsInBunch;
	const FTargetInfo info = LegStepper->GetTargetInfo();
	for (int i = index; i < index + NumOfLegsInBunch; ++i)
	{
		SplineLegs[i]->SetIsLegColliding(IsColliding, ChangeImmediately);
	}
	LegStepper->SetIsLegColliding(IsColliding);
}

bool ULegComponent::CheckObstacleCollision()
{
	FHitResult hit;
	const FVector origin = GetOwner()->GetActorLocation();
	const FTargetInfo info = LegStepper->GetTargetInfo();
	FVector target = info.TargetLocation;
	target = info.TargetLocation + info.TargetNormal * 20;
	return GetWorld()->LineTraceSingleByChannel(hit, origin, target, ECollisionChannel::ECC_WorldStatic);
}

void ULegComponent::PlayMovementAnimation()
{
	LegAnimationProgress = 0.0f;
	IsPlayingMovementAnimation = true;
}

void ULegComponent::ProcessMovementAnimation(float DeltaTime)
{
	if (!IsPlayingMovementAnimation)
	{
		return;
	}

	LegAnimationProgress += DeltaTime * AnimationSpeed;
	LegAnimationProgress = FMath::Clamp(LegAnimationProgress, 0.0f, 1.0f);

	for (int i = 0; i < SplineLegs.Num(); ++i)
	{
		const bool IsActive = SplineLegs[i]->GetIsLegActive();
		const float value = IsActive ? LegAnimationProgress : 1.0 - LegAnimationProgress;
		SplineLegs[i]->SetLegAnimationProgress(value);
	}

	if (LegAnimationProgress >= 1.0f || LegAnimationProgress <= 0)
	{
		IsPlayingMovementAnimation = false;
		OnLegAnimationFinished.Broadcast(LegStepper->GetTargetInfo());
	}
}
