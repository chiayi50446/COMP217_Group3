// Copyright Epic Games, Inc. All Rights Reserved.

//#include "ShooterGame.h"
#include "ShooterWeapon.h"
#include "FPSCharacter.h"
#include "Particles/ParticleSystemComponent.h"
#include "ShooterPlayerController.h"
#include "FPSGameInstance.h"
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

	PrimaryActorTick.bCanEverTick = true;
	PrimaryActorTick.TickGroup = TG_PrePhysics;
	SetRemoteRoleForBackwardsCompat(ROLE_SimulatedProxy);
	bReplicates = true;
	bNetUseOwnerRelevancy = true;
}

void AShooterWeapon::PostInitializeComponents()
{
	Super::PostInitializeComponents();

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

void AShooterWeapon::FireWeapon(TSubclassOf<class AFPSProjectile> ProjectileClass)
{
	if (GetWorld()->GetMapName().Contains("Menu"))
	{
		return;
	}
	UFPSGameInstance* GameInstance = Cast<UFPSGameInstance>(GetGameInstance());
	if (GameInstance)
	{
		if (GameInstance->Magazine == 0) {

			if (FireDrySound) {
				UGameplayStatics::SpawnSoundAtLocation(
					this,
					FireDrySound,
					GetActorLocation()
				);
			}
			return;
		}
		else {
			GameInstance->Magazine--;
		}
	}
	if (MuzzleFX)
	{
		if (MuzzlePSC == NULL)
		{
			// Split screen requires we create 2 effects. One that we see and one that the other player sees.
			if ((MyPawn != NULL) && (MyPawn->IsLocallyControlled() == true))
			{
				AController* PlayerCon = MyPawn->GetController();
				if (PlayerCon != NULL)
				{
					Mesh1P->GetSocketLocation(MuzzleAttachPoint);
					MuzzlePSC = UGameplayStatics::SpawnEmitterAttached(MuzzleFX, Mesh1P, MuzzleAttachPoint);
					MuzzlePSC->bOwnerNoSee = false;
					MuzzlePSC->bOnlyOwnerSee = true;
				}
			}
			else
			{
				MuzzlePSC = UGameplayStatics::SpawnEmitterAttached(MuzzleFX, Mesh1P, MuzzleAttachPoint);
			}
		}
	}

	if (FireSound) {
		UAudioComponent* AudioComponent = UGameplayStatics::SpawnSoundAtLocation(
			this,
			FireSound,
			GetActorLocation()
		);

		if (AudioComponent)
		{
			AudioComponent->SetPitchMultiplier(1.5f);
		}
	}

	FVector ShootDir = GetInstigator()->GetBaseAimRotation().Vector();
	FVector Origin = Mesh1P->GetSocketLocation(MuzzleAttachPoint);

	// trace from camera to check what's under crosshair
	const float ProjectileAdjustRange = 10000.0f;
	const FVector StartTrace = GetCameraDamageStartLocation(ShootDir);
	const FVector EndTrace = StartTrace + ShootDir * ProjectileAdjustRange;
	FHitResult Impact = WeaponTrace(StartTrace, EndTrace);

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

	GetWorld()->GetTimerManager().SetTimer(
		FlameTimerHandle,
		this,
		&AShooterWeapon::StopSimulatingWeaponFire,
		0.06f,
		false
	);
}

void AShooterWeapon::StopSimulatingWeaponFire() {

	if (MuzzlePSC != NULL)
	{
		MuzzlePSC->DeactivateSystem();
		MuzzlePSC = NULL;
	}
}

void AShooterWeapon::ReloadMagazine()
{
	GetWorld()->GetTimerManager().SetTimer(
		ReloadTimerHandle,
		this,
		&AShooterWeapon::FinishReload,
		1.0f,
		false
	);

	if (ReloadSound) {

		UGameplayStatics::PlaySoundAtLocation(
			this,
			ReloadSound,
			GetActorLocation()
		);
	}
}


void AShooterWeapon::FinishReload()
{
	UFPSGameInstance* GameInstance = Cast<UFPSGameInstance>(GetGameInstance());
	if (GameInstance)
	{
		if (GameInstance->CurrentLevel == "Safe_House") {
			GameInstance->Magazine = 5;
		}
		else {
			GameInstance->Magazine = 10;
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
