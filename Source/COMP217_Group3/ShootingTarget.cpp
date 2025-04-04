// Fill out your copyright notice in the Description page of Project Settings.


#include "ShootingTarget.h"
#include "FPSCharacter.h"
#include "FPSPlayerState.h"
#include "Animation/AnimSequence.h"
#include "COMP217_Group3GameModeBase.h"

// Sets default values
AShootingTarget::AShootingTarget()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

    // Set target Mesh
    SkeletalMesh = CreateDefaultSubobject<USkeletalMeshComponent>(TEXT("SkeletalMesh"));
    RootComponent = SkeletalMesh;

    SkeletalMesh->SetCollisionObjectType(ECC_PhysicsBody);
    SkeletalMesh->SetCollisionEnabled(ECollisionEnabled::QueryAndPhysics);
    SkeletalMesh->SetCollisionResponseToChannel(ECC_Visibility, ECR_Block);

}

// Called when the game starts or when spawned
void AShootingTarget::BeginPlay()
{
	Super::BeginPlay();
}

// Called every frame
void AShootingTarget::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}


void AShootingTarget::OnHit(AActor* HitActor)
{
    AFPSCharacter* Shooter = Cast<AFPSCharacter>(HitActor);
    if (Shooter)
    {
        AFPSPlayerState* PlayerState = Shooter->GetPlayerState<AFPSPlayerState>();
        if (PlayerState)
        {
            if (Score != 0)
            {
                PlayerState->AddScore(Score);
            }
        }
    }

    ACOMP217_Group3GameModeBase* GameMode = GetWorld()->GetAuthGameMode<ACOMP217_Group3GameModeBase>();
    if (GameMode)
    {
        GameMode->RespawnTarget(GetActorLocation(), TargetBlueprint);

        if (AddTime != 0)
        {
            GameMode->AddTimer(AddTime);
        }
    }

    // remove Target
    Destroy();
}