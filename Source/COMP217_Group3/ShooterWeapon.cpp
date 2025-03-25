// Copyright Epic Games, Inc. All Rights Reserved.

//#include "ShooterGame.h"
#include "ShooterWeapon.h"
#include "FPSCharacter.h"
#include "Particles/ParticleSystemComponent.h"
#include "ShooterPlayerController.h"
//#include "Online/ShooterPlayerState.h"
#include "FPSHUD.h"
#include "Kismet/GameplayStatics.h"
//#include "MatineeCameraShake.h"

AShooterWeapon::AShooterWeapon(const FObjectInitializer& ObjectInitializer) : Super(ObjectInitializer)
{
	Mesh1P = ObjectInitializer.CreateDefaultSubobject<USkeletalMeshComponent>(this, TEXT("WeaponMesh1P"));
	Mesh1P->VisibilityBasedAnimTickOption = EVisibilityBasedAnimTickOption::OnlyTickPoseWhenRendered;
	Mesh1P->bReceivesDecals = false;
	Mesh1P->CastShadow = false;
	Mesh1P->SetCollisionObjectType(ECC_WorldDynamic);
	Mesh1P->SetCollisionEnabled(ECollisionEnabled::NoCollision);
	Mesh1P->SetCollisionResponseToAllChannels(ECR_Ignore);
	RootComponent = Mesh1P;

	bLoopedMuzzleFX = false;
	bLoopedFireAnim = false;
	bPlayingFireAnim = false;
	bIsEquipped = false;
	bWantsToFire = false;
	bPendingReload = false;
	bPendingEquip = false;
	CurrentState = EWeaponState::Idle;

	CurrentAmmo = 0;
	CurrentAmmoInClip = 0;
	BurstCounter = 0;
	LastFireTime = 0.0f;

	PrimaryActorTick.bCanEverTick = true;
	PrimaryActorTick.TickGroup = TG_PrePhysics;
	SetRemoteRoleForBackwardsCompat(ROLE_SimulatedProxy);
	bReplicates = true;
	bNetUseOwnerRelevancy = true;
}

void AShooterWeapon::PostInitializeComponents()
{
	Super::PostInitializeComponents();

	if (WeaponConfig.InitialClips > 0)
	{
		CurrentAmmoInClip = WeaponConfig.AmmoPerClip;
		CurrentAmmo = WeaponConfig.AmmoPerClip * WeaponConfig.InitialClips;
	}

	DetachMeshFromPawn();
}

void AShooterWeapon::Destroyed()
{
	Super::Destroyed();
}

//////////////////////////////////////////////////////////////////////////
// Inventory

void AShooterWeapon::OnEquip(const AShooterWeapon* LastWeapon)
{
	AttachMeshToPawn();

	bPendingEquip = true;

	// Only play animation if last weapon is valid
	//if (LastWeapon)
	//{
	//	float Duration = PlayWeaponAnimation(EquipAnim);
	//	if (Duration <= 0.0f)
	//	{
	//		// failsafe
	//		Duration = 0.5f;
	//	}
	//	EquipStartedTime = GetWorld()->GetTimeSeconds();
	//	EquipDuration = Duration;

	//	GetWorldTimerManager().SetTimer(TimerHandle_OnEquipFinished, this, &AShooterWeapon::OnEquipFinished, Duration, false);
	//}
	//else
	//{
	//	OnEquipFinished();
	//}

}

void AShooterWeapon::OnEquipFinished()
{
	AttachMeshToPawn();

	bIsEquipped = true;
	bPendingEquip = false;
}


void AShooterWeapon::AttachMeshToPawn()
{
	if (MyPawn)
	{
		// Remove and hide meshes
		DetachMeshFromPawn();

		FName AttachPoint = MyPawn->GetWeaponAttachPoint();
		USkeletalMeshComponent* PawnMesh1p = MyPawn->GetSpecifcPawnMesh();
		Mesh1P->SetHiddenInGame(false);
		Mesh1P->AttachToComponent(PawnMesh1p, FAttachmentTransformRules::KeepRelativeTransform, AttachPoint);
	}
}

void AShooterWeapon::DetachMeshFromPawn()
{
	Mesh1P->DetachFromComponent(FDetachmentTransformRules::KeepRelativeTransform);
	Mesh1P->SetHiddenInGame(true);
}


