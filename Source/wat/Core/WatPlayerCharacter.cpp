// Fill out your copyright notice in the Description page of Project Settings.

#include "wat/Core/WatPlayerCharacter.h"
#include "wat/Laser.h"
#include "wat/GameplayActors/Interactables/Interactable.h"
#include "Camera/CameraComponent.h"
#include "Components/CapsuleComponent.h"
#include "Perception/AISense_Sight.h"
#include "Perception/AISense_Hearing.h"
#include "wat/AI/AITags.h"
#include "Kismet/GameplayStatics.h"
#include "Kismet/KismetMathLibrary.h"
#include "Components/InputComponent.h"
#include "wat/Core/WatGameState.h"
#include "wat/Core/WatSettings.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "Runtime/Engine/Classes/Engine/World.h"
#include "Perception/AIPerceptionStimuliSourceComponent.h"
#include "GameFramework/PlayerController.h"
#include "GameFramework/PlayerState.h"
#include "wat/PauseWidget.h"
#include "wat/AI/GridManager.h"
#include "wat/AI/MonsterCharacter.h"
#include "wat/Core/WatPlayerController.h"
#include "wat/Widgets/ObjectivesHUDWidget.h"

#if WITH_EDITOR
#include "wat/Core/WatGameInstance.h"
#endif

// Sets default values
AWatPlayerCharacter::AWatPlayerCharacter()
{
	// Set this character to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

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
	bIsMechanist = false;
	OriginalMaxSpeed = 0;

	CrouchingCapsuleReductionRatio = 0.5f;
	CrouchingCameraOffset = 30.f;
	CrouchingTransitionDuration = 0.2f;
}

bool AWatPlayerCharacter::AddInteractable(AInteractable* Interactable)
{
	InteractablesSet.Add(Interactable);
	return IsClosestInteractable(Interactable);
}

void AWatPlayerCharacter::RemoveInteractable(AInteractable* Interactable)
{
	InteractablesSet.Remove(Interactable);
}

AInteractable* AWatPlayerCharacter::GetClosestInteractable()
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

bool AWatPlayerCharacter::IsClosestInteractable(AInteractable* Interactable)
{
	if (GetClosestInteractable() == Interactable)
	{
		return true;
	}

	return false;
}

// Called when the game starts or when spawned
void AWatPlayerCharacter::BeginPlay()
{
	Super::BeginPlay();

	if (!IsLocallyControlled())
	{
		GetMesh()->SetVisibility(true);
	}
	else if (HasAuthority())
	{
		bIsMechanist = true;
	}
	BaseCapsuleHeight = GetCapsuleComponent()->GetScaledCapsuleHalfHeight();
	CrouchCapsuleDelta = BaseCapsuleHeight * CrouchingCapsuleReductionRatio;
	bWantsToChangeCrouchState = false;
	bIsCrouching = false;
	CrouchingDeltaTimeCount = 0.f;
}

void AWatPlayerCharacter::CreateDebugWidget()
{
	AWatPlayerController* PC = Cast<AWatPlayerController>(GetController());
	if (!PC->DebugGridWidget)
	{
		PC->DebugGridWidget = CreateWidget<UUserWidget>(PC, GridDebugWidget);
		PC->DebugGridWidget->AddToViewport();
	}
	else
	{
		PC->DebugGridWidget->RemoveFromParent();
		PC->DebugGridWidget = nullptr;
	}
}

void AWatPlayerCharacter::ClearGridWhenSprinting()
{
	const int Range = FMath::RoundToInt(ARange * GetCharacterMovement()->MaxWalkSpeed + BRange);

	if (GameState->GridManager && GetDistanceTo(GameState->MonsterCharacter) <= Range)
	{
		const int Ray = FMath::RoundToInt(ARay * GetCharacterMovement()->MaxWalkSpeed + BRay);

		GameState->GridManager->SetValueOutside(GetActorLocation(), Ray);
		GameState->GridManager->SetValueAround(GetActorLocation(), Ray);
	}
}

void AWatPlayerCharacter::SetPlayerAsReadyForRestart()
{
	RestartGame(GetPlayerState()->GetPlayerId());
}

void AWatPlayerCharacter::SwitchStartMode_Implementation()
{
	GameState->bTutorialActivated = !GameState->bTutorialActivated;
	GameState->ObjectiveIndex = !GameState->ObjectiveIndex;
}


void AWatPlayerCharacter::RestartGame_Implementation(uint32 PlayerId)
{
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
		const auto* NetworkSettingsWat = GetDefault<UWatSettings>();
		GetWorld()->ServerTravel(*NetworkSettingsWat->LobbyLevel.GetAssetName());
		GameState->PlayersReadyForRestartIds.Empty();
	}
}


