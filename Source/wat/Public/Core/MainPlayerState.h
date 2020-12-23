#pragma once

#include "CoreMinimal.h"
#include "GameFramework/PlayerState.h"
#include "MainPlayerState.generated.h"

UCLASS()
class WAT_API AMainPlayerState final : public APlayerState
{
    GENERATED_BODY()
    
public:

    virtual void OnDeactivated() override;
};
