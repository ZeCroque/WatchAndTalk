// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "BehaviorTree/Tasks/BTTask_BlackboardBase.h"
#include "NpcCharacter.h"
#include "NpcMeleeAttackTask.generated.h"

/**
 * 
 */
UCLASS()
class WAT_API UNpcMeleeAttackTask final : public UBTTask_BlackboardBase
{
    GENERATED_BODY()
public:
    explicit UNpcMeleeAttackTask(FObjectInitializer const& ObjectInitializer);
    virtual EBTNodeResult::Type ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory) override;

private:
    static bool MontageHasFinished(ANpcCharacter* const Npc);
};