void AWatPlayerCharacter::TogglePauseMenu()
{
	AWatPlayerController* PC = Cast<AWatPlayerController>(GetController());
	if (!PC->PauseWidget)
	{
		PC->PauseWidget = CreateWidget<UPauseWidget>(PC, PauseWidget);
		if (PC->PauseWidget)
		{
			PC->PauseWidget->AddToViewport();
			PC->bShowMouseCursor = true;
			FInputModeUIOnly UIOnly;
			PC->SetInputMode(UIOnly);
		}
	}
}

void AWatPlayerCharacter::UpdatePlayerHeight_Implementation(float NewHeight, FVector NewMeshRelativeLocation)
{
	GetCapsuleComponent()->SetCapsuleHalfHeight(NewHeight);
	GetMesh()->SetRelativeLocation(NewMeshRelativeLocation);
}

void AWatPlayerCharacter::UpdateRemotePlayerSpeed_Implementation(AWatPlayerCharacter* Player, float NewSpeed)
{
	Player->GetCharacterMovement()->MaxWalkSpeed = NewSpeed;
}

void AWatPlayerCharacter::SwitchPlayersClasses_Implementation()
{
	for (auto Iterator = GetWorld()->GetPlayerControllerIterator(); Iterator; ++Iterator)
	{
		auto* PlayerController = Cast<AWatPlayerController>(*Iterator);
		if (PlayerController)
		{
			auto* PlayerCharacter = Cast<AWatPlayerCharacter>(PlayerController->GetPawn());
			PlayerCharacter->bIsMechanist = !PlayerCharacter->bIsMechanist;
		}
	}
}

// Called every frame
void AWatPlayerCharacter::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
	if (!GameState)
	{
		GameState = Cast<AWatGameState>(GetWorld()->GetGameState());
	}
#if WITH_EDITOR
	if (IsLocallyControlled())
	{
		GEngine->AddOnScreenDebugMessage(-1, 0.f, FColor::Red,
		                                 Cast<UWatGameInstance>(GetGameInstance())->bIsHost ? L"HOST" : L"CLIENT");
		GEngine->AddOnScreenDebugMessage(-1, 0.f, FColor::Blue, bIsMechanist ? L"MECHANIST" : L"SCIENTIST");
		if (GameState)
		{
			GEngine->AddOnScreenDebugMessage(-1, 0.f, FColor::Purple,
			                                 FString("Start mode: ") + (GameState->bTutorialActivated
				                                                            ? FString("Tutorial")
				                                                            : FString("Quick Start")));
		}
	}
