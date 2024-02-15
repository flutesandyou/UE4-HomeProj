#include "LedgeDetectorComponent.h"
#include "GameFramework/Character.h"
#include "Components/CapsuleComponent.h"
#include "../HomeworkTypes.h"
#include "DrawDebugHelpers.h"
#include "../Utils/HWTraceUtils.h"
#include <../Characters/HWBaseCharacter.h>
#include <Kismet/GameplayStatics.h>
#include <HWGameInstance.h>
#include "../Subsystems/DebugSubsystem.h"

void ULedgeDetectorComponent::BeginPlay()
{
	Super::BeginPlay();
	checkf(GetOwner()->IsA<ACharacter>(), TEXT("ULedgeDetectorComponent::BeginPlay() only a character can use ULedgeDetectorComponent"));
	// ...
	CachedCharacterOwner = StaticCast<ACharacter*>(GetOwner());
}

bool ULedgeDetectorComponent::DetectLedge(OUT FLedgeDescription& LedgeDescription)
{
	UCapsuleComponent* OwnerCapsuleComponent = CachedCharacterOwner->GetCapsuleComponent();

	FCollisionQueryParams QueryParams;
	QueryParams.bTraceComplex = true;
	QueryParams.AddIgnoredActor(GetOwner());

#if ENABLE_DRAW_DEBUG
	UDebugSubsystem* DebugSubsystem = UGameplayStatics::GetGameInstance(GetWorld())->GetSubsystem<UDebugSubsystem>();
	bool bIsDebugEnabled = DebugSubsystem->IsCategoryEnabled(DebugCategoryLedgeDetection);
#else
	bool bIsDebugEnabled = false;
#endif

	float DrawTime = 2.0f;

	float BottomZOffset = 2.0f;
	FVector CharacterBottom = CachedCharacterOwner->GetActorLocation() - (OwnerCapsuleComponent->GetScaledCapsuleHalfHeight() - BottomZOffset) * FVector::UpVector;

	// 1. Forward check
	float ForwardTraceCapsuleRadius = OwnerCapsuleComponent->GetScaledCapsuleRadius();
	float ForwardTraceCapsuleHalfHeight = (MaximumLedgeHeight - MinimumLedgeHeight) * 0.5f;
	// float ForwardTraceCapsuleHalfHeight = (MaximumLedgeHeight - MinimumLedgeHeight) * OwnerCapsuleComponent->GetScaledCapsuleHalfHeight();

	FHitResult ForwardTraceHitResult;
	FVector ForwardTraceStartLocation = CharacterBottom + (MinimumLedgeHeight + ForwardTraceCapsuleHalfHeight) * FVector::UpVector;
	FVector ForwardTraceEndLocation = ForwardTraceStartLocation + CachedCharacterOwner->GetActorForwardVector() * ForwardCheckDistance;


	if (!HWTraceUtils::SweepCapsuleSingleByChannel(GetWorld(), ForwardTraceHitResult, ForwardTraceStartLocation, ForwardTraceEndLocation, ForwardTraceCapsuleRadius, ForwardTraceCapsuleHalfHeight, FQuat::Identity, ECC_Climbing, QueryParams, FCollisionResponseParams::DefaultResponseParam, bIsDebugEnabled, DrawTime))
	{
		return false;
	}

	// 2. Downward check
	FHitResult DownwardTraceHitResult;
	float DownwardTraceSphereRadius = OwnerCapsuleComponent->GetScaledCapsuleRadius();
	float DownwardTraceDepthOffset = 10.0f;
	FVector DownwardTraceStartLocation = ForwardTraceHitResult.ImpactPoint - ForwardTraceHitResult.ImpactNormal * DownwardTraceDepthOffset;
	DownwardTraceStartLocation.Z = CharacterBottom.Z + MaximumLedgeHeight + DownwardTraceSphereRadius;
	FVector DownwardTraceEndLocation(DownwardTraceStartLocation.X, DownwardTraceStartLocation.Y, CharacterBottom.Z);

	if (!HWTraceUtils::SweepSphereSingleByChannel(GetWorld(), DownwardTraceHitResult, DownwardTraceStartLocation, DownwardTraceEndLocation, DownwardTraceSphereRadius, ECC_Climbing, QueryParams, FCollisionResponseParams::DefaultResponseParam, bIsDebugEnabled, DrawTime))
	{
		return false;
	}

	// 3. Overlap check
	float OverlapCapsuleRadius = OwnerCapsuleComponent->GetScaledCapsuleRadius();
	float OverlapCapsuleHalfHeight = OwnerCapsuleComponent->GetScaledCapsuleHalfHeight();
	float OverlapCapsuleFloorOffset = 2.0f;
	FVector OverlapLocation = DownwardTraceHitResult.ImpactPoint + (OverlapCapsuleHalfHeight + OverlapCapsuleFloorOffset) * FVector::UpVector;

	if (HWTraceUtils::OverlapCapsuleAnyByProfile(GetWorld(), OverlapLocation, OverlapCapsuleRadius, OverlapCapsuleHalfHeight, FQuat::Identity, CollisionProfilePawn, QueryParams, bIsDebugEnabled, DrawTime))
	{
		return false;
	}
	LedgeDescription.Location = OverlapLocation;
	LedgeDescription.Rotation = (ForwardTraceHitResult.ImpactNormal * FVector(-1.0f, -1.0f, 0.0f)).ToOrientationRotator();
	LedgeDescription.LedgeNormal = ForwardTraceHitResult.ImpactNormal;

	return true;
}