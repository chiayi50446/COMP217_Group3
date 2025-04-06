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

	/** get pawn owner */
	UFUNCTION(BlueprintCallable, Category = "Game|Weapon")
	class AFPSCharacter* GetPawnOwner() const;

protected:

	/** pawn owner */
	UPROPERTY(Transient)
	class AFPSCharacter* MyPawn;

private:
	/** weapon mesh: 1st person view */
	UPROPERTY(VisibleDefaultsOnly, Category = Mesh)
	USkeletalMeshComponent* Mesh1P;
protected:
	/** name of bone/socket for muzzle in weapon mesh */
	UPROPERTY(EditDefaultsOnly, Category = Effects)
	FName MuzzleAttachPoint;

	/** single fire sound */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Sound)
	USoundBase* FireSound;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Sound)
	USoundBase* FireDrySound;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Sound)
	USoundBase* ReloadSound;

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
	
	/** weapon specific fire implementation */
	virtual void FireWeapon(TSubclassOf<class AFPSProjectile> ProjectileClass);
	virtual void ReloadMagazine();
	FTimerHandle ReloadTimerHandle;
	void FinishReload();

	/** get the originating location for camera damage */
	FVector GetCameraDamageStartLocation(const FVector& AimDir) const;
	
	/** find hit */
	FHitResult WeaponTrace(const FVector& TraceFrom, const FVector& TraceTo) const;
};

