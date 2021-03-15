// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"


#include "GameFramework/Character.h"
#include "Perception/AIPerceptionStimuliSourceComponent.h"

#include "WatPlayerCharacter.generated.h"

DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnInteract, class AInteractable*, InteractedObject);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnInteractionEnd, class AInteractable*, InteractedObject);

UCLASS()
class WAT_API AWatPlayerCharacter : public ACharacter
{
	GENERATED_BODY()

//ACTOR
//=====
	
public:
	UPROPERTY(VisibleDefaultsOnly)
	class UCameraComponent* FirstPersonCameraComponent = nullptr;

	UPROPERTY(EditAnywhere)
	class USkeletalMeshComponent* Mesh1P = nullptr;
	
protected:
	AWatPlayerCharacter();
	virtual void BeginPlay() override;
	virtual void EndPlay(const EEndPlayReason::Type EndPlayReason) override;
	virtual void Tick(float DeltaTime) override;
	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;
	virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;

//UTILITY
//=======
private:
	UPROPERTY()
	class AWatPlayerController* LocalPlayerController;

	UPROPERTY()
	class AWatGameState* GameState;

//NETWORK
//=======
public:
	UFUNCTION(Server, Reliable)
	void SwitchStartMode();
	
private:
	UFUNCTION(BlueprintCallable, Category = "WAT Network")
	void SetPlayerAsReadyForRestart();

	UFUNCTION(Server, Reliable)
	void RestartGame(uint32 PlayerId);

//GAMEPLAY
//========
public:
	bool IsAlive() const;
	void Die();

private:
	UPROPERTY(BlueprintReadOnly, Meta=(AllowPrivateAccess = "true"))
	bool bIsAlive;
	
//MOVEMENT
//========
//	DISPLACEMENTS
//	-------------
private:
	float OriginalMaxSpeed;

	UFUNCTION(Server, Reliable)
	void UpdateRemotePlayerSpeed(AWatPlayerCharacter* Player, float NewSpeed);
	
	void MoveForward(float Value);
	void MoveRight(float Value);


//	CAMERA
//	------
public:
	UPROPERTY(EditAnywhere, Category = "WAT Camera")
	float BaseTurnRate;

	UPROPERTY(EditAnywhere, Category = "WAT Camera")
	float BaseLookUpRate;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "WAT Gameplay Options")
	bool bInvertYAxis;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "WAT Gameplay Options")
	float SensibilityModifier;
private:
	void TurnAtRate(float Rate);
	void LookUpAtRate(float Rate);

//	SPRINT
//	------
public:
	UPROPERTY(EditAnywhere, Category = "WAT Sprint")
	float SprintAdditionalSpeedValue;

	UPROPERTY(EditAnywhere, Category = "WAT Sprint")
	float SprintSpeedIncreaseRate;

	UPROPERTY(EditAnywhere, Category = "WAT Sprint")
	float SprintSpeedDecreaseRate;

	UPROPERTY(EditAnywhere, Category= "WAT Sprint")
	TSubclassOf<UCameraShake> SprintShake;

	bool bSprintEnabled;
private:
	UPROPERTY(Replicated)
	bool bIsSprinting;
	
	void HandleSprint(float DeltaTime);

	void NotifyWantsToSprint();
	
	UFUNCTION(Server, Reliable)
	void StartSprinting();

	UFUNCTION(Server, Reliable)
	void StopSprinting();

//	CROUCH
//	------
public:
	UPROPERTY(EditAnywhere, Category = "WAT Crouch")
	float CrouchingTransitionDuration = 0.5f;

	bool bCrouchEnabled;
private:	
	float BaseCameraRelativeHeight;
	float BaseHalfHeightDelta;
	float CurrentHalfHeightDelta;

	void ToggleCrouch();
	virtual void OnStartCrouch(float HalfHeightAdjust, float ScaledHalfHeightAdjust) override;
	virtual void OnEndCrouch(float HalfHeightAdjust, float ScaledHalfHeightAdjust) override;
	
	void HandleCrouch(float DeltaTime);
	
