// Fill out your copyright notice in the Description page of Project Settings.


#include "Characters/PlayerCharacter.h"
#include "Interactables/Interactable.h"
#include "Camera/CameraComponent.h"
#include "Perception/AISense_Sight.h"
#include "Perception/AISense_Hearing.h"
#include "AI/AITags.h"
#include "Kismet/GameplayStatics.h"
#include "Kismet/KismetMathLibrary.h"
#include "Components/InputComponent.h"
#include "Core/MainGameState.h"
#include "Core/NetworkSettingsWat.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "Runtime/Engine/Classes/Engine/World.h"
#include "Perception/AIPerceptionStimuliSourceComponent.h"
#include "GameFramework/PlayerController.h"
#include "GameFramework/PlayerState.h"
#include "Core/MainPlayerController.h"

#if WITH_EDITOR
#include "Core/MainGameInstance.h"
#endif

// Sets default values
APlayerCharacter::APlayerCharacter()
{
	// Set this character to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;
	GetCharacterMovement()->NavAgentProps.bCanCrouch = true;

	// Create a CameraComponent	
	FirstPersonCameraComponent = CreateDefaultSubobject<UCameraComponent>(TEXT("FirstPersonCamera"));
	FirstPersonCameraComponent->SetupAttachment(RootComponent);
	FirstPersonCameraComponent->SetRelativeLocation(FVector(-39.56f, 1.75f, 64.f)); // Position the camera
	FirstPersonCameraComponent->bUsePawnControlRotation = true;

	// Create a mesh component that will be used when being viewed from a '1st person' view (when controlling this pawn)
	Mesh1P = CreateDefaultSubobject<USkeletalMeshComponent>(TEXT("CharacterMesh1P"));
	Mesh1P->SetOnlyOwnerSee(true);
	Mesh1P->SetupAttachment(FirstPersonCameraComponent);
	Mesh1P->bCastDynamicShadow = false;
	Mesh1P->CastShadow = false;
	Mesh1P->SetRelativeRotation(FRotator(1.9f, -19.19f, 5.2f));
	Mesh1P->SetRelativeLocation(FVector(-0.5f, -4.4f, -155.7f));

	auto* Mesh3P = this->GetMesh();
	Mesh3P->AddRelativeRotation(FRotator(0.0f, -90.0f, 0.0f));
	Mesh3P->AddRelativeLocation(FVector(-30.0f, 10.0f, -90.0f));
	Mesh3P->SetVisibleFlag(false);

	SetupStimulus();

	bIsAlive = true;
}

bool APlayerCharacter::AddInteractable(AInteractable* Interactable)
{
	InteractablesSet.Add(Interactable);
	return IsClosestInteractable(Interactable);
}

void APlayerCharacter::RemoveInteractable(AInteractable* Interactable)
{
	InteractablesSet.Remove(Interactable);
}

AInteractable* APlayerCharacter::GetClosestInteractable()
{
	const FVector PlayerLocation = GetActorLocation();
	TPair<float, AInteractable*> ClosestInteractable(-1.0f, nullptr);

	for (auto InteractableSetItem : InteractablesSet)
	{
		if (InteractableSetItem)
		{
			const float Distance = UKismetMathLibrary::Vector_Distance2D(
				PlayerLocation, InteractableSetItem->GetActorLocation());
			if (ClosestInteractable.Key == -1.0f || ClosestInteractable.Key > Distance)
			{
				ClosestInteractable.Key = Distance;
				ClosestInteractable.Value = InteractableSetItem;
			}
		}
	}
	return ClosestInteractable.Value;
}

bool APlayerCharacter::IsClosestInteractable(AInteractable* Interactable)
{
	if (GetClosestInteractable() == Interactable)
	{
		return true;
	}

	return false;
}

// Called when the game starts or when spawned
void APlayerCharacter::BeginPlay()
{
	Super::BeginPlay();

	if (!IsLocallyControlled())
	{
		GetMesh()->SetVisibility(true);
	}
}

void APlayerCharacter::CreateDebugWidget()
{
	UE_LOG(LogTemp,Warning,TEXT("Oui"))
	AMainPlayerController* PC = Cast<AMainPlayerController>(GetController());
	if (!PC->DebugGridWidget)
	{
		PC->DebugGridWidget = CreateWidget<UUserWidget>(PC, GridDebugWidget);
		PC->DebugGridWidget->AddToViewport();
	}
	else
	{
		PC->DebugGridWidget->RemoveFromParent();
		PC->DebugGridWidget->Destruct();
		PC->DebugGridWidget = nullptr;
	}
}

void APlayerCharacter::SetPlayerAsReadyForRestart()
{
	RestartGame(GetPlayerState()->GetPlayerId());
}


void APlayerCharacter::RestartGame_Implementation(uint32 PlayerId)
{
	auto* GameState = Cast<AMainGameState>(GetWorld()->GetGameState());
	GameState->PlayersReadyForRestartIds.Add(PlayerId);

	int MatchingPlayersIdsCount = 0;
	for (auto CurrentPlayerState : GameState->PlayerArray)
	{
		for (auto& ReadyPlayerId : GameState->PlayersReadyForRestartIds)
		{
			if (ReadyPlayerId == CurrentPlayerState->GetPlayerId())
			{
				++MatchingPlayersIdsCount;
				break;
			}
		}
	}

	if (MatchingPlayersIdsCount == GameState->PlayerArray.Num())
	{
		UE_LOG(LogTemp, Warning, TEXT("All players are ready to restart, moving to lobby level"));
		const auto* NetworkSettingsWat = GetDefault<UNetworkSettingsWat>();
		GetWorld()->ServerTravel(*NetworkSettingsWat->LobbyLevel.GetAssetName());
		GameState->PlayersReadyForRestartIds.Empty();
	}
}


