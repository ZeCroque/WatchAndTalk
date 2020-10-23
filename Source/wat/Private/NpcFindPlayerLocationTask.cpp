// Fill out your copyright notice in the Description page of Project Settings.


#include "NpcFindPlayerLocationTask.h"
#include "BehaviorTree/BlackboardComponent.h"
#include "Runtime/NavigationSystem/Public/NavigationSystem.h"
#include "NpcAIController.h"
#include "GameFramework/Character.h"
#include "Kismet/GameplayStatics.h"
#include "BlackboardKeys.h"

UNpcFindPlayerLocationTask::UNpcFindPlayerLocationTask(const FObjectInitializer& ObjectInitializer)
{
    NodeName = TEXT("Find Player Location");
}

EBTNodeResult::Type UNpcFindPlayerLocationTask::ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory)
{
    // get player character and the NPC controller
    ACharacter* const Player = UGameplayStatics::GetPlayerCharacter(GetWorld(), 0);
    auto const Controller = Cast<ANpcAIController>(OwnerComp.GetAIOwner());

    // get player location to use as an origin
    FVector const PlayerLocation = Player->GetActorLocation();
    if (SearchRandom)
    {
        FNavLocation Location;

        // get the navigation system and generate a random location near the player
        UNavigationSystemV1* const NavigationSystem = UNavigationSystemV1::GetCurrent(GetWorld());
        if (NavigationSystem->GetRandomPointInNavigableRadius(PlayerLocation, SearchRadius, Location, nullptr))
        {
            Controller->GetBlackboard()->SetValueAsVector(BBKeys::TargetLocation, Location.Location);
        }
    }
    else
    {
        Controller->GetBlackboard()->SetValueAsVector(BBKeys::TargetLocation, PlayerLocation);
    }

    // finish with success
    FinishLatentTask(OwnerComp, EBTNodeResult::Succeeded);

    return EBTNodeResult::Succeeded;
}
