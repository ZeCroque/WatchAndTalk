// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"


#include "GameFramework/Character.h"
#include "Perception/AIPerceptionStimuliSourceComponent.h"

#include "WatPlayerCharacter.generated.h"

DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnInteract, class AInteractable*, InteractedObject);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnInteractionEnd, class AInteractable*, InteractedObject);

class AWatGameState;
class AGridManager;
class AInteractable;

UCLASS()
class WAT_API AWatPlayerCharacter : public ACharacter
{
	GENERATED_BODY()

public:
	// Sets default values for this character's properties
	AWatPlayerCharacter();

	UPROPERTY(VisibleDefaultsOnly)
	class UCameraComponent* FirstPersonCameraComponent = nullptr;

	UPROPERTY(EditAnywhere)
	class USkeletalMeshComponent* Mesh1P = nullptr;

	/** Base turn rate, in deg/sec. Other scaling may affect final turn rate. */
	UPROPERTY(EditAnywhere, Category = Camera)
	float BaseTurnRate;

	/** Base look up/down rate, in deg/sec. Other scaling may affect final rate. */
	UPROPERTY(EditAnywhere, Category = Camera)
	float BaseLookUpRate;

	UPROPERTY(EditAnywhere)
	float SprintAdditionalSpeedValue;

	UPROPERTY(EditAnywhere)
	float SprintSpeedIncreaseRate;

	UPROPERTY(EditAnywhere)
	float SprintSpeedDecreaseRate;

	UPROPERTY()
	float OriginalMaxSpeed;

	UPROPERTY(BlueprintAssignable, meta = (DisplayName = "OnInteractionBegin"))
	FOnInteract OnInteractDispatcher;

	UPROPERTY(BlueprintAssignable, meta = (DisplayName = "OnInteractionEnd"))
	FOnInteract OnInteractionEndDispatcher;

	bool AddInteractable(AInteractable* Interactable);
	void RemoveInteractable(AInteractable* Interactable);
	AInteractable* GetClosestInteractable();
	bool IsClosestInteractable(AInteractable* Interactable);

	UPROPERTY()
	bool bIsUsingLaser;

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

	//Inputs

	void CreateDebugWidget();

	//Movement
	void MoveForward(float Value);
	void MoveRight(float Value);
	void TurnAtRate(float Rate);
	void LookUpAtRate(float Rate);

	//Special displacements
	void StartSprinting();
	void StopSprinting();
	void StartCrouching();
	void StopCrouching();
	void ToggleObjectivesHUDVisibility();


	//Interactables
	UFUNCTION(Server, Reliable)
	void Interact();

	//GridManager
	void ClearGridWhenSprinting();

	UFUNCTION(BlueprintCallable, Category = "Network|Test")
	void SetPlayerAsReadyForRestart();

	UFUNCTION(Server, Reliable)
	void RestartGame(uint32 PlayerId);

	UFUNCTION()
	void TogglePauseMenu();

	UFUNCTION(Server, Reliable)
	void UpdateRemotePlayerSpeed(AWatPlayerCharacter* Player, float NewSpeed);

public:
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	// Called to bind functionality to input
	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;

	UFUNCTION(Server, Reliable)
	void SwitchPlayersClasses();

	UFUNCTION(Server, Reliable)
	void SwitchStartMode();

	bool IsMechanist() const;

	bool IsAlive() const;

	void Die();

private:

	bool bIsAlive;
	bool bIsSprinting;

	UPROPERTY(Replicated)
	bool bIsMechanist;

	UPROPERTY()
	class UAIPerceptionStimuliSourceComponent* Stimulus;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="WAT Sound", meta=(AllowPrivateAccess = "true"))
	USoundBase* DistractionSound;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta=(AllowPrivateAccess = "true"))
	TSubclassOf<UUserWidget> GridDebugWidget;

	UPROPERTY(EditAnywhere, Category="WAT HUD")
	TSubclassOf<UUserWidget> ObjectivesHUDWidgetClass;

	UPROPERTY()
	class ALaser* LaserPointer;

	UPROPERTY(EditAnywhere)
	TSubclassOf<ALaser> LaserPointerClass;

	UPROPERTY()
	TSet<AInteractable*> InteractablesSet;


	void SetupStimulus();

	void OnDistract();

	UPROPERTY(EditAnywhere, Category= "WAT Sprint")
	TSubclassOf<UCameraShake> SprintShake;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "WAT - Setup Clean Grid", meta = (AllowPrivateAccess = "true"
	))
	int MinRangToKeep = 4;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "WAT - Setup Clean Grid", meta = (AllowPrivateAccess = "true"
	))
	int MaxRangToKeep = 10;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "WAT - Setup Clean Grid", meta = (AllowPrivateAccess = "true"
	))
	int MinRangFromMonster = 500;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "WAT - Setup Clean Grid", meta = (AllowPrivateAccess = "true"
	))
	int MaxRangFromMonster = 1000;

	UPROPERTY()
	AWatGameState* GameState;

	//Coeff for affine function  used to get the range to keep when cleaning the grid during sprint
	float ARay;
	float BRay;

	float ARange;
	float BRange;


	UPROPERTY(EditAnywhere, meta=(AllowPrivateAccess = "true"))
	TSubclassOf<UUserWidget> PauseWidget;

	virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;
};
