// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/CharacterMovementComponent.h"
#include <../Components/LedgeDetectorComponents/LedgeDetectorComponent.h>
#include "HWBaseCharacterMovementComponent.generated.h"

UENUM(BlueprintType)
enum class ECustomMovementMode : uint8
{
	CMOVE_None = 0 UMETA(DisplayName = "None"),
	CMOVE_Mantling UMETA(DisplayName = "Mantling"),
	CMOVE_Max UMETA(Hidden)
};

class AHWBaseCharacter;

UCLASS()
class HOMEWORK_API UHWBaseCharacterMovementComponent : public UCharacterMovementComponent
{
	GENERATED_BODY()

public:
	FORCEINLINE bool IsSprinting() { return bIsSprinting; }
	FORCEINLINE bool IsProning() { return bIsProning; }
	virtual float GetMaxSpeed() const override;
	virtual bool CanAttemptJump() const override;
	
	void StartSprint();
	void StopSprint();

	// mantleing

	void StartMantle(const FLedgeDescription& LedgeDescription);
	void EndMantle();
	bool IsMantling() const;

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

	virtual void OnMovementModeChanged(EMovementMode PreviousMovementMode, uint8 PreviousCustomMode) override;

	UPROPERTY(Category = "Character Movement: Swimming", EditAnywhere, BlueprintReadWrite, meta = (ClampMin = "0", UIMin = "0"))
	float SwimmingCapsuleRadius = 60.0f;

	UPROPERTY(Category = "Character Movement: Swimming", EditAnywhere, BlueprintReadWrite, meta = (ClampMin = "0", UIMin = "0"))
	float SwimmingCapsuleHalfHeigh = 60.0f;

private:
	bool bIsSprinting;
	bool bIsProning;

	FLedgeDescription TargetLedge;
	float TargetMantlingTime = 1.0f;
	FVector InitialMantlingLocation;
	FRotator InitialMantlingRotation;
	FTimerHandle MantlingTimer;
};