// Called every frame
void APlayerCharacter::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

#if WITH_EDITOR
	GEngine->AddOnScreenDebugMessage(-1, 0.f, FColor::Red,
	                                 Cast<UMainGameInstance>(GetGameInstance())->bIsHost ? L"HOST" : L"CLIENT");
#endif
}

// Called to bind functionality to input
void APlayerCharacter::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);

	// Bind jump events
	PlayerInputComponent->BindAction("Jump", IE_Pressed, this, &ACharacter::Jump);
	PlayerInputComponent->BindAction("Jump", IE_Released, this, &ACharacter::StopJumping);
	PlayerInputComponent->BindAction("Interact", IE_Pressed, this, &APlayerCharacter::Interact);
	PlayerInputComponent->BindAction("Return", IE_Pressed, this, &APlayerCharacter::Return);

	// Bind movement events
	PlayerInputComponent->BindAxis("MoveForward", this, &APlayerCharacter::MoveForward);
	PlayerInputComponent->BindAxis("MoveRight", this, &APlayerCharacter::MoveRight);

	// We have 2 versions of the rotation bindings to handle different kinds of devices differently
	// "turn" handles devices that provide an absolute delta, such as a mouse.
	// "turnRate" is for devices that we choose to treat as a rate of change, such as an analog joystick
	PlayerInputComponent->BindAxis("Turn", this, &APawn::AddControllerYawInput);
	PlayerInputComponent->BindAxis("TurnRate", this, &APlayerCharacter::TurnAtRate);
	PlayerInputComponent->BindAxis("LookUp", this, &APawn::AddControllerPitchInput);
	PlayerInputComponent->BindAxis("LookUpRate", this, &APlayerCharacter::LookUpAtRate);

	PlayerInputComponent->BindAction("Distract", IE_Pressed, this, &APlayerCharacter::OnDistract);
	PlayerInputComponent->BindAction("Sprint", IE_Pressed, this, &APlayerCharacter::StartSprinting);
	PlayerInputComponent->BindAction("Sprint", IE_Released, this, &APlayerCharacter::StopSprinting);
	PlayerInputComponent->BindAction("Crouch", IE_Pressed, this, &APlayerCharacter::StartCrouching);
	PlayerInputComponent->BindAction("Crouch", IE_Released, this, &APlayerCharacter::StopCrouching);

	PlayerInputComponent->BindAction("Debug", IE_Pressed, this, &APlayerCharacter::CreateDebugWidget);
}

bool APlayerCharacter::IsAlive() const
{
	return bIsAlive;
}

void APlayerCharacter::Die()
{
	//TODO trigger die anim
	bIsAlive = false;
	DisableInput(nullptr);
}

void APlayerCharacter::SetupStimulus()
{
	Stimulus = CreateDefaultSubobject<UAIPerceptionStimuliSourceComponent>(TEXT("Stimulus"));
	Stimulus->RegisterForSense(TSubclassOf<UAISense_Sight>());
	Stimulus->RegisterWithPerceptionSystem();
}

void APlayerCharacter::OnDistract()
{
	if (DistractionSound)
	{
		FVector const Location = GetActorLocation();
		UGameplayStatics::PlaySoundAtLocation(GetWorld(), DistractionSound, Location);
		UAISense_Hearing::ReportNoiseEvent(GetWorld(), Location, 1.0f, this, 0.0f, AITags::NoiseTag);
	}
}

void APlayerCharacter::MoveForward(float Value)
{
	if (Value != 0.0f)
	{
		// add movement in that direction
		AddMovementInput(GetActorForwardVector(), Value);
	}
}

void APlayerCharacter::MoveRight(float Value)
{
	if (Value != 0.0f)
	{
		// add movement in that direction
		AddMovementInput(GetActorRightVector(), Value);
	}
}

void APlayerCharacter::TurnAtRate(float Rate)
{
	// calculate delta for this frame from the rate information
	AddControllerYawInput(Rate * BaseTurnRate * GetWorld()->GetDeltaSeconds());
}

void APlayerCharacter::LookUpAtRate(float Rate)
{
	// calculate delta for this frame from the rate information
	AddControllerPitchInput(Rate * BaseLookUpRate * GetWorld()->GetDeltaSeconds());
}

void APlayerCharacter::Interact_Implementation()
{
	auto* ClosestInteractable = GetClosestInteractable();
	if (ClosestInteractable)
	{
		ClosestInteractable->Interact();
	}
}

void APlayerCharacter::Return_Implementation()
{
	Cast<APlayerController>(this->GetController())->SetViewTargetWithBlend(this, 1.0f);
}

void APlayerCharacter::StartSprinting()
{
	GetCharacterMovement()->MaxWalkSpeed += SprintAdditionalSpeedValue;
}

void APlayerCharacter::StopSprinting()
{
	GetCharacterMovement()->MaxWalkSpeed -= SprintAdditionalSpeedValue;
}

void APlayerCharacter::StartCrouching()
{
	Crouch();
}

void APlayerCharacter::StopCrouching()
{
	UnCrouch();
}
