// Fill out your copyright notice in the Description page of Project Settings.


#include "AI/Services/IsPlayerInMeleeRangeBTS.h"
#include "AI/MonsterAIController.h"
#include "Kismet/GameplayStatics.h"
#include "BehaviorTree/BlackboardComponent.h"

UIsPlayerInMeleeRangeBTS::UIsPlayerInMeleeRangeBTS()
{
    bNotifyBecomeRelevant = true;
    bNotifyCeaseRelevant = true;
    NodeName = TEXT("Is Player in Melee Range");
    Player = nullptr;
}

void UIsPlayerInMeleeRangeBTS::OnBecomeRelevant(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory)
{
    Super::OnBecomeRelevant(OwnerComp, NodeMemory);

    // get NPC
    auto* const Controller = OwnerComp.GetAIOwner();
    auto* const Npc = Controller->GetPawn();


    auto const Target = Controller->GetBlackboardComponent()->GetValueAsObject(PlayerKey.SelectedKeyName);
    //UGameplayStatics::GetPlayerCharacter(GetWorld(), 0);

    if (Target && !Player)
    {
        Player = Cast<AActor>(Target);
    }

    // write true or false depending on whether the player is withing melee range
    Controller->GetBlackboardComponent()->SetValueAsBool(PlayerIsInMeleeRange.SelectedKeyName,
                                                         Npc->GetDistanceTo(Player) <= MeleeRange);
}

void UIsPlayerInMeleeRangeBTS::OnCeaseRelevant(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory)
{
    auto* const Controller = OwnerComp.GetAIOwner();
    auto const Target = Controller->GetBlackboardComponent()->GetValueAsObject(PlayerKey.SelectedKeyName);
    if (!Target)
    {
        Player = nullptr;
    }
}
