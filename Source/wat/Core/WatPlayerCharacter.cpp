// Fill out your copyright notice in the Description page of Project Settings.

#include "wat/Core/WatPlayerCharacter.h"
#include "wat/GameplayActors/Laser.h"
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
#include "GameFramework/CharacterMovementComponent.h"
#include "Runtime/Engine/Classes/Engine/World.h"
#include "Perception/AIPerceptionStimuliSourceComponent.h"
#include "GameFramework/PlayerController.h"
#include "wat/Widgets/PauseWidget.h"
#include "wat/AI/GridManager.h"
#include "wat/AI/MonsterCharacter.h"
#include "wat/Core/WatPlayerController.h"
#include "wat/Widgets/MainHUDWidget.h"
#include "wat/Widgets/ObjectivesHUDWidget.h"

#if WITH_EDITOR
#include "wat/Core/WatGameInstance.h"
#endif

// Sets default values
AWatPlayerCharacter::AWatPlayerCharacter()
{
	PrimaryActorTick.bCanEverTick = true;

	// Create a CameraComponent	
	FirstPersonCameraComponent = CreateDefaultSubobject<UCameraComponent>(TEXT("FirstPersonCamera"));
	FirstPersonCameraComponent->SetupAttachment(RootComponent);
	FirstPersonCameraComponent->bUsePawnControlRotation = true;

	// Create a mesh component that will be used when being viewed from a '1st person' view (when controlling this pawn)
	Mesh1P = CreateDefaultSubobject<USkeletalMeshComponent>(TEXT("CharacterMesh1P"));
	Mesh1P->SetOnlyOwnerSee(true);
	Mesh1P->SetupAttachment(FirstPersonCameraComponent);
	Mesh1P->bCastDynamicShadow = false;
	Mesh1P->CastShadow = false;

	auto* Mesh3P = this->GetMesh();
	Mesh3P->SetVisibleFlag(false);

	SetupStimulus();
}

void AWatPlayerCharacter::BeginPlay()
{
	Super::BeginPlay();
	
	if (!IsLocallyControlled())
	{
		GetMesh()->SetVisibility(true);
		LocalPlayerController = Cast<AWatPlayerController>(GetWorld()->GetFirstPlayerController());
	}
	else
	{
		LocalPlayerController = Cast<AWatPlayerController>(GetController());
		if (HasAuthority())
		{
			bIsMechanist = true;
		}
	}

	auto* GameInstance = Cast<UWatGameInstance>(GetWorld()->GetGameInstance());
	if(GameInstance && GameInstance->LoadingScreenWidget)
	{
		GameInstance->LoadingScreenWidget->RemoveFromParent();
		GameInstance->LoadingScreenWidget = nullptr;
	}
	
	GameState = Cast<AWatGameState>(GetWorld()->GetGameState());

	if(HasAuthority())
	{
		if(GameInstance && GameInstance->bHostIsScientist)
		{
			if(IsLocallyControlled())
			{
				bIsMechanist = false;
			}
			else
			{
				bIsMechanist = true;
			}
		}
	}
	
	if(IsLocallyControlled() && GameState->bIsInLobby || GameState->bGameStarted || GameState->bIsInTutorialMode)
	{
		SetActorTickEnabled(true);
		LocalPlayerController->EnableInput(nullptr);
        LocalPlayerController->SetInputMode(FInputModeGameOnly());
		if(GameState->bIsInLobby)
		{
			bCrouchEnabled = true;
			bSprintEnabled = true;
			bObjectiveInputEnabled = false;
		}
		else if(GameState->bIsInTutorialMode && !GameState->bGameStarted)
		{
			bCrouchEnabled = false;
			bSprintEnabled = false;
			bObjectiveInputEnabled = false;
		}
		else
		{
			bCrouchEnabled = true;
			bSprintEnabled = true;
			bObjectiveInputEnabled = true;
		}

		if(!LocalPlayerController->MainHUDWidget)
		{
			LocalPlayerController->MainHUDWidget = CreateWidget<UMainHUDWidget>(LocalPlayerController, MainHUDWidgetClass);
			if(LocalPlayerController->MainHUDWidget)
			{
				LocalPlayerController->MainHUDWidget->AddToViewport();
				LocalPlayerController->MainHUDWidget->ParentCharacter = this;
			}
		}
	}
	else
	{
		SetActorTickEnabled(false);
	}
	
	bIsAlive = true;

	if (!OriginalMaxSpeed)
	{
		OriginalMaxSpeed = GetCharacterMovement()->MaxWalkSpeed;
	}
	
	BaseCameraRelativeHeight = FirstPersonCameraComponent->GetRelativeLocation().Z;
	BaseHalfHeightDelta = 0.f;
	CurrentHalfHeightDelta = 0.f;

	LaserDeltaTimeCount = 0.f;

	ARay = static_cast<float>(MaxRangToKeep - MinRangToKeep) / (OriginalMaxSpeed - (OriginalMaxSpeed + SprintAdditionalSpeedValue));
	BRay = static_cast<float>(MinRangToKeep) - ARay * (OriginalMaxSpeed + SprintAdditionalSpeedValue);

	ARange = static_cast<float>(MinRangFromMonster - MaxRangFromMonster) / (OriginalMaxSpeed - (OriginalMaxSpeed + SprintAdditionalSpeedValue));
	BRange = static_cast<float>(MaxRangFromMonster) - ARange * (OriginalMaxSpeed + SprintAdditionalSpeedValue);
}

