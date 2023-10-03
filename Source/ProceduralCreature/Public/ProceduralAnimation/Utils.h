#pragma once

#include "CoreMinimal.h"

class PROCEDURALCREATURE_API Utils
{
public:
	static bool RaycastByArch(const UWorld* InWorld, FVector origin, FVector forward, FVector up, FVector right,
		float RaycastHeight, float RaycastForwardDist, float MinDistFromHitToOrigin, FHitResult& OutHitResult, bool DrawDebug = false, float DebugLineLifetime = -1.0f);

	static float AngleBetweenTwoVectors(const FVector& A, const FVector& B);

	static FVector GetQuadraticBezier(FVector start, FVector control, FVector end, float t);
	static FVector GetCubicBezier(FVector start, FVector controlA, FVector controlB, FVector end, float t);

};
