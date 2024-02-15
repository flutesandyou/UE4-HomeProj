// Fill out your copyright notice in the Description page of Project Settings.


#include "HWBaseCharacterMovementComponent.h"
#include "../../Characters/HWBaseCharacter.h"
#include "Components/CapsuleComponent.h"
#include "Curves/CurveVector.h"

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

void UHWBaseCharacterMovementComponent::StartMantle(const FMantlingMovementParameters& MantlingParameters)
{
	CurrentMantlingParameters = MantlingParameters;
	SetMovementMode(EMovementMode::MOVE_Custom, (uint8)ECustomMovementMode::CMOVE_Mantling);
}

void UHWBaseCharacterMovementComponent::EndMantle()
{
	SetMovementMode(MOVE_Walking);
}

bool UHWBaseCharacterMovementComponent::IsMantling() const
{
	return UpdatedComponent && MovementMode == MOVE_Custom && CustomMovementMode == (uint8)ECustomMovementMode::CMOVE_Mantling;
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

void UHWBaseCharacterMovementComponent::PhysCustom(float DeltaTime, int32 Iterations)
{
	switch (CustomMovementMode)
	{
		case (uint8)ECustomMovementMode::CMOVE_Mantling:
		{
			float ElapsedTime = GetWorld()->GetTimerManager().GetTimerElapsed(MantlingTimer) + CurrentMantlingParameters.StartTime;
			FVector MantlingCurveValue = CurrentMantlingParameters.MantlingCurve->GetVectorValue(ElapsedTime);
			float PositionAlpha = MantlingCurveValue.X;
			FVector NewLocation = FMath::Lerp(CurrentMantlingParameters.InitialLocation, CurrentMantlingParameters.TargetLocation, PositionAlpha);
			FRotator NewRotation = FMath::Lerp(CurrentMantlingParameters.InitialRotation, CurrentMantlingParameters.TargetRotation, PositionAlpha);

			FVector Delta =  NewLocation - GetActorLocation();

			FHitResult HitResult;
			SafeMoveUpdatedComponent(Delta, NewRotation, false, HitResult);
			break;
		}
	default:
		break;
	}
	

	Super::PhysCustom(DeltaTime, Iterations);
}

void UHWBaseCharacterMovementComponent::OnMovementModeChanged(EMovementMode PreviousMovementMode, uint8 PreviousCustomMode)
{
	Super::OnMovementModeChanged(PreviousMovementMode, PreviousCustomMode);
	if (MovementMode == MOVE_Swimming)
	{
		CharacterOwner->GetCapsuleComponent()->SetCapsuleSize(SwimmingCapsuleRadius, SwimmingCapsuleHalfHeigh);
	}
	else if (PreviousMovementMode == MOVE_Swimming)
	{
		ACharacter* DefaultCharacter = CharacterOwner->GetClass()->GetDefaultObject<ACharacter>();
		CharacterOwner->GetCapsuleComponent()->SetCapsuleSize(DefaultCharacter->GetCapsuleComponent()->GetUnscaledCapsuleRadius(), DefaultCharacter->GetCapsuleComponent()->GetUnscaledCapsuleHalfHeight(), true);
	}

	if (MovementMode == MOVE_Custom)
	{
		switch (CustomMovementMode)
		{
			case (uint8)ECustomMovementMode::CMOVE_Mantling:
			{
				GetWorld()->GetTimerManager().SetTimer(MantlingTimer, this, &UHWBaseCharacterMovementComponent::EndMantle, CurrentMantlingParameters.Duration, false);
				break;
			}
		default:
			break;
		}
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
