#pragma once

#include "CoreMinimal.h"
#include "Interactables/BlendableInteractable.h"
#include "Objective.generated.h"

DECLARE_MULTICAST_DELEGATE_OneParam(FOnObjectiveInteractionEnd, ACharacter*);
typedef FOnObjectiveInteractionEnd::FDelegate FOnObjectiveInteractionEndDelegate;

UCLASS()
class WAT_API AObjective : public ABlendableInteractable
{
	GENERATED_BODY()

private:
	UPROPERTY()
	class AMainGameState* GameState;
	
    UFUNCTION(NetMulticast, Reliable)
    void EndGame();

public:

	UPROPERTY(Replicated)
	bool bCompleted;

    UPROPERTY(EditAnywhere)
    TSubclassOf<UUserWidget> EndGameWidgetTemplate;

	UPROPERTY(VisibleAnywhere)
	UWidgetComponent* MiniGameWidget;


	AObjective();
	
	virtual void BeginPlay() override;

	virtual void OnInteractionBegin() override;
	
	virtual void EndInteraction(bool bSuccess, bool bShouldWarnMonster);

private:
	virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;
};