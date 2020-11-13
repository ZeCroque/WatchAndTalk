// Fill out your copyright notice in the Description page of Project Settings.


#include "NpcCharacter.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "Kismet/GameplayStatics.h"
#include "MainPlayerState.h"

// Sets default values
ANpcCharacter::ANpcCharacter()
{
    // Set this character to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
    PrimaryActorTick.bCanEverTick = true;
    bUseControllerRotationYaw = false;
    GetCharacterMovement()->bOrientRotationToMovement = true;
}

void ANpcCharacter::MeleeAttack() const
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

UAnimMontage* ANpcCharacter::GetMontage() const
{
    return Montage;
}

// Called when the game starts or when spawned
void ANpcCharacter::BeginPlay()
{
    Super::BeginPlay();
}

// Called every frame
void ANpcCharacter::Tick(const float DeltaTime)
{
    Super::Tick(DeltaTime);
}

// Called to bind functionality to input
void ANpcCharacter::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
    Super::SetupPlayerInputComponent(PlayerInputComponent);
}
