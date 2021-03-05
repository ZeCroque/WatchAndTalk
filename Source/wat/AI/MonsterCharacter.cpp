// Fill out your copyright notice in the Description page of Project Settings.


#include "MonsterCharacter.h"
#include "Blueprint/UserWidget.h"
#include "wat/Core/WatPlayerCharacter.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "wat/AI/GridClearComponent.h"
#include "Kismet/GameplayStatics.h"

// Sets default values
AMonsterCharacter::AMonsterCharacter()
{
	// Set this character to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;
	bUseControllerRotationYaw = false;
	GetCharacterMovement()->bOrientRotationToMovement = true;

	//CreateClearGridComponent
	GridClearComponent = CreateDefaultSubobject<UGridClearComponent>(TEXT("GridClearComponent"));
	GridClearComponent->SetupAttachment(RootComponent);
	GridClearComponent->SetRelativeLocation(FVector(-39.56f, 1.75f, 64.f));
}

void AMonsterCharacter::MeleeAttack(AWatPlayerCharacter* Player)
{
	OnEventAttackDispatcher.Broadcast();
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

void AMonsterCharacter::KillPlayer_Implementation(AWatPlayerCharacter* Player)
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
	PlayerToChase = Cast<AWatPlayerCharacter>(Character);
	OnEventReactionModeDispatcher.Broadcast();
}

void AMonsterCharacter::StopGoalReaction()
{
	PlayerToChase = nullptr;
}
