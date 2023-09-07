// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "HWBaseCharacterMovementComponent.generated.h"

/**
 * 
 */
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

	//void Prone();
	void Prone(bool bClientSimulation);
	void UnProne();
	virtual bool CanProneInCurrentState() const;

	// stamina feature

	bool bIsOutOfStamina;
	FORCEINLINE bool IsOutOfStamina() const { return bIsOutOfStamina; }
	void SetIsOutOfStamina(bool bIsOutOfStamina_In);

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Character movement: sprint", meta = (ClampMin = 0.0f, UIMin = 0.0f))
	float PressTime = 0.5f;

	AHWBaseCharacter* GetCharacterOwner() const;

	FORCEINLINE AHWBaseCharacter* UHWBaseCharacterMovementComponent::GetCharacterOwner() const
	{
		return BaseCharacterOwner;
	}
	
	UPROPERTY(Category = "Character movement: prone", EditAnywhere, BlueprintReadOnly, meta = (ClampMin = "0", UIMin = "0"))
	float PronedHalfHeight;

	UPROPERTY(Category = "Character Movement (General Settings)", VisibleInstanceOnly, BlueprintReadWrite, AdvancedDisplay)
	uint8 bProneMaintainsBaseLocation : 1;

protected:
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="Character movement: sprint", meta=(ClampMin = 0.0f, UIMin = 0.0f))
	float SprintSpeed = 1200.0f;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Character movement: sprint", meta = (ClampMin = 0.0f, UIMin = 0.0f))
	float OutOfStaminaSpeed = 150.0f;
	

	/** Character movement component belongs to */
	UPROPERTY(Transient, DuplicateTransient)
	AHWBaseCharacter* BaseCharacterOwner;

private:
	bool bIsSprinting;
	bool bIsProning;
	bool CanEverProne;
};
