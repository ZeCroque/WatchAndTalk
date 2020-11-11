// Fill out your copyright notice in the Description page of Project Settings.


#include "ChangeSpeedBTS.h"
#include "NpcAIController.h"
#include "NpcCharacter.h"
#include "GameFramework/CharacterMovementComponent.h"

UChangeSpeedBTS::UChangeSpeedBTS()
{
    bNotifyBecomeRelevant = true;
    NodeName = TEXT("Change Speed");
}

void UChangeSpeedBTS::OnBecomeRelevant(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory)
{
    Super::OnBecomeRelevant(OwnerComp, NodeMemory);
    ANpcAIController* const Controller = Cast<ANpcAIController>(OwnerComp.GetAIOwner());
    ANpcCharacter* const Npc = Cast<ANpcCharacter>(Controller->GetPawn());
    Npc->GetCharacterMovement()->MaxWalkSpeed = Speed;
}

FString UChangeSpeedBTS::GetStaticServiceDescription() const
{
    return FString("Change the NPC speed");
}
