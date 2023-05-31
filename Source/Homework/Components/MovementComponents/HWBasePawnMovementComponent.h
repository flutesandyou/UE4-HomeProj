// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/PawnMovementComponent.h"
#include "HWBasePawnMovementComponent.generated.h"

/**
 * 
 */
UCLASS()
class HOMEWORK_API UHWBasePawnMovementComponent : public UPawnMovementComponent
{
	GENERATED_BODY()

	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;

public:
	void JumpStart();

	virtual bool IsFalling() const override;
	
protected:
	UPROPERTY(EditAnywhere)
	float MaxSpeed = 1200.0f;

	UPROPERTY(EditAnywhere)
	float InitialJumpVelocity = 500.0f;
	
	UPROPERTY(EditAnywhere)
	bool bEnableGravity;

private:
	FVector VerticalVelocity = FVector::ZeroVector;
	bool bIsFalling = false;
};
