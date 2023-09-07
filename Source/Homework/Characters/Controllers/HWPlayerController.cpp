// Fill out your copyright notice in the Description page of Project Settings.


#include "HWPlayerController.h"
#include "Homework/Components/MovementComponents/HWBaseCharacterMovementComponent.h" // Include the correct header file
#include "../HWBaseCharacter.h"

void AHWPlayerController::SetPawn(APawn* InPawn)
{
	Super::SetPawn(InPawn);
	CachedBaseCharacter = Cast<AHWBaseCharacter>(InPawn);
}

void AHWPlayerController::SetupInputComponent()
{
	Super::SetupInputComponent();
	InputComponent->BindAxis("MoveForward", this, &AHWPlayerController::MoveForward);
	InputComponent->BindAxis("MoveRight", this, &AHWPlayerController::MoveRight);
	InputComponent->BindAxis("Turn", this, &AHWPlayerController::Turn);
	InputComponent->BindAxis("LookUp", this, &AHWPlayerController::LookUp);
	InputComponent->BindAxis("TurnAtRate", this, &AHWPlayerController::TurnAtRate);
	InputComponent->BindAxis("LookUpAtRate", this, &AHWPlayerController::LookUpAtRate);
	InputComponent->BindAction("Jump", EInputEvent::IE_Pressed, this, &AHWPlayerController::Jump);
	InputComponent->BindAction("Crouch", EInputEvent::IE_Pressed, this, &AHWPlayerController::ChangeCrouchState);
    InputComponent->BindAction("Sprint", EInputEvent::IE_Pressed, this, &AHWPlayerController::StartSprint);
    InputComponent->BindAction("Sprint", EInputEvent::IE_Released, this, &AHWPlayerController::StopSprint);
	InputComponent->BindAction("Prone", EInputEvent::IE_Pressed, this, &AHWPlayerController::StartPress);
	InputComponent->BindAction("Prone", EInputEvent::IE_Released, this, &AHWPlayerController::StopPress);
}

void AHWPlayerController::MoveForward(float Value)
{
	if (CachedBaseCharacter.IsValid())
	{
		CachedBaseCharacter->MoveForward(Value);
	}
}

void AHWPlayerController::MoveRight(float Value)
{
	if (CachedBaseCharacter.IsValid())
	{
		CachedBaseCharacter->MoveRight(Value);
	}
}

void AHWPlayerController::Turn(float Value)
{
	if (CachedBaseCharacter.IsValid())
	{
		CachedBaseCharacter->Turn(Value);
	}
}

void AHWPlayerController::LookUp(float Value)
{
	if (CachedBaseCharacter.IsValid())
	{
		CachedBaseCharacter->LookUp(Value);
	}
}

void AHWPlayerController::TurnAtRate(float Value)
{
	if (CachedBaseCharacter.IsValid())
	{
		CachedBaseCharacter->TurnAtRate(Value);
	}
}

void AHWPlayerController::LookUpAtRate(float Value)
{
	if (CachedBaseCharacter.IsValid())
	{
		CachedBaseCharacter->LookUpAtRate(Value);
	}
}

void AHWPlayerController::Jump()
{
	if (CachedBaseCharacter.IsValid())
	{
		CachedBaseCharacter->Jump();
	}
}

void AHWPlayerController::ChangeCrouchState()
{
	if (CachedBaseCharacter.IsValid())
	{
		CachedBaseCharacter->ChangeCrouchState();
	}	
}

void AHWPlayerController::ChangeProneState()
{
	if (CachedBaseCharacter.IsValid())
	{
		CachedBaseCharacter->ChangeProneState();
	}
}

void AHWPlayerController::StartSprint()
{
    if (CachedBaseCharacter.IsValid())
    {
        CachedBaseCharacter->StartSprint();
    }
}

void AHWPlayerController::StopSprint()
{
    if (CachedBaseCharacter.IsValid())
    {
        CachedBaseCharacter->StopSprint();
    }
}

void AHWPlayerController::StartPress()
{
	if (CachedBaseCharacter.IsValid()) 
	{
		GetWorldTimerManager().SetTimer(LongPressTimerHandle, this, &AHWPlayerController::HandleLongPress, CachedBaseCharacter->GetBaseCharacterMovementComponent()->PressTime, false);
	}
}

void AHWPlayerController::StopPress()
{
	if (CachedBaseCharacter.IsValid()) 
	{
		GetWorldTimerManager().ClearTimer(LongPressTimerHandle);
	}

}

void AHWPlayerController::HandleLongPress()
{
	ChangeProneState();
}
