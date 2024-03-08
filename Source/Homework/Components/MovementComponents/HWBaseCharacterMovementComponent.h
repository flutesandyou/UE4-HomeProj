// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/CharacterMovementComponent.h"
#include <../Components/LedgeDetectorComponents/LedgeDetectorComponent.h>
#include "HWBaseCharacterMovementComponent.generated.h"

struct FMantlingMovementParameters
{
	FVector InitialLocation = FVector::ZeroVector;
	FRotator InitialRotation = FRotator::ZeroRotator;
	UPrimitiveComponent* HitComponent = nullptr;
	FVector TargetLocation = FVector::ZeroVector;
	FRotator TargetRotation= FRotator::ZeroRotator;

	FVector InitialAnimationLocation = FVector::ZeroVector;

	float Duration = 1.0f;
	float StartTime = 0.0f;

	UCurveVector* MantlingCurve;
};

UENUM(BlueprintType)
enum class ECustomMovementMode : uint8
{
	CMOVE_None = 0 UMETA(DisplayName = "None"),
	CMOVE_Mantling UMETA(DisplayName = "Mantling"),
	CMOVE_Ladder UMETA(DisplayName = "Ladder"),
	CMOVE_Max UMETA(Hidden)
};

UENUM(BlueprintType)
enum class EDetachFromLadderMethod : uint8
{
	Fall = 0,
	ReachingTop,
	ReachingBottom,
	JumpOff
};

class AHWBaseCharacter;

UCLASS()
class HOMEWORK_API UHWBaseCharacterMovementComponent : public UCharacterMovementComponent
{
	GENERATED_BODY()

public:
	virtual void PhysicsRotation(float DeltaTime) override;
	FORCEINLINE bool IsSprinting() { return bIsSprinting; }
	FORCEINLINE bool IsProning() { return bIsProning; }
	virtual float GetMaxSpeed() const override;
	virtual bool CanAttemptJump() const override;
	
	void StartSprint();
	void StopSprint();

	// mantleing

	void StartMantle(const FMantlingMovementParameters& MantlingParameters);
	void EndMantle();
	bool IsMantling() const;

	// ladder
	void AttachToLadder(const class ALadder* Ladder);
	float GetActorToCurrentLadderProjection(const FVector& Location) const;
	float GetLadderSpeedRatio() const;
	void DetachFromLadder(EDetachFromLadderMethod DetachFromLadderMethod = EDetachFromLadderMethod::Fall);
	bool IsOnLadder() const;
	const class ALadder* GetCurrentLadder();

	// proning
	void Prone();
	void UnProne();

	// stamina feature

	bool bIsOutOfStamina;
	FORCEINLINE bool IsOutOfStamina() const { return bIsOutOfStamina; }
	void SetIsOutOfStamina(bool bIsOutOfStamina_In);

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Character movement: sprint", meta = (ClampMin = 0.0f, UIMin = 0.0f))
	float PressTime = 0.5f;

protected:
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="Character movement: sprint", meta=(ClampMin = 0.0f, UIMin = 0.0f))
	float SprintSpeed = 1200.0f;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Character movement: sprint", meta = (ClampMin = 0.0f, UIMin = 0.0f))
	float OutOfStaminaSpeed = 150.0f;

	virtual void PhysCustom(float DeltaTime, int32 Iterations) override;

	void PhysMantling(float DeltaTime, int32 Iterations);
	void PhysLadder(float DeltaTime, int32 Iterations);

	virtual void OnMovementModeChanged(EMovementMode PreviousMovementMode, uint8 PreviousCustomMode) override;

	UPROPERTY(Category = "Character Movement: Swimming", EditAnywhere, BlueprintReadWrite, meta = (ClampMin = "0", UIMin = "0"))
	float SwimmingCapsuleRadius = 60.0f;

	UPROPERTY(Category = "Character Movement: Swimming", EditAnywhere, BlueprintReadWrite, meta = (ClampMin = "0", UIMin = "0"))
	float SwimmingCapsuleHalfHeigh = 60.0f;

	UPROPERTY(Category = "Character Movement: Ladder", EditAnywhere, BlueprintReadWrite, meta = (ClampMin = "0", UIMin = "0"))
	float ClimbingOnLadderMaxSpeed = 200.0f;

	UPROPERTY(Category = "Character Movement: Ladder", EditAnywhere, BlueprintReadWrite, meta = (ClampMin = "0", UIMin = "0"))
	float LadderToCharacterOffset = 60.0f;
	
	UPROPERTY(Category = "Character Movement: Ladder", EditAnywhere, BlueprintReadWrite, meta = (ClampMin = "0", UIMin = "0"))
	float ClimbingOnLadderBreakingDeceleration = 2048.0f;

	UPROPERTY(Category = "Character Movement: Ladder", EditAnywhere, BlueprintReadWrite, meta = (ClampMin = "0", UIMin = "0"))
	float MaxLadderTopOffset = 90.0f;

	UPROPERTY(Category = "Character Movement: Ladder", EditAnywhere, BlueprintReadWrite, meta = (ClampMin = "0", UIMin = "0"))
	float MinLadderBottomOffset = 90.0f;

	UPROPERTY(Category = "Character Movement: Ladder", EditAnywhere, BlueprintReadWrite, meta = (ClampMin = "0", UIMin = "0"))
	float JumpOffFromLadderSpeed = 500.0f;

	class AHWBaseCharacter* GetBaseCharacterOwner() const;

private:
	bool bIsSprinting;
	bool bIsProning;
	
	FMantlingMovementParameters CurrentMantlingParameters;
	FTimerHandle MantlingTimer;
	const ALadder* CurrentLadder = nullptr;

	FRotator ForceTargetRotation = FRotator::ZeroRotator;
	bool bForceRotation = false;

};
