// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Animation/AnimInstance.h"
#include "Homework/Characters/HWBaseCharacter.h"
#include "HWBaseCharacterAnimInstance.generated.h"

/**
 * 
 */
UCLASS()
class HOMEWORK_API UHWBaseCharacterAnimInstance : public UAnimInstance
{
	GENERATED_BODY()

public:
	virtual void NativeBeginPlay() override;
	virtual void NativeUpdateAnimation(float DeltaSeconds) override;
	
protected:
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Character animation")
	float Speed = 0.0f;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Character animation")
	bool bIsFalling = false;
    
    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Character animation")
    bool bIsCrouching = false;

    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Character animation")
    bool bIsProning = false;
    
    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Character animation")
    bool bIsSprinting = false;
    
    //Stamina
    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Character animation")
    bool bIsOutOfStamina = false;

    //IK
    UPROPERTY(VisibleAnywhere, Transient, BlueprintReadOnly, Category = "IK Settings")
    FVector RightFootEffectorLocation = FVector::ZeroVector;

    UPROPERTY(VisibleAnywhere, Transient, BlueprintReadOnly, Category = "IK Settings")
    FVector LeftFootEffectorLocation = FVector::ZeroVector;

    UPROPERTY(VisibleAnywhere, Transient, BlueprintReadOnly, Category = "IK Settings")
    float HipDisplacement = 0.0f;

private:
	TWeakObjectPtr<class AHWBaseCharacter> CachedBaseCharacter;
};
