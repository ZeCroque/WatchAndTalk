#pragma once

#include "CoreMinimal.h"
#include "GameFramework/PlayerState.h"
#include "WatPlayerState.generated.h"

UCLASS()
class WAT_API AWatPlayerState final : public APlayerState
{
    GENERATED_BODY()
    
public:

    virtual void OnDeactivated() override;
};
