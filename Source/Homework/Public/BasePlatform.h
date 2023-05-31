// Fill out your copyright notice in the Description page of Project Settings.
// review update test
#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "Components/TimelineComponent.h"
#include "BasePlatform.generated.h"

UENUM()
enum class EPlatformBehavior : uint8
{
	OnDemand = 0,
	Loop
};

UCLASS()
class HOMEWORK_API ABasePlatform : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	ABasePlatform();

	// property of mesh of platform
	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	UStaticMeshComponent* PlatformMesh;

	// declare start and end vector to use in blueprints
	UPROPERTY(EditInstanceOnly, BlueprintReadOnly, meta = (MakeEditWidget))
	FVector EndLocation;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Transient)
	FVector StartLocation;

	// declare pointer of UCurveFloat type
	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	UCurveFloat* TimelineCurve;

	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	EPlatformBehavior PlatformBehavior = EPlatformBehavior::OnDemand;

	UPROPERTY(EditInstanceOnly, BlueprintReadOnly)
	class APlatformInvocator* PlatormInvocator;

	UPROPERTY(EditInstanceOnly, BlueprintReadOnly, Category = "BasePlatformSettings", meta = (UIMin = 0.0f, ClampMin = 0.0f))
	float PlatformDelayTime = 0.1f;

private:
	// declare var of FTimeline type
	FTimeline PlatformTimeline;
	FTimerHandle PlatformDelayTimer;

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;
	// function to move platform when using timeline methods: play(), reverse()
	UFUNCTION()
	void PlatformTimelineUpdate(float Alpha);
	
	UFUNCTION(BlueprintCallable, Category="BasePlatformFunctions")
	void PlatformTimelineReverse();

	UFUNCTION(BlueprintCallable, Category = "BasePlatformFunctions")
	void OnPlatformInvoked();

	UFUNCTION()
	void OnEndPlatformTimeline();

private:
	FORCEINLINE void OnPlatformTimelineAtStart() { PlatformTimeline.PlayFromStart();}
	FORCEINLINE void OnPlatformTimelineAtEnd() { PlatformTimeline.ReverseFromEnd();}
	void MovePlatform();
};

