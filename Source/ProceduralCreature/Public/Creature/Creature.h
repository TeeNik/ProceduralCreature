#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Pawn.h"
#include "Creature.generated.h"

UCLASS()
class PROCEDURALCREATURE_API ACreature : public APawn
{
	GENERATED_BODY()

public:
	ACreature();

	UFUNCTION(BlueprintNativeEvent)
	FVector GetVisualBodyLocation() const;
	
protected:
	virtual void BeginPlay() override;
	virtual void Tick(float DeltaTime) override;
	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Camera, meta = (AllowPrivateAccess = "true"))
	class USpringArmComponent* CameraBoom;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Camera, meta = (AllowPrivateAccess = "true"))
	class UCameraComponent* FollowCamera;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Movement, meta = (AllowPrivateAccess = "true"))
	class UFloatingPawnMovement* MovementComp;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, meta = (AllowPrivateAccess = "true"))
	class UCapsuleComponent* CapsuleComponent;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category=Input)
	float TurnRateGamepad;

	void MoveForward(float Value);
	void MoveRight(float Value);
	void TurnAtRate(float Rate);
	void LookUpAtRate(float Rate);

	void TurnWithMouse(float Rate);
	void LookUpWithMouse(float Rate);

	FORCEINLINE class USpringArmComponent* GetCameraBoom() const { return CameraBoom; }
	FORCEINLINE class UCameraComponent* GetFollowCamera() const { return FollowCamera; }

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
	float Speed = 400.0f;
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
	float BaseHeight = 50.0f;

	void CheckForwardSurface(float DistModifier);
	bool RaycastForwardSurface(float InRaycastHeight, float InRaycastForwardDist, FHitResult& OutHitResult);
	void UpdateHeight(float DeltaTime);

private:
	FVector TargetForward;
	FVector TargetUp;
	bool IsMovingForward = false;
	float RaycastForwardDist = 50.0f;
};
