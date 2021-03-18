#pragma once

#include "CoreMinimal.h"

#include "Sound/SoundCue.h"
#include "wat/GameplayActors/Interactables/BlendableInteractable.h"
#include "Objective.generated.h"

UCLASS()
class WAT_API AObjective : public ABlendableInteractable
{
	GENERATED_BODY()

public:

	UPROPERTY()
	class AWatGameState* GameState;
	
	UPROPERTY(Replicated)
	bool bCompleted;

	UPROPERTY(EditAnywhere, Category = "WAT Objectives")
	TSubclassOf<class UMiniGameWidget> MiniGameWidgetClass;

    UPROPERTY(EditAnywhere, Category = "WAT Objectives")
    int ObjectiveIndex;

	UPROPERTY(EditAnywhere, Category = "WAT Objectives")
	USoundCue* EndInteractionSound;
	
    UPROPERTY(Replicated, EditAnywhere, Category = "WAT Objectives")
    bool bIsOnlyForMechanist;

	AObjective();

	virtual bool CanInteract(class AWatPlayerCharacter* Player) override;

	virtual void BeginPlay() override;

	virtual void Tick(float DeltaSeconds) override;
	
	virtual void EndInteraction(bool bSuccess, bool bShouldWarnMonster);

	virtual void OnNotFinalBlend() override;

	virtual void OnFinalBlend() override;

	virtual int GetStencilIndex() override;
	
	// Called to bind functionality to input
	virtual void SetupPlayerInputComponent(class UInputComponent* DeskInputComponent) override;

	float XVelocity;

	float YVelocity;
	
private:
	void AddXVelocity(float Value);

	void AddYVelocity(float Value);
	
	virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;
};