// Fill out your copyright notice in the Description page of Project Settings.


#include "HWBaseCharacterAnimInstance.h"
#include "../HWBaseCharacter.h"
#include "../../Components/MovementComponents/HWBaseCharacterMovementComponent.h"

void UHWBaseCharacterAnimInstance::NativeBeginPlay()
{
	Super::NativeBeginPlay();

	checkf(TryGetPawnOwner()->IsA<AHWBaseCharacter>(), TEXT("UHWBaseCharacterAnimInstance::NativeBeginPlay() UHWBaseCharacterAnimInstance can only be used with AHWBaseCharacter"));
	CachedBaseCharacter = StaticCast<AHWBaseCharacter*>(TryGetPawnOwner());
}

void UHWBaseCharacterAnimInstance::NativeUpdateAnimation(float DeltaSeconds)
{
	Super::NativeUpdateAnimation(DeltaSeconds);

	if (!CachedBaseCharacter.IsValid())
	{
		return;
	}
	UHWBaseCharacterMovementComponent* CharacterMovement = CachedBaseCharacter->GetBaseCharacterMovementComponent();
	Speed = CharacterMovement->Velocity.Size();
	bIsFalling = CharacterMovement->IsFalling();
    bIsCrouching = CharacterMovement->IsCrouching();
    bIsSprinting = CharacterMovement->IsSprinting();
	bIsOutOfStamina = CharacterMovement->IsOutOfStamina();
	bIsProning = CharacterMovement->IsProning();
	bIsSwimming = CharacterMovement->IsSwimming();
	bIsOnLadder = CharacterMovement->IsOnLadder();
	if (bIsOnLadder)
	{
		LadderSpeedRatio = CharacterMovement->GetLadderSpeedRatio();
	}
    
    //IK
    RightFootEffectorLocation = FVector(CachedBaseCharacter->GetIKRightFootOffset() * -1.0f, 0.0f, 0.0f);
    LeftFootEffectorLocation = FVector(CachedBaseCharacter->GetIKLeftFootOffset(), 0.0f, 0.0f);

	HipDisplacement = CachedBaseCharacter->GetHipOffset();
}
