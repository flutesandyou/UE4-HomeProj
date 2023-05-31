// Fill out your copyright notice in the Description page of Project Settings.


#include "HomeworkBasePawn.h"
#include "Components/SphereComponent.h"
#include "Engine/CollisionProfile.h"
#include "GameFramework/FloatingPawnMovement.h"
#include "Homework/Components/MovementComponents/HWBasePawnMovementComponent.h"
#include "Kismet/GameplayStatics.h"
#include "Camera/PlayerCameraManager.h"
#include "../Homework.h"
#include "Camera/CameraComponent.h"
#include "Components/ArrowComponent.h"
#include "GameFramework/SpringArmComponent.h"

// Sets default values
AHomeworkBasePawn::AHomeworkBasePawn()
{
	CollisionComponent = CreateDefaultSubobject<USphereComponent>(TEXT("CollisionComponent"));
	CollisionComponent->SetSphereRadius(CollisionSphereRadius);
	CollisionComponent->SetCollisionProfileName(UCollisionProfile::Pawn_ProfileName);
	RootComponent = CollisionComponent;

	//MovementComponent = CreateDefaultSubobject<UPawnMovementComponent, UFloatingPawnMovement>(TEXT("Movement component"));
	MovementComponent = CreateDefaultSubobject<UPawnMovementComponent, UHWBasePawnMovementComponent>(TEXT("Movement component"));
	MovementComponent->SetUpdatedComponent(CollisionComponent);

	SpringArmComponent = CreateDefaultSubobject<USpringArmComponent>(TEXT("Spring arm"));
	SpringArmComponent->bUsePawnControlRotation = 1;
	SpringArmComponent->SetupAttachment(RootComponent);

	CameraComponent = CreateDefaultSubobject<UCameraComponent>(TEXT("Camera"));
	CameraComponent->SetupAttachment(SpringArmComponent);

#if	WITH_EDITORONLY_DATA
	ArrowComponent = CreateDefaultSubobject<UArrowComponent>(TEXT("Arrow"));
	ArrowComponent->SetupAttachment(RootComponent);
#endif
}

// Called to bind functionality to input
void AHomeworkBasePawn::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);
	PlayerInputComponent->BindAxis("Turn", this, &APawn::AddControllerYawInput);
	PlayerInputComponent->BindAxis("LookUp", this, &APawn::AddControllerPitchInput);
	PlayerInputComponent->BindAxis("MoveForward", this, &AHomeworkBasePawn::MoveForward);
	PlayerInputComponent->BindAxis("MoveRight", this, &AHomeworkBasePawn::MoveRight);
	PlayerInputComponent->BindAction("Jump", IE_Pressed, this, &AHomeworkBasePawn::Jump);
}

void AHomeworkBasePawn::MoveForward(float Value)
{
	InputForward = Value;
	if (Value != 0.0f)
	{
		AddMovementInput(CurrentViewActor->GetActorForwardVector(), Value);
	}
}

void AHomeworkBasePawn::MoveRight(float Value)
{
	InputRight = Value;
	if (Value != 0.0f)
	{
		AddMovementInput(CurrentViewActor->GetActorRightVector(), Value);
	}
}

void AHomeworkBasePawn::Jump()
{
	checkf(MovementComponent->IsA<UHWBasePawnMovementComponent>(), TEXT("AHomeworkBasePawn::Jump() can work only with UHWBasePawnMovementComponent"));
	UHWBasePawnMovementComponent* BaseMovement = StaticCast<UHWBasePawnMovementComponent*>(MovementComponent);
	BaseMovement->JumpStart();
}

void AHomeworkBasePawn::BeginPlay()
{
	Super::BeginPlay();
	APlayerCameraManager* CameraManager = UGameplayStatics::GetPlayerCameraManager(GetWorld(),0);
	CurrentViewActor = CameraManager->GetViewTarget();
	CameraManager->OnBlendComplete().AddUFunction(this, FName("OnBlendComplete"));
}

void AHomeworkBasePawn::OnBlendComplete()
{
	CurrentViewActor = GetController()->GetViewTarget();
	UE_LOG(LogCameras, Verbose, TEXT("AHomeworkBasePawn::OnBlendComplete() current view actor %s"), *CurrentViewActor->GetName());
}

