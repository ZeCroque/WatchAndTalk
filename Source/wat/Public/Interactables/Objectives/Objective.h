#pragma once

#include "CoreMinimal.h"
#include "Interactables/Interactable.h"
#include "Objective.generated.h"

UCLASS()
class WAT_API AObjective : public AInteractable
{
	GENERATED_BODY()

private:
        UFUNCTION(NetMulticast, Reliable)
        void EndGame();

protected:
	virtual void Interact() override;

public:
	UPROPERTY(Replicated)
	bool bUsed = false;

    UPROPERTY(EditAnywhere)
    TSubclassOf<UUserWidget> EndGameWidgetTemplate;

private:
	virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;
};