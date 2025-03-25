// Fill out your copyright notice in the Description page of Project Settings.


#include "ShootingTarget.h"
#include "FPSCharacter.h"
#include "FPSPlayerState.h"
#include "COMP217_Group3GameModeBase.h"

// Sets default values
AShootingTarget::AShootingTarget()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

    // Set target Mesh
    TargetMesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("TargetMesh"));
    RootComponent = TargetMesh;

    // set target can be shoot
    TargetMesh->SetCollisionObjectType(ECC_PhysicsBody);
    TargetMesh->SetCollisionEnabled(ECollisionEnabled::QueryAndPhysics);
    TargetMesh->SetCollisionResponseToChannel(ECC_Visibility, ECR_Block);

    if (TargetMesh->GetMaterial(0) == nullptr)
    {
        static ConstructorHelpers::FObjectFinder<UStaticMesh>Mesh(TEXT("'/Game/Sphere.Sphere'"));
        if (Mesh.Succeeded())
        {
            TargetMesh->SetStaticMesh(Mesh.Object);
        }
    }
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
            PlayerState->AddScore(10); // add 10 score
        }
    }

    ACOMP217_Group3GameModeBase* GameMode = GetWorld()->GetAuthGameMode<ACOMP217_Group3GameModeBase>();
    if (GameMode)
    {
        GameMode->RespawnTarget(GetActorLocation());
    }

    // remove Target
    Destroy();
}