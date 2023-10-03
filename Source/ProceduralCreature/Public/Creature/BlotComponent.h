#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "GameFramework/Actor.h"
#include "ProceduralAnimation/LegStepperComponent.h"
#include "BlotComponent.generated.h"

UCLASS()
class ABlot : public AActor
{
	GENERATED_BODY()

public:
	ABlot();

	UFUNCTION(BlueprintImplementableEvent, Category = Blot)
	void OnBlotPlaced();
};


UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class UBlotComponent : public UActorComponent
{
	GENERATED_BODY()

public:	
	UBlotComponent();

protected:
	virtual void BeginPlay() override;

	UFUNCTION()
	void OnLegStepped(const struct FTargetInfo& TargetInfo);

	UPROPERTY(EditDefaultsOnly, Category = Blot)
	TSubclassOf<ABlot> BlotToSpawn;
	UPROPERTY(EditDefaultsOnly, Category = Blot)
	int32 NumOfBlots = 20;

private:
	UPROPERTY()
	TArray<ABlot*> BlotsPool;
	int32 CurrentBlotIndex = 0;

};
