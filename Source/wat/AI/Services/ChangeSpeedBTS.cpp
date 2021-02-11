// Fill out your copyright notice in the Description page of Project Settings.


#include "wat/AI/Services/ChangeSpeedBTS.h"
#include "wat/AI/MonsterAIController.h"
#include "wat/AI/MonsterCharacter.h"
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
	Npc->GetCharacterMovement()->MaxWalkSpeed = Speed * Npc->bNotAnimPlaying;
}

FString UChangeSpeedBTS::GetStaticServiceDescription() const
{
	return FString("Change the NPC speed");
}
