// Fill out your copyright notice in the Description page of Project Settings.


#include "AI/Services/GoalReactionManagerBTS.h"
#include "AI/MonsterAIController.h"
#include "Kismet/GameplayStatics.h"
#include "BehaviorTree/BlackboardComponent.h"

UGoalReactionManagerBTS::UGoalReactionManagerBTS()
{
    NodeName = TEXT("Goal Complection Manager");
}

FString UGoalReactionManagerBTS::GetStaticServiceDescription() const
{
    return FString("Setup actual perception from stimulus state");
}

void UGoalReactionManagerBTS::TickNode(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory, float DeltaSeconds)
{
    auto* const Controller = OwnerComp.GetAIOwner();
    auto const Target = Controller->GetBlackboardComponent()->GetValueAsObject(PlayerKey.SelectedKeyName);

    if (Target && !Player)
    {
        Player = Cast<AActor>(Target);
    }

    Controller->GetBlackboardComponent()->SetValueAsVector(PlayerLocation.SelectedKeyName, Player->GetActorLocation());
}
