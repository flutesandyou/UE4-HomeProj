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
	InputComponent->BindAxis("SwimForward", this, &AHWPlayerController::SwimForward);
	InputComponent->BindAxis("SwimRight", this, &AHWPlayerController::SwimRight);
	InputComponent->BindAxis("SwimUp", this, &AHWPlayerController::SwimUp);
	InputComponent->BindAxis("ClimbLadderUp", this, &AHWPlayerController::ClimbLadderUp);
	InputComponent->BindAction("InteractWithLadder", EInputEvent::IE_Released, this, &AHWPlayerController::InteractWithLadder);
	InputComponent->BindAction("Mantle", EInputEvent::IE_Pressed, this, &AHWPlayerController::Mantle);
	InputComponent->BindAction("Jump", EInputEvent::IE_Pressed, this, &AHWPlayerController::Jump);
	InputComponent->BindAction("Crouch", EInputEvent::IE_Pressed, this, &AHWPlayerController::ChangeCrouchState);
    InputComponent->BindAction("Sprint", EInputEvent::IE_Pressed, this, &AHWPlayerController::StartSprint);
	InputComponent->BindAction("Sprint", EInputEvent::IE_Released, this, &AHWPlayerController::StopSprint);
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

void AHWPlayerController::Mantle()
{
	if (CachedBaseCharacter.IsValid())
	{
		CachedBaseCharacter->Mantle();
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

void AHWPlayerController::SwimForward(float Value)
{
	if (CachedBaseCharacter.IsValid())
	{
		CachedBaseCharacter->SwimForward(Value);
	}
}

void AHWPlayerController::SwimRight(float Value)
{
	if (CachedBaseCharacter.IsValid())
	{
		CachedBaseCharacter->SwimRight(Value);
	}
}

void AHWPlayerController::SwimUp(float Value)
{
	if (CachedBaseCharacter.IsValid())
	{
		CachedBaseCharacter->SwimUp(Value);
	}
}

void AHWPlayerController::ClimbLadderUp(float Value)
{
	if (CachedBaseCharacter.IsValid())
	{
		CachedBaseCharacter->ClimbLadderUp(Value);
	}
}

void AHWPlayerController::InteractWithLadder()
{
	if (CachedBaseCharacter.IsValid())
	{
		CachedBaseCharacter->InteractWithLadder();
	}
}
