#include "LedgeDetectorComponent.h"
#include "GameFramework/Character.h"
#include "Components/CapsuleComponent.h"
#include "../HomeworkTypes.h"

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

	float BottomZOffset = 2.0f;
	FVector CharacterBottom = CachedCharacterOwner->GetActorLocation() - (OwnerCapsuleComponent->GetScaledCapsuleHalfHeight() - BottomZOffset) * FVector::UpVector;

	// 1. Forward check
	float ForwardTraceCapsuleRadius = OwnerCapsuleComponent->GetScaledCapsuleRadius();
	float ForwardTraceCapsuleHalfHeight = (MaximumLedgeHeight - MinimumLedgeHeight) * 0.5f;
	// float ForwardTraceCapsuleHalfHeight = (MaximumLedgeHeight - MinimumLedgeHeight) * OwnerCapsuleComponent->GetScaledCapsuleHalfHeight();

	FHitResult ForwardTraceHitResult;
	FCollisionShape ForwardTraceCapsuleShape = FCollisionShape::MakeCapsule(ForwardTraceCapsuleRadius, ForwardTraceCapsuleHalfHeight);
	FVector ForwardTraceStartLocation = CharacterBottom + (MinimumLedgeHeight + ForwardTraceCapsuleHalfHeight) * FVector::UpVector;
	FVector ForwardTraceEndLocation = ForwardTraceStartLocation + CachedCharacterOwner->GetActorForwardVector() * ForwardCheckDistance;

	if (!GetWorld()->SweepSingleByChannel(ForwardTraceHitResult, ForwardTraceStartLocation, ForwardTraceEndLocation, FQuat::Identity, ECC_Climbing, ForwardTraceCapsuleShape, QueryParams))
	{
		return false;
	}

	// 2. Downward check
	FHitResult DownwardTraceHitResult;
	float DownwardTraceSphereRadius = OwnerCapsuleComponent->GetScaledCapsuleRadius();
	FCollisionShape DownwardTraceSphereShape = FCollisionShape::MakeSphere(DownwardTraceSphereRadius);
	float DownwardTraceDepthOffset = 10.0f;
	FVector DownwardTraceStartLocation = ForwardTraceHitResult.ImpactPoint - ForwardTraceHitResult.ImpactNormal * DownwardTraceDepthOffset;
	DownwardTraceStartLocation.Z = CharacterBottom.Z + MaximumLedgeHeight + DownwardTraceSphereRadius;
	FVector DownwardTraceEndLocation(DownwardTraceStartLocation.X, DownwardTraceStartLocation.Y, CharacterBottom.Z);

	if (!GetWorld()->SweepSingleByChannel(DownwardTraceHitResult, DownwardTraceStartLocation, DownwardTraceEndLocation, FQuat::Identity, ECC_Climbing, DownwardTraceSphereShape, QueryParams))
	{
		return false;
	}

	// 3. Overlap check
	float OverlapCapsuleRadius = OwnerCapsuleComponent->GetScaledCapsuleRadius();
	float OverlapCapsuleHalfHeight = OwnerCapsuleComponent->GetScaledCapsuleHalfHeight();
	FCollisionShape OverlapCapsuleShape = FCollisionShape::MakeCapsule(OverlapCapsuleRadius, OverlapCapsuleHalfHeight);
	FVector OverlapLocation = DownwardTraceHitResult.ImpactPoint + OverlapCapsuleHalfHeight * FVector::UpVector;
	if (GetWorld()->OverlapAnyTestByProfile(OverlapLocation, FQuat::Identity, FName("Pawn"), OverlapCapsuleShape, QueryParams))
	{
		return false;
	}

	LedgeDescription.Location = DownwardTraceHitResult.ImpactPoint;
	LedgeDescription.Rotation = (ForwardTraceHitResult.ImpactNormal * FVector(-1.0f, -1.0f, 0.0f)).ToOrientationRotator();

	return true;
}