#pragma once

#include "CoreMinimal.h"
#include "wat/GameplayActors/Interactables/BlendableInteractable.h"
#include "Objective.generated.h"

DECLARE_MULTICAST_DELEGATE_OneParam(FOnObjectiveInteractionEnd, ACharacter*);
typedef FOnObjectiveInteractionEnd::FDelegate FOnObjectiveInteractionEndDelegate;

UCLASS()
class WAT_API AObjective : public ABlendableInteractable
{
	GENERATED_BODY()

public:

	UPROPERTY()
	class AWatGameState* GameState;
	
	UPROPERTY(Replicated)
	bool bCompleted;

	UPROPERTY(Replicated, VisibleAnywhere)
	UWidgetComponent* MiniGameWidgetComponent;

	UPROPERTY(Replicated)
	class UQTEWidget* MiniGameWidget;

    UPROPERTY(EditAnywhere, Category = "WAT Objectives")
    int ObjectiveIndex;

    UPROPERTY(EditAnywhere, Category = "WAT Objectives")
    bool bIsOnlyForMechanist;

	AObjective();

	virtual void OnInteractionBegin() override;

    virtual void OnInteraction() override;

	virtual void BeginPlay() override;
	
	virtual void EndInteraction(bool bSuccess, bool bShouldWarnMonster);

private:
	virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;
};