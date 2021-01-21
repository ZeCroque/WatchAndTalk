#pragma once

#include "CoreMinimal.h"
#include "Interactables/Interactable.h"
#include "Objective.generated.h"

DECLARE_MULTICAST_DELEGATE_OneParam(FOnObjectiveInteractionEnd, ACharacter*);
typedef FOnObjectiveInteractionEnd::FDelegate FOnObjectiveInteractionEndDelegate;

UCLASS()
class WAT_API AObjective : public AInteractable
{
	GENERATED_BODY()

private:
	UPROPERTY()
	class AMainGameState* GameState;
	
    UFUNCTION(NetMulticast, Reliable)
    void EndGame();

protected:
	FOnObjectiveInteractionEndDelegate OnObjectiveInteractionEnd;
	
	virtual void Interact() override;

public:
	AObjective();

	UPROPERTY(Replicated)
	bool bCompleted;

    UPROPERTY(EditAnywhere)
    TSubclassOf<UUserWidget> EndGameWidgetTemplate;

	virtual void BeginPlay() override;
	
	virtual void InteractionEnd();
	bool bCanInteract = true;

private:
	virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;
};