// Fill out your copyright notice in the Description page of Project Settings.


#include "NpcStopInvestigatingTask.h"
#include "NpcAIController.h"
#include "BehaviorTree/BlackboardComponent.h"
#include "BlackboardKeys.h"

UNpcStopInvestigatingTask::UNpcStopInvestigatingTask(FObjectInitializer const& ObjectInitializer)
{
    NodeName = TEXT("Stop Investigating");
}

EBTNodeResult::Type UNpcStopInvestigatingTask::ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory)
{
    // get the NPC controller and write false to the blackboard key "IsInvestigating"
    const auto Controller = Cast<ANpcAIController>(OwnerComp.GetAIOwner());
    Controller->GetBlackboard()->SetValueAsBool(BBKeys::IsInvestigating, false);


    //finish with success
    FinishLatentTask(OwnerComp, EBTNodeResult::Succeeded);
    return EBTNodeResult::Succeeded;
}
