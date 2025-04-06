// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "Engine/Canvas.h" // for FCanvasIcon
#include "ShooterWeapon.generated.h"

class UAnimMontage;
class AFPSCharacter;
class UAudioComponent;
class UParticleSystemComponent;
class UForceFeedbackEffect;
class USoundCue;
class UMatineeCameraShake;


USTRUCT()
struct FWeaponAnim
{
	GENERATED_BODY()

	/** animation played on pawn (1st person view) */
	UPROPERTY(EditDefaultsOnly, Category = Animation)
	UAnimMontage* Pawn1P;

	FWeaponAnim()
		: Pawn1P(nullptr)
	{
	}
};

UCLASS(Abstract, Blueprintable)
class AShooterWeapon : public AActor
{
	GENERATED_BODY()

	/** perform initial setup */
	virtual void PostInitializeComponents() override;

	virtual void Destroyed() override;
	
	/** weapon is now equipped by owner pawn */
	virtual void OnEquipFinished();

	/** check if it's currently equipped */
	bool IsEquipped() const;

	/** get pawn owner */
	UFUNCTION(BlueprintCallable, Category = "Game|Weapon")
	class AFPSCharacter* GetPawnOwner() const;

	/** crosshair parts icons (left, top, right, bottom and center) */
	UPROPERTY(EditDefaultsOnly, Category = HUD)
	FCanvasIcon Crosshair[5];

protected:

	/** pawn owner */
	UPROPERTY(Transient)
	class AFPSCharacter* MyPawn;

private:
	/** weapon mesh: 1st person view */
	UPROPERTY(VisibleDefaultsOnly, Category = Mesh)
	USkeletalMeshComponent* Mesh1P;
protected:

	/** firing audio (bLoopedFireSound set) */
	UPROPERTY(Transient)
	UAudioComponent* FireAC;

	/** name of bone/socket for muzzle in weapon mesh */
	UPROPERTY(EditDefaultsOnly, Category = Effects)
	FName MuzzleAttachPoint;

	/** camera shake on firing */
	UPROPERTY(EditDefaultsOnly, Category = Effects)
	TSubclassOf<UCameraShakeBase> FireCameraShake;

	/** force feedback effect to play when the weapon is fired */
	UPROPERTY(EditDefaultsOnly, Category = Effects)
	UForceFeedbackEffect* FireForceFeedback;

	/** single fire sound (bLoopedFireSound not set) */
	//UPROPERTY(EditDefaultsOnly, Category = Sound)
	////USoundCue* FireSound;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Sound)
	USoundBase* FireSound;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Sound)
	USoundBase* FireDrySound;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Sound)
	USoundBase* ReloadSound;

	/** looped fire sound (bLoopedFireSound set) */
	UPROPERTY(EditDefaultsOnly, Category = Sound)
	USoundCue* FireLoopSound;

	/** finished burst sound (bLoopedFireSound set) */
	UPROPERTY(EditDefaultsOnly, Category = Sound)
	USoundCue* FireFinishSound;

	/** out of ammo sound */
	UPROPERTY(EditDefaultsOnly, Category = Sound)
	USoundCue* OutOfAmmoSound;

	/** reload sound */
	/*UPROPERTY(EditDefaultsOnly, Category = Sound)
	USoundCue* ReloadSound;*/

	/** equip sound */
	UPROPERTY(EditDefaultsOnly, Category = Sound)
	USoundCue* EquipSound;

	/** equip animations */
	UPROPERTY(EditDefaultsOnly, Category = Animation)
	FWeaponAnim EquipAnim;

	/** fire animations */
	UPROPERTY(EditDefaultsOnly, Category = Animation)
	FWeaponAnim FireAnim;

	/** is fire sound looped? */
	UPROPERTY(EditDefaultsOnly, Category = Sound)
	uint32 bLoopedFireSound : 1;

	/** is fire animation looped? */
	UPROPERTY(EditDefaultsOnly, Category = Animation)
	uint32 bLoopedFireAnim : 1;

	/** is fire animation playing? */
	uint32 bPlayingFireAnim : 1;

	/** is weapon currently equipped? */
	uint32 bIsEquipped : 1;

	/** is weapon fire active? */
	uint32 bWantsToFire : 1;

	/** is reload animation playing? */
	UPROPERTY(Transient)
	uint32 bPendingReload : 1;

	/** weapon is refiring */
	uint32 bRefiring;

	/** last time when this weapon was switched to */
	float EquipStartedTime;

	/** how much time weapon needs to be equipped */
	float EquipDuration;

	/** burst counter, used for replicating fire events to remote clients */
	UPROPERTY(Transient)
	int32 BurstCounter;

	/** Handle for efficient management of OnEquipFinished timer */
	FTimerHandle TimerHandle_OnEquipFinished;

	/** Handle for efficient management of StopReload timer */
	FTimerHandle TimerHandle_StopReload;

	/** Handle for efficient management of ReloadWeapon timer */
	FTimerHandle TimerHandle_ReloadWeapon;

	/** Handle for efficient management of HandleFiring timer */
	FTimerHandle TimerHandle_HandleFiring;


	/** attaches weapon mesh to pawn's mesh */
	void AttachMeshToPawn();

	/** detaches weapon mesh from pawn */
	void DetachMeshFromPawn();
	

protected:
	/** Returns Mesh1P subobject **/
	FORCEINLINE USkeletalMeshComponent* GetMesh1P() const { return Mesh1P; }


public:

	AShooterWeapon(const FObjectInitializer& ObjectInitializer);

	/** set the weapon's owning pawn */
	void SetOwningPawn(AFPSCharacter* AFPSCharacter);
	/** weapon is being equipped by owner pawn */
	virtual void OnEquip(const AShooterWeapon* LastWeapon);
	/** [local] weapon specific fire implementation */
	virtual void FireWeapon(TSubclassOf<class AFPSProjectile> ProjectileClass);
	virtual void ReloadMagazine();
	FTimerHandle ReloadTimerHandle;
	void FinishReload();
	/** get the originating location for camera damage */
	FVector GetCameraDamageStartLocation(const FVector& AimDir) const;
	/** find hit */
	FHitResult WeaponTrace(const FVector& TraceFrom, const FVector& TraceTo) const;
};

