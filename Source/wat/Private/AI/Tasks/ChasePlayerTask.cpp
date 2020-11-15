// Fill out your copyright notice in the Description page of Project Settings.


#include "AI/Tasks/ChasePlayerTask.h"
#include "AI/MonsterAIController.h"
#include "BehaviorTree/BlackboardComponent.h"
#include "Blueprint/AIBlueprintHelperLibrary.h"

UChasePlayerTask::UChasePlayerTask(FObjectInitializer const& ObjectInitializer)
{
    bNotifyTaskFinished = true;
    NodeName = TEXT("Smart Move To");
}

EBTNodeResult::Type UChasePlayerTask::ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory)
{
    // get TargetLocation from blackboard via AI Controller
    AMonsterAIController* const Controller = Cast<AMonsterAIController>(OwnerComp.GetAIOwner());
    UBlackboardComponent* Blackboard = OwnerComp.GetBlackboardComponent();
    FVector const TargetLocation = Blackboard->GetValueAsVector(BlackboardKey.SelectedKeyName);


    if (Controller->GetPawn()->GetActorLocation() != TargetLocation)
    {
        // move to location
        UAIBlueprintHelperLibrary::SimpleMoveToLocation(Controller, TargetLocation);
        FinishLatentTask(OwnerComp, EBTNodeResult::InProgress);
        return EBTNodeResult::InProgress;
    }

    // finish with success
    Blackboard->ClearValue(BlackboardKey.SelectedKeyName);
    FinishLatentTask(OwnerComp, EBTNodeResult::Succeeded);
    return EBTNodeResult::Succeeded;
}

void UChasePlayerTask::OnTaskFinished(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory,
                                         EBTNodeResult::Type TaskResult)
{
    if (GEngine)
    {
        GEngine->AddOnScreenDebugMessage(-1, 60.0f, FColor::Yellow, "Finish !");
    }

    UBlackboardComponent* Blackboard = OwnerComp.GetBlackboardComponent();

}
