// Fill out your copyright notice in the Description page of Project Settings.


#include "PlatformInvocator.h"

// Sets default values
APlatformInvocator::APlatformInvocator()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = false;
}

// Broadcasts to subscribers
void APlatformInvocator::Invoke()
{
	
	if (OnInvocatorActivated.IsBound())
	{
		OnInvocatorActivated.Broadcast();
		GEngine->AddOnScreenDebugMessage(-1, 2.0f, FColor::Black, FString::Printf(TEXT("OnInvocatorActivated Broadcasted")));
	}
}

