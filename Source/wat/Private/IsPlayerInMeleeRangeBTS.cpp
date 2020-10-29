// Fill out your copyright notice in the Description page of Project Settings.


#include "IsPlayerInMeleeRangeBTS.h"
#include "NpcAIController.h"
#include "NpcCharacter.h"
#include "Kismet/GameplayStatics.h"
#include "GameFramework/Character.h"
#include "Runtime/Engine/Classes/Engine/World.h"
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
    ANpcAIController* const Controller = Cast<ANpcAIController>(OwnerComp.GetAIOwner());
    ANpcCharacter* const Npc = Cast<ANpcCharacter>(Controller->GetPawn());

    //get player character
    ACharacter* const Player = UGameplayStatics::GetPlayerCharacter(GetWorld(), 0);

    // write true or false depending on whether the player is withing melee range
    Controller->GetBlackboard()->SetValueAsBool(BBKeys::PlayerIsInMeleeRange,
                                                Npc->GetDistanceTo(Player) <= MeleeRange);
}
