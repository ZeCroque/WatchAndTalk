#pragma once

#include "CoreMinimal.h"
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
	
    UPROPERTY(Replicated, EditAnywhere, Category = "WAT Objectives")
    bool bIsOnlyForMechanist;

	AObjective();

	virtual bool CanInteract(class AWatPlayerCharacter* Player) override;

	virtual void BeginPlay() override;
	
	virtual void EndInteraction(bool bSuccess, bool bShouldWarnMonster);

	virtual void OnNotFinalBlend() override;

	virtual void OnFinalBlend() override;

	virtual int GetStencilIndex() override;

private:
	virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;
};