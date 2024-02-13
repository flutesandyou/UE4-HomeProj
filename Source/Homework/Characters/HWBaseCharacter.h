// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "Components/CapsuleComponent.h"
#include "HWBaseCharacter.generated.h"

class UHWBaseCharacterMovementComponent;

UCLASS(Abstract, NotBlueprintable)
class HOMEWORK_API AHWBaseCharacter : public ACharacter
{
	GENERATED_BODY()

public:
	AHWBaseCharacter(const FObjectInitializer& ObjectInitializer);

	virtual void MoveForward(float Value) {};
	virtual void MoveRight(float Value) {};
	virtual void Turn(float Value) {};
	virtual void LookUp(float Value) {};
	virtual void TurnAtRate(float Value) {};
	virtual void LookUpAtRate(float Value) {};
	virtual void ChangeCrouchState();
	virtual void ChangeProneState();

	virtual void StartSprint();
	virtual void StopSprint();

	virtual void SwimForward(float Value) {};

	virtual void SwimRight(float Value) {};

	virtual void SwimUp(float Value) {};

	virtual void Tick(float DeltaTime) override;

	virtual void Mantle();
	virtual void BeginPlay() override;


	FORCEINLINE UHWBaseCharacterMovementComponent* GetBaseCharacterMovementComponent() { return HWBaseCharacterMovementComponent; }

	//IK
	UFUNCTION(BlueprintCallable, BlueprintPure)
	FORCEINLINE float GetIKRightFootOffset() const { return IKRightFootOffset; }
	UFUNCTION(BlueprintCallable, BlueprintPure)
	FORCEINLINE float GetIKLeftFootOffset() const { return IKLeftFootOffset; }

	UFUNCTION(BlueprintCallable, BlueprintPure)
	FORCEINLINE float GetHipOffset() const { return HipOffset; }

	//  UFUNCTION(BlueprintNativeEvent, Category = "Character | Movement")
	void OnStartProne(float HeightAdjust, float ScaledHeightAdjust);


protected:

	UFUNCTION(BlueprintNativeEvent, Category = "Character | Movement")
	void OnSprintStart();
	virtual void OnSprintStart_Implementation();

	UFUNCTION(BlueprintNativeEvent, Category = "Character | Movement")
	void OnSprintEnd();
	virtual void OnSprintEnd_Implementation();

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Character | Controls")
	float BaseTurnRate = 45.0f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Character | Controls")
	float BaseLookUpRate = 45.0f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Character | Movement")
	float SprintSpeed = 800.0f;

	virtual bool CanSprint();


	UHWBaseCharacterMovementComponent* HWBaseCharacterMovementComponent;


private:
	bool bIsSprintRequested = false;
	void TryChangeSprintState(float DeltaTime);
	void TryChangeStaminaState(float DeltaTime);

	//IK
protected:
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Character")
	class USkeletalMeshComponent* SkeletalMeshComponent;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Character |IK settings", meta = (ClampMin = 0.0f, UIMin = 0.0f))
	float IKTraceExtendDistance = 30.0f;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Character |IK settings")
	float CollisionCapsuleHalfHeight = 90.0f;

	UPROPERTY(EditInstanceOnly, BlueprintReadOnly, Category = "Character |IK settings", meta = (ClampMin = 0.0f, UIMin = 0.0f))
	float IKInterpSpeed = 20.0f;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Character |IK settings")
	FName RightFootSocketName;
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Character |IK settings")
	FName LeftFootSocketName;

	// Stamina

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Character | Movement", meta = (ClampMin = 0.0f, UIMin = 0.0f))
	float MaxStamina;
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Character | Movement", meta = (ClampMin = 0.0f, UIMin = 0.0f))
	float StaminaRestoreVelocity;
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Character | Movement", meta = (ClampMin = 0.0f, UIMin = 0.0f))
	float SprintStaminaConsumptionVelocity;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Character | Movement")
	class ULedgeDetectorComponent* LedgeDetectorComponent;

private:

	float IKTraceDistance = 0.0f;
	float IKScale = 0.0f;

	float IKRightFootOffset = 0.0f;
	float IKLeftFootOffset = 0.0f;

	float HipOffset = 0.0f;

	float GetIKOffsetForASocket(const FName& SocketName);

	float CurrentStamina;
};


