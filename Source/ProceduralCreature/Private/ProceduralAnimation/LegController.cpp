#include "ProceduralAnimation/LegController.h"
#include "ProceduralAnimation/LegComponent.h"

ULegController::ULegController()
{
	PrimaryComponentTick.bCanEverTick = true;

	for (int i = 0; i < NumOfLegs; ++i)
	{
		FName name(FString("Leg_") +  FString::FromInt(i));
		ULegComponent* leg = CreateDefaultSubobject<ULegComponent>(name);
		Legs.Add(leg);
	}
}

void ULegController::BeginPlay()
{
	Super::BeginPlay();

	float angle = 0.0f;
	float anglePeriod = 360.0f / NumOfLegs;

	for (int i = 0; i < NumOfLegs; ++i)
	{
		Legs[i]->Init(angle, angle + anglePeriod);
		angle += anglePeriod;
	}
}

void ULegController::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

	int32 numOfMovingLegs = 0;
	for (int i = 0; i < NumOfLegs; ++i)
	{
		if (Legs[i]->IsMoving())
		{
			++numOfMovingLegs;
		}
	}

	if (numOfMovingLegs < NumOfMovingLegsAtOnes)
	{
		for (int i = 0; i < NumOfLegs; ++i)
		{
			if (Legs[i]->IsFarFromPoint() && !Legs[i]->IsMoving())
			{
				if (Legs[i]->SwapLegs())
				{
					++numOfMovingLegs;
				}
			}
			if (numOfMovingLegs >= NumOfMovingLegsAtOnes) // ==
			{
				break;
			}
		}
	}
}