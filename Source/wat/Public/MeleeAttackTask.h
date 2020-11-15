// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "BehaviorTree/Tasks/BTTask_BlackboardBase.h"
#include "MonsterCharacter.h"
#include "MeleeAttackTask.generated.h"

/**
 * 
 */
UCLASS()
class WAT_API UMeleeAttackTask final : public UBTTask_BlackboardBase
{
    GENERATED_BODY()
public:
    explicit UMeleeAttackTask(FObjectInitializer const& ObjectInitializer);
    virtual EBTNodeResult::Type ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory) override;

private:
    static bool MontageHasFinished(AMonsterCharacter* const Npc);
};