void AWatPlayerCharacter::EndPlay(const EEndPlayReason::Type EndPlayReason)
{
	if(LocalPlayerController)
	{
		if(LocalPlayerController->ObjectivesHUDWidget)
		{
			LocalPlayerController->ObjectivesHUDWidget->RemoveFromParent();
			LocalPlayerController->ObjectivesHUDWidget = nullptr;
		}
		else if(LocalPlayerController->MainHUDWidget)
		{
			LocalPlayerController->MainHUDWidget->RemoveFromParent();
			LocalPlayerController->MainHUDWidget = nullptr;
		}
	}
}

void AWatPlayerCharacter::Tick(const float DeltaTime)
{
	Super::Tick(DeltaTime);
#if WITH_EDITOR
	if (IsLocallyControlled())
	{
		GEngine->AddOnScreenDebugMessage(-1, 0.f, FColor::Red,
			Cast<UWatGameInstance>(GetGameInstance())->bIsHost ? L"HOST" : L"CLIENT");
		GEngine->AddOnScreenDebugMessage(-1, 0.f, FColor::Blue, bIsMechanist ? L"MECHANIST" : L"SCIENTIST");
		if (GameState)
		{
			GEngine->AddOnScreenDebugMessage(-1, 0.f, FColor::Purple, FString("Start mode: ") + (GameState->bTutorialActivated ? FString("Tutorial") : FString("Quick Start")));
		}
	}
#endif

	if (IsLocallyControlled())
	{
		if (GameState && GameState->bIsInTutorialMode || GameState->bGameStarted)
		{
			if (!LocalPlayerController->ObjectivesHUDWidget)
			{
				CreateObjectiveList();
			}

			HandleLaser(DeltaTime);
			
			if (!GetCharacterMovement()->IsCrouching())
			{
				ClearGridWhenSprinting();
			}
		}
		HandleInteractables();
		HandleSprint(DeltaTime);
		HandleCrouch(DeltaTime);
	}

}

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
	//PlayerInputComponent->BindAction("Distract", IE_Pressed, this, &AWatPlayerCharacter::OnDistract);
	PlayerInputComponent->BindAction("Sprint", IE_Pressed, this, &AWatPlayerCharacter::NotifyWantsToSprint);
	PlayerInputComponent->BindAction("Sprint", IE_Released, this, &AWatPlayerCharacter::StopSprinting);
	PlayerInputComponent->BindAction("Crouch", IE_Pressed, this, &AWatPlayerCharacter::ToggleCrouch);
	PlayerInputComponent->BindAction("WatchListObjectives", IE_Pressed, this, &AWatPlayerCharacter::ToggleObjectivesHUDVisibility);

	//DEBUG
	PlayerInputComponent->BindAction("Jump", IE_Pressed, this, &ACharacter::Jump);
	PlayerInputComponent->BindAction("Jump", IE_Released, this, &ACharacter::StopJumping);
	PlayerInputComponent->BindAction("Debug", IE_Pressed, this, &AWatPlayerCharacter::CreateDebugWidget);
	PlayerInputComponent->BindAction("Pause", IE_Pressed, this, &AWatPlayerCharacter::TogglePauseMenu);
}

