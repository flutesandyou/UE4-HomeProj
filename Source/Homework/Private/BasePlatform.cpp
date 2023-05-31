// Fill out your copyright notice in the Description page of Project Settings.


#include "BasePlatform.h"
#include "PlatformInvocator.h"

// Sets default values
ABasePlatform::ABasePlatform()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;
	USceneComponent* DefaultPlatformRoot = CreateDefaultSubobject<USceneComponent>(TEXT("Platform root"));
	RootComponent = DefaultPlatformRoot;

	PlatformMesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("Platform"));
	PlatformMesh->SetupAttachment(DefaultPlatformRoot);
}

// Called when the game starts or when spawned
void ABasePlatform::BeginPlay()
{
	Super::BeginPlay();
	// store start position of platform
	StartLocation = PlatformMesh->GetRelativeLocation();

	// bind UObject member function delegate, add float interpolation to timeline
	if (IsValid(TimelineCurve))
	{
		FOnTimelineFloatStatic PlatformMovementTimelineUpdate;
		PlatformMovementTimelineUpdate.BindUObject(this, &ABasePlatform::PlatformTimelineUpdate);
		PlatformTimeline.AddInterpFloat(TimelineCurve, PlatformMovementTimelineUpdate);
	}

	// Initial start of platform movement

	if (PlatformBehavior == EPlatformBehavior::Loop)
	{	
		FOnTimelineEventStatic OnTimelineFinished;
		OnTimelineFinished.BindUObject(this, &ABasePlatform::OnEndPlatformTimeline);
		PlatformTimeline.SetTimelineFinishedFunc(OnTimelineFinished);
		PlatformTimeline.PlayFromStart();
	}

	if (PlatformBehavior == EPlatformBehavior::OnDemand)
	{
		// Subscribe to delegate.
		if (IsValid(PlatormInvocator))
		{
			GEngine->AddOnScreenDebugMessage(-1, 2.0f, FColor::Black, FString::Printf(TEXT("Bound")));
			PlatormInvocator->OnInvocatorActivated.AddDynamic(this, &ABasePlatform::OnPlatformInvoked);
		}
	}

}

// Called every frame
void ABasePlatform::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	// advance timeline if delegate is fired
	PlatformTimeline.TickTimeline(DeltaTime);
}

void ABasePlatform::OnEndPlatformTimeline()
{
	GEngine->AddOnScreenDebugMessage(-1, 2.0f, FColor::Black, FString::Printf(TEXT("Platform Timeline Ended")));
	MovePlatform();
}

// set relative location to platform mesh
void ABasePlatform::PlatformTimelineUpdate(const float Alpha)
{
	const FVector PlatformTargetLocation = FMath::Lerp(StartLocation, EndLocation, Alpha);
	PlatformMesh->SetRelativeLocation(PlatformTargetLocation);
}

// Method that makes platforms move when Invoked by Collider
void ABasePlatform::OnPlatformInvoked()
{
	GEngine->AddOnScreenDebugMessage(-1, 2.0f, FColor::Black, FString::Printf(TEXT("OnPlatformInvoked Activated")));
	MovePlatform();
}

// seems like FTimeline not supported by Uproperty macros and INLINE don't work with UFUNCTION
// so this one is needed to call from Blueprint
void ABasePlatform::PlatformTimelineReverse()
{
	PlatformTimeline.Reverse();
}

// platform move rules
void ABasePlatform::MovePlatform()
{
	float MaxPlaybackPosition = PlatformTimeline.GetTimelineLength();

	if (PlatformTimeline.GetPlaybackPosition() == 0)
	{
		GetWorld()->GetTimerManager().SetTimer(PlatformDelayTimer, this, &ABasePlatform::OnPlatformTimelineAtStart, PlatformDelayTime, false);
	}
	else if (PlatformTimeline.GetPlaybackPosition() == MaxPlaybackPosition)
	{
		GetWorld()->GetTimerManager().SetTimer(PlatformDelayTimer, this, &ABasePlatform::OnPlatformTimelineAtEnd, PlatformDelayTime, false);
	}
}