//INTERACTIONS
//============
public:
	UPROPERTY(BlueprintAssignable, meta = (DisplayName = "OnInteractionBegin"))
	FOnInteract OnInteractDispatcher;

	UPROPERTY(BlueprintAssignable, meta = (DisplayName = "OnInteractionEnd"))
	FOnInteract OnInteractionEndDispatcher;

	UFUNCTION(NetMulticast, Reliable)
	void AddInteractable(AInteractable* Interactable);

	UFUNCTION(NetMulticast, Reliable)
	void RemoveInteractable(AInteractable* Interactable);

	bool IsClosestInteractable(AInteractable* Interactable);
	
private:
	UPROPERTY()
	TSet<AInteractable*> InteractablesSet;

	UFUNCTION(Server, Reliable)
	void Interact();

	void HandleInteractables();
	
	AInteractable* GetClosestInteractable();

//LASER
//=====
public:
	UPROPERTY(EditAnywhere, Category="WAT Laser")
	TSubclassOf<class ALaser> LaserPointerClass;

	UPROPERTY(EditAnywhere, Category="WAT Laser")
	float LaserRefreshPeriod = 0.1f;
	
	void SetIsUsingLaser(bool bInUse);

private:
	bool bIsUsingLaser;
	float LaserDeltaTimeCount;

	
	UPROPERTY(ReplicatedUsing=SetLaserColor)
	class ALaser* LaserPointer;

	UFUNCTION(Server, Reliable)
	void UpdateLaserPosition(AWatPlayerCharacter* OwningPlayer, FVector Location, FRotator Rotation);

	UFUNCTION(Server, Reliable)
	void HideLaser();

	UFUNCTION()
	void SetLaserColor();
	
	void HandleLaser(float DeltaTime);
//GRID
//======
public:
	UPROPERTY(EditAnywhere, Category = "WAT Grid")
	int MinRangToKeep = 4;

	UPROPERTY(EditAnywhere, Category = "WAT Grid")
	int MaxRangToKeep = 10;

	UPROPERTY(EditAnywhere, Category = "WAT Grid")
	int MinRangFromMonster = 500;

	UPROPERTY(EditAnywhere, Category = "WAT Grid")
	int MaxRangFromMonster = 1000;

private:
	//Coeff for affine function used to get the range to keep when cleaning the grid during sprint
	float ARay;
	float BRay;

	//Coeff for affine function used to get the range to fill when cleaning the grid during sprint
	float ARange;
	float BRange;

	UFUNCTION(Server, Reliable)
	void ClearGridWhenSprinting();

//OBJECTIVES
//==========
public:
	UPROPERTY(EditAnywhere, Category="WAT Objectives")
	TSubclassOf<UUserWidget> ObjectivesHUDWidgetClass;

	bool bObjectiveInputEnabled;
	
	UFUNCTION(Server, Reliable)
	void SwitchPlayersClasses();
	
	bool IsMechanist() const;
private:
	UPROPERTY(Replicated)
	bool bIsMechanist;

	void CreateObjectiveList();
	void ToggleObjectivesHUDVisibility();

//PAUSE
//=====
public:
	UPROPERTY(EditAnywhere, meta=(AllowPrivateAccess = "true"))
	TSubclassOf<UUserWidget> PauseWidget;

private:
	UFUNCTION()
	void TogglePauseMenu();
	
//DEBUG
//=====
public:
	UPROPERTY(EditAnywhere, Category="WAT Debug")
	TSubclassOf<UUserWidget> GridDebugWidget;

	UPROPERTY(EditAnywhere, Category="WAT Debug")
	USoundBase* DistractionSound;
	
private:
	UPROPERTY()
	class UAIPerceptionStimuliSourceComponent* Stimulus;
	
	void CreateDebugWidget();
	void SetupStimulus();
	void OnDistract();


};
