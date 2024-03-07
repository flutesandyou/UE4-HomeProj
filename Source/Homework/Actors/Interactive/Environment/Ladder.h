// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "../Actors/Interactive/InteractiveActor.h"
#include "Ladder.generated.h"

class UAnimMontage;
class UStaticMeshComponent;
class UBoxComponent;

UCLASS(Blueprintable)
class HOMEWORK_API ALadder : public AInteractiveActor
{
	GENERATED_BODY()
	
public:
	ALadder();

	virtual void BeginPlay() override;
	virtual void OnConstruction(const FTransform& Transform) override;

	float GetLadderHeight() const { return LadderHeight; }

	bool GetIsOnTop() const { return bIsOnTop; }

	UAnimMontage* GetAttachFromTopAnimMontage() const;

	FVector GetAttachFromTopAnimMontageStartingLocation() const;

protected:
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Ladder parameters")
	float LadderHeight = 100.0f;
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Ladder parameters")
	float LadderWidth = 50.0f;
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Ladder parameters")
	float StepsInterval = 25.0f;
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Ladder parameters")
	float BottomStepOffset = 25.0f;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
	class UStaticMeshComponent* RightRailMeshComponent;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
	class UStaticMeshComponent* LeftRailMeshComponent;
	
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
	class UInstancedStaticMeshComponent* StepsMeshComponent;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
	UBoxComponent* TopInteractionVolume;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Ladder parameters")
	UAnimMontage* AttachFromTopAnimMontage;

	//Offset from top of a ladder for starting animation montage
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Ladder parameters")
	FVector AnimMontageInitialOffset = FVector::ZeroVector;
	UBoxComponent* GetLadderInteractionBox() const;

	virtual void OnInteractionVolumeBeginOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult) override;
	virtual void OnInteractionVolumeEndOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex) override;

private:
	bool bIsOnTop = false;

};
