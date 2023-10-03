#include "ProceduralAnimation/Utils.h"
#include "Kismet/KismetMathLibrary.h"

bool Utils::RaycastByArch(const UWorld* InWorld, FVector origin, FVector forward, FVector up, FVector right,
	float RaycastHeight, float RaycastForwardDist, float MinDistFromHitToOrigin, FHitResult& OutHitResult,
	bool DrawDebug, float DebugLineLifetime)
{
	const float angle = 270.0f;
	const float step = 15.0f;

	const FVector actorUp = up;
	const FVector actorForward = forward;
	const FVector actorRight = right;

	for (int i = 0; i <= angle / step; ++i)
	{
		FVector rayUp = UKismetMathLibrary::RotateAngleAxis(actorUp, step * i, actorRight);
		FVector upPos = origin + rayUp * RaycastHeight;
		FVector forward = UKismetMathLibrary::RotateAngleAxis(actorForward, step * i, actorRight);
		FVector forwardPos = upPos + forward * RaycastForwardDist;

		if (DrawDebug)
		{
			DrawDebugLine(InWorld, origin, upPos, FColor::Green, false, DebugLineLifetime, 0, 2);
			DrawDebugLine(InWorld, upPos, forwardPos, FColor::Green, false, DebugLineLifetime, 0, 2);
		}

		bool result = InWorld->LineTraceSingleByChannel(OutHitResult, origin, upPos, ECollisionChannel::ECC_WorldStatic);
		
		//if forward collision is too close, ignore it
		if (result)
		{
			float distFromHitToOrigin = FVector::Dist(OutHitResult.ImpactPoint, origin);
			if (distFromHitToOrigin < MinDistFromHitToOrigin)
			{
				result = false;
			}
		}
		
		if (!result)
		{
			result = InWorld->LineTraceSingleByChannel(OutHitResult, upPos, forwardPos, ECollisionChannel::ECC_WorldStatic);
		}
		if (result)
		{
			if (DrawDebug)
			{
				DrawDebugSphere(InWorld, OutHitResult.ImpactPoint, 5.0f, 12, FColor::Green, false, DebugLineLifetime, 0, 2);
			}
			return true;
		}
	}

	return false;
}

float Utils::AngleBetweenTwoVectors(const FVector& A, const FVector& B)
{
	const float dot = UKismetMathLibrary::Dot_VectorVector(A, B);
	return UKismetMathLibrary::DegAcos(dot);
}

FVector Utils::GetQuadraticBezier(FVector start, FVector control, FVector end, float t)
{
	const float oneMinusT = 1.0f - t;
	return oneMinusT * oneMinusT * start +
		2 * oneMinusT * t * control +
		t * t * end;
}

FVector Utils::GetCubicBezier(FVector start, FVector controlA, FVector controlB, FVector end, float t)
{
	const float oneMinusT = 1.0f - t;
	return oneMinusT * oneMinusT * oneMinusT * start +
		3.0f * oneMinusT * oneMinusT * t * controlA +
		3.0f * oneMinusT * t * t * controlB +
		t * t * t * end;
}
