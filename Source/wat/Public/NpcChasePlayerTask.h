// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "BehaviorTree/Tasks/BTTask_BlackboardBase.h"
#include "NpcChasePlayerTask.generated.h"

/**
 * 
 */
UCLASS()
class WAT_API UNpcChasePlayerTask final : public UBTTask_BlackboardBase
{
    GENERATED_BODY()
public:
    explicit UNpcChasePlayerTask(FObjectInitializer const& ObjectInitializer);

    virtual EBTNodeResult::Type ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory) override;

    virtual void OnTaskFinished(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory,
                                EBTNodeResult::Type TaskResult) override;
};
