// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "BehaviorTree/Services/BTService_BlackboardBase.h"
#include "ChangeSpeedBTS.generated.h"

/**
 * 
 */
UCLASS()
class WAT_API UChangeSpeedBTS final : public UBTService_BlackboardBase
{
    GENERATED_BODY()

public:
    UChangeSpeedBTS();
    virtual void OnBecomeRelevant(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory) override;

protected:
    virtual FString GetStaticServiceDescription() const override;
    
private:
    UPROPERTY(EditAnywhere, BlueprintReadWrite,Category= "AI", meta=(AllowPrivateAccess = "true"))
    float Speed = 600;
};
