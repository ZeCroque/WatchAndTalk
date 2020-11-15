// Fill out your copyright notice in the Description page of Project Settings.


#include "MonsterCharacter.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "Kismet/GameplayStatics.h"
#include "MainPlayerState.h"

// Sets default values
AMonsterCharacter::AMonsterCharacter()
{
    // Set this character to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
    PrimaryActorTick.bCanEverTick = true;
    bUseControllerRotationYaw = false;
    GetCharacterMovement()->bOrientRotationToMovement = true;
}

void AMonsterCharacter::MeleeAttack() const
{
    //todo : Execute Melee Attack Animation

    if (GEngine)
    {
        GEngine->AddOnScreenDebugMessage(-1, 1.0f, FColor::Yellow, "Attack !");
    }

    auto* PlayerController = UGameplayStatics::GetPlayerController(GetWorld(), 0);
    if (PlayerController)
    {
        auto* ClientState = PlayerController->GetPlayerState<AMainPlayerState>();
        if (ClientState)
        {
            ClientState->KillPlayer();
        }
    }
}

UAnimMontage* AMonsterCharacter::GetMontage() const
{
    return Montage;
}

// Called when the game starts or when spawned
void AMonsterCharacter::BeginPlay()
{
    Super::BeginPlay();
}

// Called every frame
void AMonsterCharacter::Tick(const float DeltaTime)
{
    Super::Tick(DeltaTime);
}

// Called to bind functionality to input
void AMonsterCharacter::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
    Super::SetupPlayerInputComponent(PlayerInputComponent);
}