#endif

	if (GameState && GameState->bGameStarted && IsLocallyControlled())
	{
		ARay = static_cast<float>(MaxRangToKeep - MinRangToKeep) / (OriginalMaxSpeed - (OriginalMaxSpeed +
			SprintAdditionalSpeedValue));
		BRay = static_cast<float>(MinRangToKeep) - ARay * (OriginalMaxSpeed + SprintAdditionalSpeedValue);

		ARange = static_cast<float>(MinRangFromMonster - MaxRangFromMonster) / (OriginalMaxSpeed - (OriginalMaxSpeed +
			SprintAdditionalSpeedValue));
		BRange = static_cast<float>(MaxRangFromMonster) - ARange * (OriginalMaxSpeed + SprintAdditionalSpeedValue);


		AWatPlayerController* PC = Cast<AWatPlayerController>(GetController());
		if (!PC->ObjectivesHUDWidget)
		{
			PC->ObjectivesHUDWidget = CreateWidget<UObjectivesHUDWidget>(PC, ObjectivesHUDWidgetClass);
			PC->ObjectivesHUDWidget->Parent = this;
			PC->ObjectivesHUDWidget->Controller = PC;
			PC->ObjectivesHUDWidget->UpdateObjectiveList();
		}

		if (bIsUsingLaser)
		{
			FHitResult Result;
			FVector Start = FirstPersonCameraComponent->GetComponentLocation();
			FVector ForwardVector = FirstPersonCameraComponent->GetForwardVector();
			FVector End = ((ForwardVector * 1000.f) + Start);
			FCollisionObjectQueryParams ObjectQueryParams;
			ObjectQueryParams.AddObjectTypesToQuery(ECollisionChannel::ECC_GameTraceChannel1);

			if (GetWorld()->LineTraceSingleByObjectType(Result, Start, End, ObjectQueryParams))
			{
				if (Result.bBlockingHit)
				{
					if (!LaserPointer)
					{
						LaserPointer = GetWorld()->SpawnActor<ALaser>(LaserPointerClass, Result.ImpactPoint,
						                                              Result.Normal.Rotation());
					}
					else
					{
						LaserPointer->SetActorLocation(Result.ImpactPoint + Result.Normal * 0.01);
						LaserPointer->SetActorRelativeRotation(Result.Actor->GetActorRotation());
					}
				}
			}
		}
		else
		{
			if (LaserPointer)
			{
				LaserPointer->Destroy();
				LaserPointer = nullptr;
			}
		}

		//Sprint
		auto* LocalCharacterMovement = GetCharacterMovement();
		if (!OriginalMaxSpeed)
		{
			OriginalMaxSpeed = LocalCharacterMovement->MaxWalkSpeed;
		}

		if (bIsSprinting && !GetVelocity().IsZero())
		{
			if (LocalCharacterMovement->MaxWalkSpeed < OriginalMaxSpeed + SprintAdditionalSpeedValue)
			{
				LocalCharacterMovement->MaxWalkSpeed += SprintSpeedIncreaseRate * DeltaTime;
				if (!HasAuthority())
				{
					UpdateRemotePlayerSpeed(this, LocalCharacterMovement->MaxWalkSpeed);
				}
			}
			else if (LocalCharacterMovement->MaxWalkSpeed > OriginalMaxSpeed + SprintAdditionalSpeedValue)
			{
				LocalCharacterMovement->MaxWalkSpeed = OriginalMaxSpeed + SprintAdditionalSpeedValue;
				if (!HasAuthority())
				{
					UpdateRemotePlayerSpeed(this, LocalCharacterMovement->MaxWalkSpeed);
				}
			}
			PC->PlayerCameraManager->PlayCameraShake(SprintShake,
			                                         LocalCharacterMovement->GetMaxSpeed() / (OriginalMaxSpeed +
				                                         SprintAdditionalSpeedValue));
		}
		else if (!bIsSprinting && LocalCharacterMovement->MaxWalkSpeed > OriginalMaxSpeed)
		{
			LocalCharacterMovement->MaxWalkSpeed -= SprintSpeedDecreaseRate * DeltaTime;
			if (!HasAuthority())
			{
				UpdateRemotePlayerSpeed(this, LocalCharacterMovement->MaxWalkSpeed);
			}
		}
		if (!bIsCrouching)
		{
			ClearGridWhenSprinting();
		}
	}

	//Crouch
	if(GetWorld()->IsServer() && bWantsToChangeCrouchState)
	{
		CrouchingDeltaTimeCount += DeltaTime;
		if(CrouchingDeltaTimeCount <= CrouchingTransitionDuration)
		{
			auto MeshRelativeLocation = GetMesh()->GetRelativeLocation();
			const auto CapsuleOffset = BaseCapsuleHeight * CrouchingCapsuleReductionRatio * DeltaTime;
			if(!bIsCrouching)
			{
				const auto NewCapsuleHeight = GetCapsuleComponent()->GetScaledCapsuleHalfHeight() - CapsuleOffset * 2.f;	
				MeshRelativeLocation.Z += CrouchCapsuleDelta * 2.f * DeltaTime;
				UpdatePlayerHeight(NewCapsuleHeight, MeshRelativeLocation);
			}
			else
			{
				const auto NewCapsuleHeight = GetCapsuleComponent()->GetScaledCapsuleHalfHeight() + CapsuleOffset * 2.f;
				MeshRelativeLocation.Z -= CrouchCapsuleDelta * 2.f * DeltaTime;
				UpdatePlayerHeight(NewCapsuleHeight, MeshRelativeLocation);
			}
		}
		else
		{
			bWantsToChangeCrouchState = false;
			CrouchingDeltaTimeCount = 0.f;
			if(!bIsCrouching)
			{	
				bIsCrouching = true;
			}
			else
			{
				bIsCrouching = false;
			}
		}
	}
}

