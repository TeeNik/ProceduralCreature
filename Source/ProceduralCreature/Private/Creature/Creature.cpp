#include "Creature/Creature.h"

#include "Camera/CameraComponent.h"
#include "Components/CapsuleComponent.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "GameFramework/FloatingPawnMovement.h"
#include "GameFramework/PawnMovementComponent.h"
#include "GameFramework/SpringArmComponent.h"
#include "Kismet/KismetMathLibrary.h"
#include "Kismet/GameplayStatics.h"
#include "GameFramework/FloatingPawnMovement.h"
#include "ProceduralAnimation/Utils.h"

// Sets default values
ACreature::ACreature()
{
	PrimaryActorTick.bCanEverTick = true;

	TurnRateGamepad = 50.f;

	bUseControllerRotationPitch = false;
	bUseControllerRotationYaw = false;
	bUseControllerRotationRoll = false;

	MovementComp = CreateDefaultSubobject<UFloatingPawnMovement>(TEXT("MovementComp"));

	CapsuleComponent = CreateDefaultSubobject<UCapsuleComponent>(TEXT("CapsuleComp"));
	SetRootComponent(CapsuleComponent);

	//GetCharacterMovement()->bOrientRotationToMovement = true; // Character moves in the direction of input...	
	//GetCharacterMovement()->RotationRate = FRotator(0.0f, 500.0f, 0.0f); // ...at this rotation rate
	//
	//// Note: For faster iteration times these variables, and many more, can be tweaked in the Character Blueprint
	//// instead of recompiling to adjust them
	//GetCharacterMovement()->JumpZVelocity = 700.f;
	//GetCharacterMovement()->AirControl = 0.35f;
	//GetCharacterMovement()->MaxWalkSpeed = 500.f;
	//GetCharacterMovement()->MinAnalogWalkSpeed = 20.f;
	//GetCharacterMovement()->BrakingDecelerationWalking = 2000.f;

	// Create a camera boom (pulls in towards the player if there is a collision)
	CameraBoom = CreateDefaultSubobject<USpringArmComponent>(TEXT("CameraBoom"));
	CameraBoom->SetupAttachment(RootComponent);
	CameraBoom->TargetArmLength = 400.0f; // The camera follows at this distance behind the character	
	CameraBoom->bUsePawnControlRotation = true; // Rotate the arm based on the controller

	// Create a follow camera
	FollowCamera = CreateDefaultSubobject<UCameraComponent>(TEXT("FollowCamera"));
	FollowCamera->SetupAttachment(CameraBoom, USpringArmComponent::SocketName); // Attach the camera to the end of the boom and let the boom adjust to match the controller orientation
	//FollowCamera->SetupAttachment(RootComponent);
	FollowCamera->bUsePawnControlRotation = false; // Camera does not rotate relative to arm
}

FVector ACreature::GetVisualBodyLocation_Implementation() const
{
	return GetActorLocation();
}

void ACreature::BeginPlay()
{
	Super::BeginPlay();

	TargetForward = GetActorForwardVector();
	UpdateHeight(1000);
}

void ACreature::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	CheckForwardSurface(1.0f);
	//UpdateHeight(DeltaTime);

}

void ACreature::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);

	check(PlayerInputComponent);
	PlayerInputComponent->BindAxis("Move Forward / Backward", this, &ACreature::MoveForward);
	PlayerInputComponent->BindAxis("Move Right / Left", this, &ACreature::MoveRight);

	PlayerInputComponent->BindAxis("Turn Right / Left Mouse", this, &ACreature::TurnWithMouse);
	PlayerInputComponent->BindAxis("Turn Right / Left Gamepad", this, &ACreature::TurnAtRate);
	PlayerInputComponent->BindAxis("Look Up / Down Mouse", this, &ACreature::LookUpWithMouse);
	PlayerInputComponent->BindAxis("Look Up / Down Gamepad", this, &ACreature::LookUpAtRate);
}

