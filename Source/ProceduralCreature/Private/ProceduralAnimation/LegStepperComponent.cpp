#include "ProceduralAnimation/LegStepperComponent.h"
#include "Kismet/KismetMathLibrary.h"
#include "Kismet/KismetSystemLibrary.h"
#include "ProceduralAnimation/Utils.h"

ULegStepperComponent::ULegStepperComponent()
{
	PrimaryComponentTick.bCanEverTick = true;
}

void ULegStepperComponent::BeginPlay()
{
	Super::BeginPlay();
}

void ULegStepperComponent::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

	if (!IsFarFromPoint)
	{
		const FTransform ownerTransfrom = GetOwner()->GetTransform();
		const FVector worldHomePos = ownerTransfrom.GetLocation();
		const FVector up = GetOwner()->GetActorUpVector();

		const FVector targetPoint = TargetInfo.TargetLocation;
		const FVector homePos = FVector::VectorPlaneProject(worldHomePos, up);
		const FVector endPos = FVector::VectorPlaneProject(targetPoint, up);
		FVector diff = targetPoint - worldHomePos;
		//FVector diff = homePos - endPos;

		//DrawDebugSphere(GetWorld(), homePos, 5.0f, 12, FColor::Blue, false, -1.0f, 10, 2.5f);
		//DrawDebugSphere(GetWorld(), endPos, 5.0f, 12, FColor::Blue, false, -1.0f, 10, 2.5f);
		//DrawDebugLine(GetWorld(), homePos, endPos, FColor::Blue, false, -1.0f, 10, 2.5f);

		//if leg is around the corner, move it sooner than usual
		const float collidingFactor = IsLegColliding ? StepAtDistanceCollisionFactor : 1.0f;

		const float sqrDist = (diff).SquaredLength();
		const float stepDist = FMath::Pow(WantStepAtDistance * collidingFactor, 2);
		
		if (sqrDist > stepDist)
		{
			IsFarFromPoint = true;
		}
	}
}

FTargetInfo ULegStepperComponent::GetTargetInfo() const
{
	return TargetInfo;
}

bool ULegStepperComponent::UpdateTarget(bool UseOwnerLocation, float MinAngle, float MaxAngle)
{
	const float angle = FMath::RandRange(MinAngle, MaxAngle);
	const FVector origin = UseOwnerLocation ? GetOwner()->GetActorLocation() : TargetInfo.TargetLocation;
	const FVector up = GetOwner()->GetActorUpVector();
	FVector dir = UKismetMathLibrary::RotateAngleAxis(GetOwner()->GetActorForwardVector(), angle, up);
	dir.Normalize();
	const float radius = FMath::RandRange(MinRadius, MaxRadius);
	FVector point = origin + dir * radius;

	FHitResult hit;
	FVector right = FVector::CrossProduct(up, dir);
	bool result = Utils::RaycastByArch(GetWorld(), origin, dir, up, right, radius, 60.0f, 
		MinDistFromHitToOrigin, hit, false, 20);
	
	if (result)
	{
		int32 sqrDist = (int32)(hit.ImpactPoint - origin).SquaredLength();
		if (sqrDist < WantStepAtDistance * WantStepAtDistance)
		{
			TargetInfo.TargetLocation = hit.ImpactPoint;
			TargetInfo.TargetNormal = hit.ImpactNormal;
			IsFarFromPoint = false;
			return true;
		}
	} 
	return false;
}

bool ULegStepperComponent::GetIsFarFromPoint() const
{
	return IsFarFromPoint;
}

void ULegStepperComponent::SetIsLegColliding(bool Value)
{
	IsLegColliding = Value;
}