// Called to bind functionality to input
void AWatPlayerCharacter::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);

	// Axis
	PlayerInputComponent->BindAxis("MoveForward", this, &AWatPlayerCharacter::MoveForward);
	PlayerInputComponent->BindAxis("MoveRight", this, &AWatPlayerCharacter::MoveRight);

	// We have 2 versions of the rotation bindings to handle different kinds of devices differently
	// "turn" handles devices that provide an absolute delta, such as a mouse.
	// "turnRate" is for devices that we choose to treat as a rate of change, such as an analog joystick
	PlayerInputComponent->BindAxis("Turn", this, &APawn::AddControllerYawInput);
	PlayerInputComponent->BindAxis("TurnRate", this, &AWatPlayerCharacter::TurnAtRate);
	PlayerInputComponent->BindAxis("LookUp", this, &APawn::AddControllerPitchInput);
	PlayerInputComponent->BindAxis("LookUpRate", this, &AWatPlayerCharacter::LookUpAtRate);

	//Actions

	PlayerInputComponent->BindAction("Interact", IE_Pressed, this, &AWatPlayerCharacter::Interact);
	PlayerInputComponent->BindAction("Distract", IE_Pressed, this, &AWatPlayerCharacter::OnDistract);
	PlayerInputComponent->BindAction("Sprint", IE_Pressed, this, &AWatPlayerCharacter::StartSprinting);
	PlayerInputComponent->BindAction("Sprint", IE_Released, this, &AWatPlayerCharacter::StopSprinting);
	PlayerInputComponent->BindAction("Crouch", IE_Pressed, this, &AWatPlayerCharacter::ToggleCrouch);
	PlayerInputComponent->BindAction("WatchListObjectives", IE_Pressed, this, &AWatPlayerCharacter::ToggleObjectivesHUDVisibility);

	//DEBUG
	PlayerInputComponent->BindAction("Jump", IE_Pressed, this, &ACharacter::Jump);
	PlayerInputComponent->BindAction("Jump", IE_Released, this, &ACharacter::StopJumping);
	PlayerInputComponent->BindAction("Debug", IE_Pressed, this, &AWatPlayerCharacter::CreateDebugWidget);
	PlayerInputComponent->BindAction("Pause", IE_Pressed, this, &AWatPlayerCharacter::TogglePauseMenu);
}

bool AWatPlayerCharacter::IsMechanist() const
{
	return bIsMechanist;
}

bool AWatPlayerCharacter::IsAlive() const
{
	return bIsAlive;
}

void AWatPlayerCharacter::Die()
{
	//TODO trigger die anim
	bIsAlive = false;
	DisableInput(nullptr);
}

void AWatPlayerCharacter::SetupStimulus()
{
	Stimulus = CreateDefaultSubobject<UAIPerceptionStimuliSourceComponent>(TEXT("Stimulus"));
	Stimulus->RegisterForSense(TSubclassOf<UAISense_Sight>());
	Stimulus->RegisterWithPerceptionSystem();
}

void AWatPlayerCharacter::OnDistract()
{
	if (DistractionSound)
	{
		FVector const Location = GetActorLocation();
		UGameplayStatics::PlaySoundAtLocation(GetWorld(), DistractionSound, Location);
		UAISense_Hearing::ReportNoiseEvent(GetWorld(), Location, 1.0f, this, 0.0f, AITags::NoiseTag);
	}
}

void AWatPlayerCharacter::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME(AWatPlayerCharacter, bIsMechanist);
	DOREPLIFETIME(AWatPlayerCharacter, bWantsToChangeCrouchState);
	DOREPLIFETIME(AWatPlayerCharacter, bIsCrouching);
}

void AWatPlayerCharacter::MoveForward(float Value)
{
	if (Value != 0.0f)
	{
		// add movement in that direction
		AddMovementInput(GetActorForwardVector(), Value);
	}
}

void AWatPlayerCharacter::MoveRight(float Value)
{
	if (Value != 0.0f)
	{
		// add movement in that direction
		AddMovementInput(GetActorRightVector(), Value);
	}
}

void AWatPlayerCharacter::TurnAtRate(float Rate)
{
	// calculate delta for this frame from the rate information
	AddControllerYawInput(Rate * BaseTurnRate * GetWorld()->GetDeltaSeconds());
}

void AWatPlayerCharacter::LookUpAtRate(float Rate)
{
	// calculate delta for this frame from the rate information
	AddControllerPitchInput(Rate * BaseLookUpRate * GetWorld()->GetDeltaSeconds());
}

void AWatPlayerCharacter::Interact_Implementation()
{
	auto* ClosestInteractable = GetClosestInteractable();
	if (ClosestInteractable)
	{
		ClosestInteractable->Interact();
	}
}

void AWatPlayerCharacter::StartSprinting()
{
	if(!bIsCrouching)
	{
		bIsSprinting = true;
	}
}

void AWatPlayerCharacter::StopSprinting()
{
	bIsSprinting = false;
}

void AWatPlayerCharacter::ToggleCrouch_Implementation()
{
	if(!bIsSprinting)
	{
		bWantsToChangeCrouchState = true;
	}
}

void AWatPlayerCharacter::ToggleObjectivesHUDVisibility()
{
	AWatPlayerController* PC = Cast<AWatPlayerController>(GetController());
	if (Cast<AWatGameState>(GetWorld()->GetGameState())->bGameStarted)
	{
		if (PC->ObjectivesHUDWidget->IsInViewport())
		{
			PC->ObjectivesHUDWidget->RemoveFromParent();
		}
		else
		{
			PC->ObjectivesHUDWidget->AddToViewport();
		}
	}
}
