// Fill out your copyright notice in the Description page of Project Settings.


#include "HWBaseCharacter.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "../Components/MovementComponents/HWBaseCharacterMovementComponent.h"
#include "../Components/LedgeDetectorComponents/LedgeDetectorComponent.h"
#include "Kismet/KismetSystemLibrary.h"

AHWBaseCharacter::AHWBaseCharacter(const FObjectInitializer& ObjectInitializer)
    :Super(ObjectInitializer.SetDefaultSubobjectClass<UHWBaseCharacterMovementComponent>(ACharacter::CharacterMovementComponentName))
{
    HWBaseCharacterMovementComponent = StaticCast<UHWBaseCharacterMovementComponent*>(GetCharacterMovement());
    
    // IK

    IKScale = GetActorScale3D().Z;
    IKTraceDistance = CollisionCapsuleHalfHeight * IKScale;

    CurrentStamina = MaxStamina;

    LedgeDetectorComponent = CreateDefaultSubobject<ULedgeDetectorComponent>(TEXT("LedgeDetector"));
}

void AHWBaseCharacter::ChangeCrouchState()
{
	if (GetCharacterMovement()->IsCrouching())
	{
        GEngine->AddOnScreenDebugMessage(-1, 2.0f, FColor::Black, FString::Printf(TEXT("Crouch False")));
		UnCrouch();
	}
    else if (HWBaseCharacterMovementComponent->IsProning())
    {
        ChangeProneState();
        Crouch();
        GEngine->AddOnScreenDebugMessage(-1, 2.0f, FColor::Black, FString::Printf(TEXT("Crouch True")));
    }
	else
	{
        GEngine->AddOnScreenDebugMessage(-1, 2.0f, FColor::Black, FString::Printf(TEXT("Crouch True")));
		Crouch();
	}
}

void AHWBaseCharacter::ChangeProneState()
{
    if (HWBaseCharacterMovementComponent->IsProning())
    {
        HWBaseCharacterMovementComponent->UnProne();
    }
    else
    {
        HWBaseCharacterMovementComponent->Prone();
        ChangeCrouchState();
    }
}

void AHWBaseCharacter::StartSprint()
{
    bIsSprintRequested = true;
    if (bIsCrouched)
    {
        UnCrouch();
    }
}

void AHWBaseCharacter::StopSprint()
{
    bIsSprintRequested = false;
}

void AHWBaseCharacter::Tick(float DeltaTime)
{
    Super::Tick(DeltaTime);
    TryChangeSprintState(DeltaTime);
    TryChangeStaminaState(DeltaTime);
    
    //IK
    IKRightFootOffset = FMath::FInterpTo(IKRightFootOffset, GetIKOffsetForASocket(LeftFootSocketName), DeltaTime, IKInterpSpeed);
    IKLeftFootOffset = FMath::FInterpTo(IKLeftFootOffset, GetIKOffsetForASocket(RightFootSocketName), DeltaTime, IKInterpSpeed);

    HipOffset = abs(IKRightFootOffset - IKLeftFootOffset) * -1.0f;

    //GEngine->AddOnScreenDebugMessage(-1, 2.0f, FColor::Black, FString::Printf(TEXT("%f"), IKTraceDistance));
    
    CurrentStamina += StaminaRestoreVelocity * DeltaTime;
    CurrentStamina = FMath::Clamp(CurrentStamina, 0.0f, MaxStamina);



    // Debug of stamina values

    if (HWBaseCharacterMovementComponent->bIsOutOfStamina == false && CurrentStamina != MaxStamina)
    {
        GEngine->AddOnScreenDebugMessage(1, 1.0f, FColor::Yellow, FString::Printf(TEXT("Stamina: %.2f"), CurrentStamina));
    }
    else if (HWBaseCharacterMovementComponent->bIsOutOfStamina == true)
    {
        GEngine->AddOnScreenDebugMessage(1, 1.0f, FColor::Red, FString::Printf(TEXT("Stamina: %.2f"), CurrentStamina));
    }
}

