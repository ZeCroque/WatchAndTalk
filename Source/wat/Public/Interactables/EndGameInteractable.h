#pragma once

#include "CoreMinimal.h"
#include "Interactables/Interactable.h"
#include "EndGameInteractable.generated.h"

UCLASS()
class WAT_API AEndGameInteractable : public AInteractable
{
	GENERATED_BODY()

protected:
	virtual void Interact() override;

public:
	UPROPERTY(Replicated)
	bool bUsed = false;

private:
	virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;
};