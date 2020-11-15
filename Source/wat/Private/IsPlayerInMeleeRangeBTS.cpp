// Fill out your copyright notice in the Description page of Project Settings.


#include "IsPlayerInMeleeRangeBTS.h"
#include "MonsterAIController.h"
#include "MonsterCharacter.h"
#include "Kismet/GameplayStatics.h"
#include "BehaviorTree/BlackboardComponent.h"
#include "BlackboardKeys.h"

UIsPlayerInMeleeRangeBTS::UIsPlayerInMeleeRangeBTS()
{
    bNotifyBecomeRelevant = true;
    NodeName = TEXT("Is Player in Melee Range");
}

void UIsPlayerInMeleeRangeBTS::OnBecomeRelevant(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory)
{
    Super::OnBecomeRelevant(OwnerComp, NodeMemory);

    // get NPC
    AMonsterAIController* const Controller = Cast<AMonsterAIController>(OwnerComp.GetAIOwner());
    AMonsterCharacter* const Npc = Cast<AMonsterCharacter>(Controller->GetPawn());

    //get player character
    ACharacter* const Player = UGameplayStatics::GetPlayerCharacter(GetWorld(), 0);

    // write true or false depending on whether the player is withing melee range
    Controller->GetBlackboard()->SetValueAsBool(BBKeys::PlayerIsInMeleeRange,
                                                Npc->GetDistanceTo(Player) <= MeleeRange);
    UE_LOG(LogTemp, Warning, TEXT("Distance is %f and I'm able to see you: %d"), Npc->GetDistanceTo(Player), Npc->GetDistanceTo(Player) <= MeleeRange);

	//TODO KeySelector
}
