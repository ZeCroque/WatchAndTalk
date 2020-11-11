// Fill out your copyright notice in the Description page of Project Settings.


#include "NpcChasePlayerTask.h"
#include "NpcAIController.h"
#include "BehaviorTree/BlackboardComponent.h"
#include "Blueprint/AIBlueprintHelperLibrary.h"
#include "BlackboardKeys.h"

UNpcChasePlayerTask::UNpcChasePlayerTask(FObjectInitializer const& ObjectInitializer)
{
    NodeName = TEXT("Chase Player");
}

EBTNodeResult::Type UNpcChasePlayerTask::ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory)
{
    // get TargetLocation from blackboard via AI Controller
    ANpcAIController* const Controller = Cast<ANpcAIController>(OwnerComp.GetAIOwner());
    FVector const PlayerLocation = Controller->GetBlackboard()->GetValueAsVector(BBKeys::TargetLocation);

    // move to player's location
    UAIBlueprintHelperLibrary::SimpleMoveToLocation(Controller, PlayerLocation);

    // finish with success
    FinishLatentTask(OwnerComp, EBTNodeResult::Succeeded);
    return EBTNodeResult::Succeeded;
}
