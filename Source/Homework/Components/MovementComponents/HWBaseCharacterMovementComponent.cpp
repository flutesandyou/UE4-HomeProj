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

//void UHWBaseCharacterMovementComponent::Prone()
//{
//	GEngine->AddOnScreenDebugMessage(-1, 2.0f, FColor::Black, FString::Printf(TEXT("Prone True")));
//	bIsProning = true;
//}

void UHWBaseCharacterMovementComponent::Prone()
{
	if (!HasValidData())
	{
		return;
	}

	if (CanProneInCurrentState())
	{
		return;
	}

	if (BaseCharacterOwner->GetLocalRole() == ROLE_SimulatedProxy)
	{
		// restore collision size before proning
		ACharacter* DefaultCharacter = BaseCharacterOwner->GetClass()->GetDefaultObject<ACharacter>();
		BaseCharacterOwner->GetCapsuleComponent()->SetCapsuleSize(DefaultCharacter->GetCapsuleComponent()->GetUnscaledCapsuleRadius(), DefaultCharacter->GetCapsuleComponent()->GetUnscaledCapsuleHalfHeight());
		bShrinkProxyCapsule = true;
	}

	// Change collision size to proning dimensions
	const float ComponentScale = BaseCharacterOwner->GetCapsuleComponent()->GetShapeScale();
	const float OldUnscaledHalfHeight = BaseCharacterOwner->GetCapsuleComponent()->GetUnscaledCapsuleHalfHeight();
	const float OldUnscaledRadius = BaseCharacterOwner->GetCapsuleComponent()->GetUnscaledCapsuleRadius();
	// Height is not allowed to be smaller than radius.
	const float ClampedPronedHalfHeight = FMath::Max3(0.f, OldUnscaledRadius, PronedHalfHeight);
	BaseCharacterOwner->GetCapsuleComponent()->SetCapsuleSize(OldUnscaledRadius, ClampedPronedHalfHeight);
	float HalfHeightAdjust = (OldUnscaledHalfHeight - ClampedPronedHalfHeight);
	float ScaledHalfHeightAdjust = HalfHeightAdjust * ComponentScale;

	bForceNextFloorCheck = true;

	// OnStartProne takes the change from the Default size, not the current one (though they are usually the same).
	const float MeshAdjust = ScaledHalfHeightAdjust;
	ACharacter* DefaultCharacter = BaseCharacterOwner->GetClass()->GetDefaultObject<ACharacter>();
	HalfHeightAdjust = (DefaultCharacter->GetCapsuleComponent()->GetUnscaledCapsuleHalfHeight() - ClampedPronedHalfHeight);
	ScaledHalfHeightAdjust = HalfHeightAdjust * ComponentScale;

	AdjustProxyCapsuleSize();
//	BaseCharacterOwner->OnStartProne(HalfHeightAdjust, ScaledHalfHeightAdjust);

	// Don't smooth this change in mesh position
	if ((BaseCharacterOwner->GetLocalRole() == ROLE_SimulatedProxy) || (IsNetMode(NM_ListenServer) && BaseCharacterOwner->GetRemoteRole() == ROLE_AutonomousProxy))
	{
		FNetworkPredictionData_Client_Character* ClientData = GetPredictionData_Client_Character();
		if (ClientData)
		{
			ClientData->MeshTranslationOffset -= FVector(0.f, 0.f, MeshAdjust);
			ClientData->OriginalMeshTranslationOffset = ClientData->MeshTranslationOffset;
		}
	}
}

bool UHWBaseCharacterMovementComponent::CanProneInCurrentState() const
{

	return (IsFalling() || IsMovingOnGround()) && UpdatedComponent && !UpdatedComponent->IsSimulatingPhysics();
}


void UHWBaseCharacterMovementComponent::UnProne()
{
	GEngine->AddOnScreenDebugMessage(-1, 2.0f, FColor::Black, FString::Printf(TEXT("Prone False")));
	bIsProning = false;
}
