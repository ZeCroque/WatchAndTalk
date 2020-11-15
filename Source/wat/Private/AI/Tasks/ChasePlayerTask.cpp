// Fill out your copyright notice in the Description page of Project Settings.


#include "AI/Tasks/ChasePlayerTask.h"
#include "AI/MonsterAIController.h"
#include "BehaviorTree/BlackboardComponent.h"
#include "Blueprint/AIBlueprintHelperLibrary.h"

UChasePlayerTask::UChasePlayerTask(FObjectInitializer const& ObjectInitializer)
{
    bNotifyTaskFinished = true;
    NodeName = TEXT("Chase Player");
}

EBTNodeResult::Type UChasePlayerTask::ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory)
{
    // get TargetLocation from blackboard via AI Controller
    AMonsterAIController* const Controller = Cast<AMonsterAIController>(OwnerComp.GetAIOwner());
    UBlackboardComponent* Blackboard = OwnerComp.GetBlackboardComponent();
    FVector const TargetLocation = Blackboard->GetValueAsVector(BlackboardKey.SelectedKeyName);

    UAIBlueprintHelperLibrary::SimpleMoveToLocation(Controller, TargetLocation);

    // finish with success
    FinishLatentTask(OwnerComp, EBTNodeResult::Succeeded);
    return EBTNodeResult::Succeeded;
}
