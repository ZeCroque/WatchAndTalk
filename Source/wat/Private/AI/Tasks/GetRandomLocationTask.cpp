// Fill out your copyright notice in the Description page of Project Settings.


#include "AI/Tasks/GetRandomLocationTask.h"

#include "BehaviorTree/BlackboardComponent.h"
#include "Runtime/NavigationSystem/Public/NavigationSystem.h"
#include "AI/MonsterAIController.h"
#include "Core/MainGameState.h"
#include "Kismet/GameplayStatics.h"

UGetRandomLocationTask::UGetRandomLocationTask(const FObjectInitializer& ObjectInitializer)
{
    NodeName = TEXT("Find Random Location");
}

EBTNodeResult::Type UGetRandomLocationTask::ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory)
{
    if (!GridManager)
    {
        GridManager = Cast<AMainGameState>(UGameplayStatics::GetGameState(GetWorld()))->GridManager;
    }

    if (GridManager)
    {
        auto const Controller = Cast<AMonsterAIController>(OwnerComp.GetAIOwner());
        FVector Location;

        if (!GridManager->RandomHotWorldCoordinate(Location))
        {
            auto const NPC = Controller->GetPawn();
            FVector const Origin = NPC->GetActorLocation();

            GridManager->SetValueAround(Origin, SearchRadius);
            GridManager->RandomHotWorldCoordinate(Location);
        }


        FNavLocation NavLocation;
        UNavigationSystemV1* const NavSys = UNavigationSystemV1::GetCurrent(GetWorld());

        if (NavSys->GetRandomPointInNavigableRadius(Location, 100, NavLocation,
                                                    nullptr))
        {
            Controller->GetBlackboard()->SetValueAsVector(BlackboardKey.SelectedKeyName, NavLocation.Location);
        }


        FinishLatentTask(OwnerComp, EBTNodeResult::Succeeded);
        return EBTNodeResult::Succeeded;
    }


    FinishLatentTask(OwnerComp, EBTNodeResult::Failed);
    return EBTNodeResult::Failed;
}
