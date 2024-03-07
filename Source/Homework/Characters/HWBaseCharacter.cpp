// Fill out your copyright notice in the Description page of Project Settings.


#include "HWBaseCharacter.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "../Components/MovementComponents/HWBaseCharacterMovementComponent.h"
#include "../Components/LedgeDetectorComponents/LedgeDetectorComponent.h"
#include "Kismet/KismetSystemLibrary.h"
#include "Curves/CurveVector.h"
#include "../Actors/Interactive/Environment/Ladder.h"

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

void AHWBaseCharacter::OnEndCrouch(float HalfHeightAdjust, float ScaledHalfHeightAdjust)
{
    if (bWantsToMantle)
    {
        StartMantle();
        bWantsToMantle = false;
    }
    Super::OnEndCrouch(HalfHeightAdjust, ScaledHalfHeightAdjust);
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

void AHWBaseCharacter::ClimbLadderUp(float Value)
{
    if (GetBaseCharacterMovementComponent()->IsOnLadder() && !FMath::IsNearlyZero(Value))
    {
        FVector LadderUpVector = GetBaseCharacterMovementComponent()->GetCurrentLadder()->GetActorUpVector();
        AddMovementInput(LadderUpVector, Value);
    }
}

void AHWBaseCharacter::InteractWithLadder()
{
    if (GetBaseCharacterMovementComponent()->IsOnLadder())
	{
        GetBaseCharacterMovementComponent()->DetachFromLadder();
	}
    else
    {
        const ALadder* AvailableLadder = GetAvailableLadder();
        if (IsValid(AvailableLadder))
        {
            if (AvailableLadder->GetIsOnTop())
            {
                PlayAnimMontage(AvailableLadder->GetAttachFromTopAnimMontage());
            }
            GetBaseCharacterMovementComponent()->AttachToLadder(AvailableLadder);
        }
    }
}

void AHWBaseCharacter::Mantle(bool bForce)
{
    if (!(CanMantle() || bForce))
    {
        return;
    }
    bWantsToMantle = true;
    StartMantle();
}

void AHWBaseCharacter::StartMantle()
{
    if (!GetBaseCharacterMovementComponent()->IsMantling() && !GetBaseCharacterMovementComponent()->IsCrouching() && bWantsToMantle == true)
    { 
        FLedgeDescription LedgeDescription;
        if (LedgeDetectorComponent->DetectLedge(LedgeDescription))
        {
            FMantlingMovementParameters MantlingParameters;
            MantlingParameters.InitialLocation = GetActorLocation() - LedgeDescription.HitComponent->GetComponentLocation();
		    MantlingParameters.InitialRotation = GetActorRotation() - LedgeDescription.HitComponent->GetComponentRotation();
            MantlingParameters.TargetLocation = LedgeDescription.Location - LedgeDescription.HitComponent->GetComponentLocation();
		    MantlingParameters.TargetRotation = LedgeDescription.Rotation - LedgeDescription.HitComponent->GetComponentRotation();
            MantlingParameters.HitComponent = LedgeDescription.HitComponent;
            FVector DownwardTraceHitResult = LedgeDescription.DownwardTraceHitResult;

            FVector CharacterBottom = GetActorLocation() - GetCapsuleComponent()->GetScaledCapsuleHalfHeight() * FVector::UpVector;

            float MantlingHeight = DownwardTraceHitResult.Z - CharacterBottom.Z;
            const FMantlingSettings& MantlingSettings = GetMantlingSettings(MantlingHeight);

            float MinRange;
            float MaxRange;
            MantlingSettings.MantlingCurve->GetTimeRange(MinRange, MaxRange);

		    MantlingParameters.Duration = MaxRange - MinRange;


            // Math way
            //float StartTime = MantlingSettings.MaxHeightStartTime + (MantlingHeight - MantlingSettings.MinHeight) / (MantlingSettings.MaxHeight - MantlingSettings.MinHeight) * (MantlingSettings.MaxHeightStartTime - MantlingSettings.MinHeightStartTime);
            MantlingParameters.MantlingCurve = MantlingSettings.MantlingCurve;

            FVector2D SourceRange(MantlingSettings.MinHeight, MantlingSettings.MaxHeight);
		    FVector2D TargetRange(MantlingSettings.MinHeightStartTime, MantlingSettings.MaxHeightStartTime);
            MantlingParameters.StartTime = FMath::GetMappedRangeValueClamped(SourceRange, TargetRange, MantlingHeight);

            MantlingParameters.InitialAnimationLocation = MantlingParameters.TargetLocation - MantlingSettings.AnimationCorrectionZ * FVector::UpVector + MantlingSettings.AnimationCorrectionXY * LedgeDescription.LedgeNormal;

            GetBaseCharacterMovementComponent()->StartMantle(MantlingParameters);

            UAnimInstance* AnimInstance = GetMesh()->GetAnimInstance();
            AnimInstance->Montage_Play(MantlingSettings.MantlingMontage, 1.0f, EMontagePlayReturnType::Duration, MantlingParameters.StartTime);

            bWantsToMantle = false;
        }
    }
}

bool AHWBaseCharacter::CanJumpInternal_Implementation() const
{
    return Super::CanJumpInternal_Implementation() && !GetBaseCharacterMovementComponent()->IsMantling();
}

void AHWBaseCharacter::BeginPlay()
{
    Super::BeginPlay();
    CurrentStamina = MaxStamina;
}

bool AHWBaseCharacter::CanMantle() const
{
    return !GetBaseCharacterMovementComponent()->IsOnLadder();
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

void AHWBaseCharacter::RegisterInteractiveActor(AInteractiveActor* InteractiveActor)
{
    AvailableInteractiveActors.AddUnique(InteractiveActor);
}

void AHWBaseCharacter::UnregisterInteractiveActor(AInteractiveActor* InteractiveActor)
{
    AvailableInteractiveActors.RemoveSingleSwap(InteractiveActor);
}

const ALadder* AHWBaseCharacter::GetAvailableLadder() const
{
    const ALadder* Result = nullptr;
    for (const AInteractiveActor* InteractiveActor : AvailableInteractiveActors)
    {
        if (InteractiveActor->IsA<ALadder>())
        {
            Result = StaticCast<const ALadder*>(InteractiveActor);
            break;
        }
    }
    return Result;
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

const FMantlingSettings& AHWBaseCharacter::GetMantlingSettings(float LedgeHeight) const
{
    return LedgeHeight > LowMantleMaxHeight ? HighMantlingSettings : LowMantlingSettings;
}