void ACreature::MoveForward(float Value)
{
	FVector origin = GetActorLocation();
	FVector cameraPos = GetFollowCamera()->GetComponentLocation();
	FVector dir = origin - cameraPos;
	FVector proj = FVector::VectorPlaneProject(dir, GetActorUpVector());
	proj.Normalize();


	if ((Controller != nullptr) && (Value != 0.0f))
	{
		float deltaTime = UGameplayStatics::GetWorldDeltaSeconds(GetWorld());
		FVector newForward = FMath::Lerp(GetActorForwardVector(), TargetForward, 0.1f);
		FVector newUp = FMath::Lerp(GetActorUpVector(), TargetUp, 0.1f);;
		FVector newRight = FVector::CrossProduct(newUp, newForward);
		FRotator newRot = UKismetMathLibrary::MakeRotationFromAxes(newForward, newRight, newUp);

		SetActorLocation(origin + TargetForward * Speed * deltaTime);
		//AddMovementInput(TargetForward * Speed * deltaTime, Value);
		SetActorRotation(newRot);

		UpdateHeight(deltaTime);
	}
	IsMovingForward = Value != 0.0f;
}

void ACreature::MoveRight(float Value)
{
	if ( (Controller != nullptr) && (Value != 0.0f) && IsMovingForward)
	{
		float deltaTime = UGameplayStatics::GetWorldDeltaSeconds(GetWorld());
		FVector forward = GetActorForwardVector();
		FVector up = GetActorUpVector();
		FVector newForward = UKismetMathLibrary::RotateAngleAxis(forward, Value * 135 * deltaTime, up);
		FVector newRight = FVector::CrossProduct(up, newForward);
		FRotator newRot = UKismetMathLibrary::MakeRotationFromAxes(newForward, newRight, up);

		SetActorRotation(newRot);
	}
}

void ACreature::TurnAtRate(float Rate)
{
	AddControllerYawInput(Rate * TurnRateGamepad * GetWorld()->GetDeltaSeconds());
}

void ACreature::LookUpAtRate(float Rate)
{
	AddControllerPitchInput(Rate * TurnRateGamepad * GetWorld()->GetDeltaSeconds());
}

void ACreature::TurnWithMouse(float Rate)
{
	AddControllerYawInput(Rate);
}

void ACreature::LookUpWithMouse(float Rate)
{
	AddControllerPitchInput(Rate);
}

void ACreature::CheckForwardSurface(float DistModifier)
{
	FHitResult hit1;
	bool isHit = RaycastForwardSurface(15 * DistModifier, RaycastForwardDist * DistModifier, hit1);
	FHitResult hit2;
	isHit &= RaycastForwardSurface(30 * DistModifier, RaycastForwardDist * DistModifier, hit2);

	if (isHit)
	{
		FVector newForward = hit2.ImpactPoint - hit1.ImpactPoint;
		newForward.Normalize();
		TargetForward = newForward;
		TargetUp = hit1.ImpactNormal;
	}
	else
	{
		CheckForwardSurface(DistModifier * 1.1f);
	}
}

bool ACreature::RaycastForwardSurface(float InRaycastHeight, float InRaycastForwardDist, FHitResult& OutHitResult)
{
	const FVector origin = GetActorLocation();
	const FVector actorUp = GetActorUpVector();
	const FVector actorForward = GetActorForwardVector();
	const FVector actorRight = GetActorRightVector();

	return Utils::RaycastByArch(GetWorld(), origin, actorForward, actorUp, actorRight,
		InRaycastHeight, InRaycastForwardDist, -1.0f, OutHitResult, false);
}

void ACreature::UpdateHeight(float DeltaTime)
{
	const FVector origin = GetActorLocation();
	const FVector actorUp = GetActorUpVector();
	FHitResult hit;
	FVector start = origin + actorUp * 200.0f;
	FVector end = origin - actorUp * 1000;
	const bool result = GetWorld()->LineTraceSingleByChannel(hit, start, end, ECollisionChannel::ECC_WorldStatic);
	if (result)
	{
		float height = FVector::Dist(origin, hit.ImpactPoint);
		float diff = FMath::Abs(BaseHeight - height);
		//if (diff > 5.0f)
		{
			FVector targetHeight = hit.ImpactPoint + actorUp * BaseHeight;
			FVector dir = targetHeight - origin;
			dir.Normalize();
			const float length = FMath::Clamp(100.0f * DeltaTime, 0.0f, diff);
			FVector newPos = origin + dir * length;
			SetActorLocation(newPos);
		}
	}
}