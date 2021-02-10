// Fill out your copyright notice in the Description page of Project Settings.

#include "wat/AI/Tasks/ClearVectorTask.h"
#include "BehaviorTree/BlackboardComponent.h"


UClearVectorTask::UClearVectorTask(FObjectInitializer const& ObjectInitializer)
{
    NodeName = TEXT("Clear Vector");
}

EBTNodeResult::Type UClearVectorTask::ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory)
{
    OwnerComp.GetBlackboardComponent()->ClearValue(BlackboardKey.SelectedKeyName);
    FinishLatentTask(OwnerComp, EBTNodeResult::Succeeded);
    return EBTNodeResult::Succeeded;
}