void AWatPlayerCharacter::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME(AWatPlayerCharacter, bIsMechanist);
	DOREPLIFETIME(AWatPlayerCharacter, bIsSprinting);
	DOREPLIFETIME(AWatPlayerCharacter, LaserPointer);
}

void AWatPlayerCharacter::SwitchStartMode_Implementation()
{
	GameState->bTutorialActivated = !GameState->bTutorialActivated;
	auto* GameInstance = Cast<UWatGameInstance>(GetWorld()->GetGameInstance());
	if(GameInstance)
	{
		GameInstance->bIsInTutorialMode = !GameInstance->bIsInTutorialMode;
	}
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

void AWatPlayerCharacter::UpdateRemotePlayerSpeed_Implementation(AWatPlayerCharacter* Player, float NewSpeed)
{
	Player->GetCharacterMovement()->MaxWalkSpeed = NewSpeed;
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
	//Adding 0.01 to SensibilityModifier in case player chose 0
	AddControllerYawInput(Rate * BaseTurnRate * (SensibilityModifier + 0.01) * GetWorld()->GetDeltaSeconds());
}

void AWatPlayerCharacter::LookUpAtRate(float Rate)
{
	// calculate delta for this frame from the rate information
	if (bInvertYAxis)
	{
		//Adding 0.01 to SensibilityModifier in case player chose 0
		AddControllerPitchInput(-(Rate * BaseLookUpRate * (SensibilityModifier + 0.01) * GetWorld()->GetDeltaSeconds()));
	} else
	{
		//Adding 0.01 to SensibilityModifier in case player chose 0
		AddControllerPitchInput(Rate * BaseLookUpRate * (SensibilityModifier + 0.01) * GetWorld()->GetDeltaSeconds());
	}
}

void AWatPlayerCharacter::HandleSprint(const float DeltaTime)
{
	auto* LocalCharacterMovement = GetCharacterMovement();

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
		LocalPlayerController->PlayerCameraManager->PlayCameraShake(SprintShake,
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
}

void AWatPlayerCharacter::NotifyWantsToSprint()
{
	if(bSprintEnabled)
	{
		if(GetCharacterMovement()->IsCrouching())
		{
			UnCrouch();
		}
		StartSprinting();
	}
}

void AWatPlayerCharacter::StartSprinting_Implementation()
{
	bIsSprinting = true;
}

void AWatPlayerCharacter::StopSprinting_Implementation()
{
	bIsSprinting = false;
}


void AWatPlayerCharacter::ToggleCrouch()
{
	if(bCrouchEnabled)
	{
		if(bIsSprinting)
		{
			StopSprinting();
		}
		if(!GetCharacterMovement()->IsCrouching())
		{
			Crouch();
		}
		else
		{
			UnCrouch();
		}
	}
}

void AWatPlayerCharacter::OnStartCrouch(float HalfHeightAdjust, float ScaledHalfHeightAdjust)
{
	Super::OnStartCrouch(HalfHeightAdjust, ScaledHalfHeightAdjust);
	BaseHalfHeightDelta = ScaledHalfHeightAdjust;
	CurrentHalfHeightDelta = CurrentHalfHeightDelta == 0.f ? ScaledHalfHeightAdjust : CurrentHalfHeightDelta + ScaledHalfHeightAdjust;
}

void AWatPlayerCharacter::OnEndCrouch(float HalfHeightAdjust, float ScaledHalfHeightAdjust)
{
	Super::OnEndCrouch(HalfHeightAdjust, ScaledHalfHeightAdjust);
	BaseHalfHeightDelta = ScaledHalfHeightAdjust;
	CurrentHalfHeightDelta = CurrentHalfHeightDelta == 0.f ? -ScaledHalfHeightAdjust : CurrentHalfHeightDelta - ScaledHalfHeightAdjust;
}

void AWatPlayerCharacter::HandleCrouch(const float DeltaTime)
{
	auto RelativeLocation = FirstPersonCameraComponent->GetRelativeLocation();
	if(CurrentHalfHeightDelta != 0.f)
	{
		const auto Sign = CurrentHalfHeightDelta > 0.f ? -1.f : 1.f;
		CurrentHalfHeightDelta += (BaseHalfHeightDelta * Sign * DeltaTime) / CrouchingTransitionDuration;
		
		if((Sign == -1.f  && CurrentHalfHeightDelta < 0.f) || (Sign == 1.f  && CurrentHalfHeightDelta > 0.f))
		{
			CurrentHalfHeightDelta = 0.f;
		}

		RelativeLocation.Z = BaseCameraRelativeHeight + CurrentHalfHeightDelta;
		FirstPersonCameraComponent->SetRelativeLocation(RelativeLocation);
	}
}

void AWatPlayerCharacter::AddInteractable_Implementation(AInteractable* Interactable)
{
	if(HasAuthority() || IsLocallyControlled())
	{
		InteractablesSet.Add(Interactable);
	}
}

void AWatPlayerCharacter::RemoveInteractable_Implementation(AInteractable* Interactable)
{
	if(HasAuthority() || IsLocallyControlled())
	{
		InteractablesSet.Remove(Interactable);
		auto UserWidget = Interactable->KeyIconWidget->GetUserWidgetObject();
		if(UserWidget)
		{
			UserWidget->SetVisibility(ESlateVisibility::Hidden);
		}
	}
}

bool AWatPlayerCharacter::IsClosestInteractable(AInteractable* Interactable)
{
	if (GetClosestInteractable() == Interactable)
	{
		return true;
	}

	return false;
}

void AWatPlayerCharacter::Interact_Implementation()
{
	auto* ClosestInteractable = GetClosestInteractable();
	if (ClosestInteractable)
	{
		bIsSprinting = false;
		GetCharacterMovement()->MaxWalkSpeed = OriginalMaxSpeed;
		ClosestInteractable->Interact(this);
	}
}

void AWatPlayerCharacter::HandleInteractables()
{
	auto* ClosestInteractable = GetClosestInteractable();
	if(ClosestInteractable)
	{
		if(!ClosestInteractable->KeyIconWidget->GetUserWidgetObject()->IsVisible())
		{
			ClosestInteractable->KeyIconWidget->GetUserWidgetObject()->SetVisibility(ESlateVisibility::Visible);
		}
		ClosestInteractable->RefreshIconWidgetForCurrentDevice(LocalPlayerController->bIsUsingController);
		ClosestInteractable->KeyIconWidget->SetWorldRotation((GetActorForwardVector() * -1.f).Rotation());
	}
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

void AWatPlayerCharacter::SetIsUsingLaser(const bool bInUse)
{
	bIsUsingLaser = bInUse;
}

void AWatPlayerCharacter::UpdateLaserPosition_Implementation(AWatPlayerCharacter* OwningPlayer, FVector Location, FRotator Rotation)
{
	if(!LaserPointer)
	{
		LaserPointer = GetWorld()->SpawnActor<ALaser>(LaserPointerClass, Location, Rotation);
		if(OwningPlayer->IsLocallyControlled())
		{
			LaserPointer->LaserMesh->SetMaterial(0, LaserPointer->PlayerOneMaterial);
		}
		else
		{
			LaserPointer->LaserMesh->SetMaterial(0, LaserPointer->PlayerTwoMaterial);
		}
	}
	
	LaserPointer->SetActorHiddenInGame(false);
	LaserPointer->SetActorLocation(Location);
	LaserPointer->SetActorRelativeRotation(Rotation);
}

void AWatPlayerCharacter::HideLaser_Implementation()
{
	LaserPointer->SetActorHiddenInGame(true);
}

void AWatPlayerCharacter::SetLaserColor()
{
	if(IsLocallyControlled())
	{
		LaserPointer->LaserMesh->SetMaterial(0, LaserPointer->PlayerTwoMaterial);
	}
	else
	{
		LaserPointer->LaserMesh->SetMaterial(0, LaserPointer->PlayerOneMaterial);
	}
}

void AWatPlayerCharacter::HandleLaser(float DeltaTime)
{
	if (bIsUsingLaser)
	{
		LaserDeltaTimeCount += DeltaTime;
		
		FHitResult Result;
		const FVector Start = FirstPersonCameraComponent->GetComponentLocation();
		const FVector ForwardVector = FirstPersonCameraComponent->GetForwardVector();
		const FVector End = ((ForwardVector * 1000.f) + Start);
		FCollisionObjectQueryParams ObjectQueryParams;
		ObjectQueryParams.AddObjectTypesToQuery(ECollisionChannel::ECC_GameTraceChannel1);

		if (GetWorld()->LineTraceSingleByObjectType(Result, Start, End, ObjectQueryParams))
		{
			if (Result.bBlockingHit)
			{
				if(!HasAuthority() && LaserPointer)
				{
					LaserPointer->SetActorHiddenInGame(false);
					LaserPointer->SetActorLocation(Result.ImpactPoint + Result.Normal);
					LaserPointer->SetActorRelativeRotation(Result.Actor->GetActorRotation());
				}
				if(!HasAuthority() && LaserDeltaTimeCount >= LaserRefreshPeriod)
				{
					LaserDeltaTimeCount = 0.f;
					UpdateLaserPosition(this, Result.ImpactPoint + Result.Normal, Result.Actor->GetActorRotation());
				}
				else if(HasAuthority())
				{
					UpdateLaserPosition(this, Result.ImpactPoint + Result.Normal, Result.Actor->GetActorRotation());
				}
			}
		}
	}
	else
	{
		if (LaserPointer)
		{
			LaserPointer->SetActorHiddenInGame(true);
			HideLaser();
		}
	}
}

void AWatPlayerCharacter::ClearGridWhenSprinting_Implementation()
{
	const int Range = FMath::RoundToInt(ARange * GetCharacterMovement()->MaxWalkSpeed + BRange);

	if (GameState->GridManager && GetDistanceTo(GameState->MonsterCharacter) <= Range)
	{
		const int Ray = FMath::RoundToInt(ARay * GetCharacterMovement()->MaxWalkSpeed + BRay);

		GameState->GridManager->SetValueOutside(GetActorLocation(), Ray);
		GameState->GridManager->SetValueAround(GetActorLocation(), Ray);
	}
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

			if(PlayerCharacter->IsLocallyControlled() && !PlayerCharacter->bIsMechanist || !PlayerCharacter->IsLocallyControlled() && PlayerCharacter->bIsMechanist)
			{
				auto* GameInstance = Cast<UWatGameInstance>(GetWorld()->GetGameInstance());
				GameInstance->bHostIsScientist = true;
			}
		}
	}
}

bool AWatPlayerCharacter::IsMechanist() const
{
	return bIsMechanist;
}

void AWatPlayerCharacter::CreateObjectiveList()
{
	LocalPlayerController->ObjectivesHUDWidget = CreateWidget<UObjectivesHUDWidget>(LocalPlayerController, ObjectivesHUDWidgetClass);
	LocalPlayerController->ObjectivesHUDWidget->Parent = this;
	LocalPlayerController->ObjectivesHUDWidget->Controller = LocalPlayerController;
	LocalPlayerController->ObjectivesHUDWidget->UpdateObjectiveList();
}

void AWatPlayerCharacter::ToggleObjectivesHUDVisibility()
{
	if(bObjectiveInputEnabled)
	{
		
		if (GameState && GameState->bGameStarted || GameState->bIsInTutorialMode)
		{
			if (LocalPlayerController->ObjectivesHUDWidget->IsInViewport())
			{
				LocalPlayerController->ObjectivesHUDWidget->RemoveFromParent();
			}
			else
			{
				LocalPlayerController->ObjectivesHUDWidget->AddToViewport();
			}
		}
	}
}


void AWatPlayerCharacter::TogglePauseMenu()
{
	if (!LocalPlayerController->PauseWidget)
	{
		LocalPlayerController->PauseWidget = CreateWidget<UPauseWidget>(LocalPlayerController, PauseWidget);
		if (LocalPlayerController->PauseWidget)
		{
			LocalPlayerController->PauseWidget->AddToViewport();
			LocalPlayerController->bShowMouseCursor = true;
			const FInputModeUIOnly UIOnly;
			LocalPlayerController->SetInputMode(UIOnly);
		}
	}
}


void AWatPlayerCharacter::CreateDebugWidget()
{
	if (!LocalPlayerController->DebugGridWidget)
	{
		LocalPlayerController->DebugGridWidget = CreateWidget<UUserWidget>(LocalPlayerController, GridDebugWidget);
		LocalPlayerController->DebugGridWidget->AddToViewport();
	}
	else
	{
		LocalPlayerController->DebugGridWidget->RemoveFromParent();
		LocalPlayerController->DebugGridWidget = nullptr;
	}
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