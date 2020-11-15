// Fill out your copyright notice in the Description page of Project Settings.


#include "AI/Services/ChangeSpeedBTS.h"
#include "AI/MonsterAIController.h"
#include "Characters/MonsterCharacter.h"
#include "GameFramework/CharacterMovementComponent.h"

UChangeSpeedBTS::UChangeSpeedBTS()
{
    bNotifyBecomeRelevant = true;
    NodeName = TEXT("Change Speed");
}

void UChangeSpeedBTS::OnBecomeRelevant(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory)
{
    Super::OnBecomeRelevant(OwnerComp, NodeMemory);
    AMonsterAIController* const Controller = Cast<AMonsterAIController>(OwnerComp.GetAIOwner());
    AMonsterCharacter* const Npc = Cast<AMonsterCharacter>(Controller->GetPawn());
    Npc->GetCharacterMovement()->MaxWalkSpeed = Speed;
}

FString UChangeSpeedBTS::GetStaticServiceDescription() const
{
    return FString("Change the NPC speed");
}
