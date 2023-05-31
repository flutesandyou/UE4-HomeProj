// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "HWBaseCharacterMovementComponent.generated.h"

/**
 * 
 */
UCLASS()
class HOMEWORK_API UHWBaseCharacterMovementComponent : public UCharacterMovementComponent
{
	GENERATED_BODY()

public:
	FORCEINLINE bool IsSprinting() { return bIsSprinting; }
	virtual float GetMaxSpeed() const override;
	virtual bool CanAttemptJump() const override;
	
	void StartSprint();
	void StopSprint();

	// stamina feature

	bool bIsOutOfStamina;
	FORCEINLINE bool IsOutOfStamina() const { return bIsOutOfStamina; }
	void SetIsOutOfStamina(bool bIsOutOfStamina_In);
	
protected:
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="Character movement: sprint", meta=(ClampMin = 0.0f, UIMin = 0.0f))
	float SprintSpeed = 1200.0f;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Character movement: sprint", meta = (ClampMin = 0.0f, UIMin = 0.0f))
	float OutOfStaminaSpeed = 150.0f;
	
private:
	bool bIsSprinting;
};
