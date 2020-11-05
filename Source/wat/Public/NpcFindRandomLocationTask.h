// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "BehaviorTree/Tasks/BTTask_BlackboardBase.h"
#include "BehaviorTree/BehaviorTreeTypes.h"
#include "NpcFindRandomLocationTask.generated.h"

/**
 * 
 */
UCLASS(Blueprintable)
class WAT_API UNpcFindRandomLocationTask final : public UBTTask_BlackboardBase
{
    GENERATED_BODY()
public:
    explicit UNpcFindRandomLocationTask(const FObjectInitializer& ObjectInitializer);
    virtual EBTNodeResult::Type ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory) override;
private:
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Search", meta= (AllowPrivateAccess = "true"))
    float SearchRadius = 1500.0F;
};
