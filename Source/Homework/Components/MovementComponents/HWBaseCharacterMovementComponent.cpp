// Fill out your copyright notice in the Description page of Project Settings.


#include "HWBaseCharacterMovementComponent.h"
#include "../../Characters/HWBaseCharacter.h"

float UHWBaseCharacterMovementComponent::GetMaxSpeed() const
{
	float Result = Super::GetMaxSpeed();
	if (bIsSprinting)
	{
		Result = SprintSpeed;
	}

	if (bIsOutOfStamina)
	{
		Result = OutOfStaminaSpeed;
	}

	return Result;
}

void UHWBaseCharacterMovementComponent::StartSprint()
{
	bIsSprinting = true;
	bForceMaxAccel = 1;
}

void UHWBaseCharacterMovementComponent::StopSprint()
{
	bIsSprinting = false;
	bForceMaxAccel = 0;
}

void UHWBaseCharacterMovementComponent::SetIsOutOfStamina(bool bIsOutOfStamina_In)
{
	bIsOutOfStamina = bIsOutOfStamina_In;
	//TODO add actions when stamina runs out ex. stop sprint
	if (bIsOutOfStamina == true)
	{
		StopSprint();
		
	}
}

bool UHWBaseCharacterMovementComponent::CanAttemptJump() const
{
	bool Result = Super::CanAttemptJump();
	
	if (bIsOutOfStamina)
	{
		Result = !bIsOutOfStamina;
	}

	return Result;
}

void UHWBaseCharacterMovementComponent::Prone()
{
	GEngine->AddOnScreenDebugMessage(-1, 2.0f, FColor::Black, FString::Printf(TEXT("Prone True")));
	bIsProning = true;
}


void UHWBaseCharacterMovementComponent::UnProne()
{
	GEngine->AddOnScreenDebugMessage(-1, 2.0f, FColor::Black, FString::Printf(TEXT("Prone False")));
	bIsProning = false;
}
