// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "HWBaseCharacter.h"
#include "Components/TimelineComponent.h"
#include "../Components/MovementComponents/HWBaseCharacterMovementComponent.h"
#include "PlayerCharacter.generated.h"

/**
 * 
 */
UCLASS(Blueprintable)
class HOMEWORK_API APlayerCharacter : public AHWBaseCharacter
{
	GENERATED_BODY()

public:
	APlayerCharacter(const FObjectInitializer& ObjectInitializer);
	
	virtual void MoveForward(float Value) override;
	virtual void MoveRight(float Value) override;
	virtual void Turn(float Value) override;
	virtual void LookUp(float Value) override;
	virtual void TurnAtRate(float Value) override;
	virtual void LookUpAtRate(float Value) override;
	virtual void SwimForward(float Value) override;
	virtual void SwimRight(float Value) override;
	virtual void SwimUp(float Value) override;
	
    virtual void OnStartCrouch(float HalfHeightAdjust, float ScaledHalfHeightAdjust) override;
    virtual void OnEndCrouch(float HalfHeightAdjust, float ScaledHalfHeightAdjust) override;
    
    virtual bool CanJumpInternal_Implementation() const override;
    virtual void OnJumped_Implementation() override;

	virtual void Tick(float DeltaSeconds) override;
	virtual void BeginPlay() override;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Character | Camera")
	UCurveFloat* TimelineCurve;

	UFUNCTION()
	void SpringArmTimelineUpdate(float Alpha);
    
protected:
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Character | Camera")
	class UCameraComponent* CameraComponent;
	
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Character | Camera")
	class USpringArmComponent* SpringArmComponent;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Character | Camera")
	float DefaultArmLength = 300.0f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Character | Camera")
	float SprintArmLength = 400.0f;

private:
	virtual void OnSprintStart_Implementation() override;
	virtual void OnSprintEnd_Implementation() override;

	FTimeline SpringArmTimeline;
	FTimerHandle SpringArmTimelineHandle;
};
