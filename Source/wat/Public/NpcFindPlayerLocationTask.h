// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "BehaviorTree/Tasks/BTTask_BlackboardBase.h"
#include "BehaviorTree/BehaviorTreeTypes.h"
#include "NpcFindPlayerLocationTask.generated.h"

/**
 * 
 */
UCLASS(Blueprintable)
class WAT_API UNpcFindPlayerLocationTask final : public UBTTask_BlackboardBase
{
    GENERATED_BODY()
public:
    explicit UNpcFindPlayerLocationTask(const FObjectInitializer& ObjectInitializer);

    virtual EBTNodeResult::Type ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory) override;

protected:
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Search", meta = (AllowProtectedAccess="true"))
    bool SearchRandom = false;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Search", meta = (AllowProtectedAccess="true"))
    float SearchRadius = 150.0f;
};