void AHWBaseCharacter::Mantle()
{
    FLedgeDescription LedgeDescription;
    if (LedgeDetectorComponent->DetectLedge(LedgeDescription))
    {
        // TODO activate mantling
    }
}

void AHWBaseCharacter::BeginPlay()
{
    Super::BeginPlay();
    CurrentStamina = MaxStamina;
}

void AHWBaseCharacter::OnSprintStart_Implementation()
{
    UE_LOG(LogTemp, Log, TEXT("AHWBaseCharacter::OnSprintStart_Implementation()"))
}

void AHWBaseCharacter::OnSprintEnd_Implementation()
{
    UE_LOG(LogTemp, Warning, TEXT("AHWBaseCharacter::OnSprintEnd_Implementation()"))
}

bool AHWBaseCharacter::CanSprint()
{
    return true;
}

void AHWBaseCharacter::OnStartProne(float HeightAdjust, float ScaledHeightAdjust)
{
    RecalculateBaseEyeHeight();

    const ACharacter* DefaultChar = GetDefault<ACharacter>(GetClass());
    if (GetMesh() && DefaultChar->GetMesh())
    {
        FVector& MeshRelativeLocation = GetMesh()->GetRelativeLocation_DirectMutable();
        MeshRelativeLocation.Z = DefaultChar->GetMesh()->GetRelativeLocation().Z + HeightAdjust;
        BaseTranslationOffset.Z = MeshRelativeLocation.Z;
    }
    else
    {
        BaseTranslationOffset.Z = DefaultChar->GetBaseTranslationOffset().Z + HeightAdjust;
    }
}

void AHWBaseCharacter::TryChangeSprintState(float DeltaTime)
{
    if (bIsSprintRequested && !HWBaseCharacterMovementComponent->IsSprinting() && CanSprint() && !HWBaseCharacterMovementComponent->IsFalling())
    {
        HWBaseCharacterMovementComponent->StartSprint();
        OnSprintStart();
    }
    if (!bIsSprintRequested && HWBaseCharacterMovementComponent->IsSprinting())
    {
        HWBaseCharacterMovementComponent->StopSprint();
        OnSprintEnd();
    }

    if (HWBaseCharacterMovementComponent->IsSprinting())
    {
        CurrentStamina -= SprintStaminaConsumptionVelocity * DeltaTime;
        CurrentStamina = FMath::Clamp(CurrentStamina, 0.0f, MaxStamina);
    }
}

void AHWBaseCharacter::TryChangeStaminaState(float DeltaTime)
{
    if (CurrentStamina <= 0.5f)
    {
        HWBaseCharacterMovementComponent->SetIsOutOfStamina(true);
        // for spring arm to reverse its timeline
        OnSprintEnd();
    }
    else if (CurrentStamina == MaxStamina)
    {
        HWBaseCharacterMovementComponent->SetIsOutOfStamina(false);
    }
}

// IK

float AHWBaseCharacter::GetIKOffsetForASocket(const FName& SocketName)
{
    float Result = 0.0f;
    FVector SocketLocation = FindComponentByClass<USkeletalMeshComponent>()->GetSocketLocation(SocketName);
    FVector TraceStart(SocketLocation.X, SocketLocation.Y, GetActorLocation().Z);
    FVector TraceEnd = TraceStart - IKTraceDistance * FVector::UpVector;

    FHitResult HitResult;
    ETraceTypeQuery TraceType = UEngineTypes::ConvertToTraceType(ECC_Visibility);
    if(UKismetSystemLibrary::LineTraceSingle(GetWorld(), TraceStart, TraceEnd, TraceType, true, TArray<AActor*>(), EDrawDebugTrace::ForOneFrame, HitResult, true))
    {
        Result = (TraceEnd.Z - HitResult.Location.Z) / IKScale;
    }

    else if(UKismetSystemLibrary::LineTraceSingle(GetWorld(), TraceEnd, TraceEnd - IKTraceExtendDistance * FVector::UpVector, TraceType, true, TArray<AActor*>(), EDrawDebugTrace::ForOneFrame, HitResult, true))
    {
        Result = (TraceEnd.Z - HitResult.Location.Z) / IKScale;
    }
    
    return Result;
}