void AShooterWeapon::SetOwningPawn(AFPSCharacter* NewOwner)
{
	if (MyPawn != NewOwner)
	{
		SetInstigator(NewOwner);
		MyPawn = NewOwner;
		// net owner for RPC calls
		SetOwner(NewOwner);
	}
}

class AFPSCharacter* AShooterWeapon::GetPawnOwner() const
{
	return MyPawn;
}

bool AShooterWeapon::IsEquipped() const
{
	return bIsEquipped;
}

bool AShooterWeapon::IsAttachedToPawn() const
{
	return bIsEquipped || bPendingEquip;
}

void AShooterWeapon::FireWeapon(TSubclassOf<class AFPSProjectile> ProjectileClass)
{
	FVector ShootDir = GetInstigator()->GetBaseAimRotation().Vector();
	FVector Origin = Mesh1P->GetSocketLocation(MuzzleAttachPoint);

	// trace from camera to check what's under crosshair
	const float ProjectileAdjustRange = 10000.0f;
	const FVector StartTrace = GetCameraDamageStartLocation(ShootDir);
	const FVector EndTrace = StartTrace + ShootDir * ProjectileAdjustRange;
	FHitResult Impact = WeaponTrace(StartTrace, EndTrace);

	// and adjust directions to hit that actor
	//if (Impact.bBlockingHit)
	//{
	//	const FVector AdjustedDir = (Impact.ImpactPoint - Origin).GetSafeNormal();
	//	bool bWeaponPenetration = false;

	//	const float DirectionDot = FVector::DotProduct(AdjustedDir, ShootDir);
	//	if (DirectionDot < 0.0f)
	//	{
	//		// shooting backwards = weapon is penetrating
	//		bWeaponPenetration = true;
	//	}
	//	else if (DirectionDot < 0.5f)
	//	{
	//		// check for weapon penetration if angle difference is big enough
	//		// raycast along weapon mesh to check if there's blocking hit

	//		FVector MuzzleStartTrace = Origin - Mesh1P->GetSocketRotation(MuzzleAttachPoint).Vector() * 150.0f;
	//		FVector MuzzleEndTrace = Origin;
	//		FHitResult MuzzleImpact = WeaponTrace(MuzzleStartTrace, MuzzleEndTrace);

	//		if (MuzzleImpact.bBlockingHit)
	//		{
	//			bWeaponPenetration = true;
	//		}
	//	}

	//	if (bWeaponPenetration)
	//	{
	//		// spawn at crosshair position
	//		Origin = Impact.ImpactPoint - ShootDir * 10.0f;
	//	}
	//	else
	//	{
	//		// adjust direction to hit
	//		ShootDir = AdjustedDir;
	//	}
	//}

	FTransform SpawnTM(ShootDir.Rotation(), Origin);

	UWorld* World = GetWorld();
	if (World)
	{
		FActorSpawnParameters SpawnParams;
		SpawnParams.Owner = this;
		SpawnParams.Instigator = GetInstigator();

		// Spawn the projectile at the muzzle.
		AFPSProjectile* Projectile = World->SpawnActor<AFPSProjectile>(ProjectileClass, SpawnTM, SpawnParams);
		if (Projectile)
		{
			// Set the projectile's initial trajectory.
			Projectile->FireInDirection(ShootDir);

		}
	}
}

FVector AShooterWeapon::GetCameraDamageStartLocation(const FVector& AimDir) const
{
	AShooterPlayerController* PC = MyPawn ? Cast<AShooterPlayerController>(MyPawn->Controller) : NULL;
	FVector OutStartTrace = FVector::ZeroVector;

	if (PC)
	{
		// use player's camera
		FRotator UnusedRot;
		PC->GetPlayerViewPoint(OutStartTrace, UnusedRot);

		// Adjust trace so there is nothing blocking the ray between the camera and the pawn, and calculate distance from adjusted start
		OutStartTrace = OutStartTrace + AimDir * ((GetInstigator()->GetActorLocation() - OutStartTrace) | AimDir);
	}

	return OutStartTrace;
}

FHitResult AShooterWeapon::WeaponTrace(const FVector& StartTrace, const FVector& EndTrace) const
{
	// Perform trace to retrieve hit info
	FCollisionQueryParams TraceParams(SCENE_QUERY_STAT(WeaponTrace), true, GetInstigator());
	TraceParams.bReturnPhysicalMaterial = true;

	FHitResult Hit;
	GetWorld()->LineTraceSingleByChannel(Hit, StartTrace, EndTrace, ECC_Visibility, TraceParams);
	return Hit;
}
