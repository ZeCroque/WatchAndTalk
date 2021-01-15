// Fill out your copyright notice in the Description page of Project Settings.


#include "Characters/MonsterCharacter.h"

#include "Blueprint/UserWidget.h"
#include "Characters/PlayerCharacter.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "Kismet/GameplayStatics.h"

// Sets default values
AMonsterCharacter::AMonsterCharacter()
{
	// Set this character to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;
	bUseControllerRotationYaw = false;
	GetCharacterMovement()->bOrientRotationToMovement = true;
}

void AMonsterCharacter::MeleeAttack(APlayerCharacter* Player)
{
	//todo : Execute Melee Attack Animation

	if (GEngine)
	{
		GEngine->AddOnScreenDebugMessage(-1, 1.0f, FColor::Yellow, "Attack !");
	}


	KillPlayer(Player);
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

void AMonsterCharacter::KillPlayer_Implementation(APlayerCharacter* Player)
{
	UE_LOG(LogTemp, Warning, TEXT("Player %s was kill"), *Player->GetName());
	Player->Die();

	auto* GameOverWidget = NewObject<UUserWidget>(this, GameOverWidgetTemplate);
	if (GameOverWidget != nullptr)
	{
		GameOverWidget->AddToViewport();
		auto* PlayerController = UGameplayStatics::GetPlayerController(GetWorld(), 0);
		PlayerController->SetInputMode(FInputModeUIOnly());
		PlayerController->bShowMouseCursor = true;
	}
}

void AMonsterCharacter::StartGoalReaction(ACharacter* Character)
{
	GEngine->AddOnScreenDebugMessage(-1, 1, FColor::Purple,TEXT("Start Goal Reaction"));
	PlayerToChase = Cast<APlayerCharacter>(Character);
}

void AMonsterCharacter::StopGoalReaction()
{
	PlayerToChase = nullptr;
}
