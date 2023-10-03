#include "Creature/BlotComponent.h"
#include "ProceduralAnimation/LegComponent.h"
#include "Kismet/KismetMathLibrary.h"

ABlot::ABlot()
{
}

UBlotComponent::UBlotComponent()
{
	PrimaryComponentTick.bCanEverTick = true;
}

void UBlotComponent::BeginPlay()
{
	Super::BeginPlay();

	TArray<ULegComponent*> Legs;
	GetOwner()->GetComponents<ULegComponent>(Legs);
	for (int i = 0; i < Legs.Num(); ++i)
	{
		ULegComponent* leg = Cast<ULegComponent>(Legs[i]);
		leg->OnLegAnimationFinished.AddUFunction(this, TEXT("OnLegStepped"));
	}

	if (BlotToSpawn)
	{
		for (int i = 0; i < NumOfBlots; ++i)
		{
			ABlot* blot = Cast<ABlot>(GetWorld()->SpawnActor(BlotToSpawn));
			blot->SetActorHiddenInGame(true);
			BlotsPool.Add(blot);
		}
	}

}

void UBlotComponent::OnLegStepped(const FTargetInfo& TargetInfo)
{
	if (BlotsPool.Num() > 0)
	{
		ABlot* blot = BlotsPool[CurrentBlotIndex];
		CurrentBlotIndex = (CurrentBlotIndex + 1) % NumOfBlots;

		const FVector Location = TargetInfo.TargetLocation;
		const FVector X = FMath::VRand() * (FVector::OneVector - FVector::UpVector);
		FRotator NewRot = UKismetMathLibrary::MakeRotFromZ(TargetInfo.TargetNormal);
		blot->SetActorLocationAndRotation(Location, NewRot);

		FQuat additionalRot(blot->GetActorUpVector(), FMath::RandRange(0, 360));
		blot->AddActorWorldRotation(additionalRot);
		blot->SetActorHiddenInGame(false);
		blot->OnBlotPlaced();
	}
}