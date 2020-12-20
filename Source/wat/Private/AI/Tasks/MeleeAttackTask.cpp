// Fill out your copyright notice in the Description page of Project Settings.


#include "AI/Tasks/MeleeAttackTask.h"
#include "AIController.h"
#include "BehaviorTree/BlackboardComponent.h"
#include "Characters/PlayerCharacter.h"

UMeleeAttackTask::UMeleeAttackTask(FObjectInitializer const& ObjectInitializer)
{
    NodeName = TEXT("Melee Attack");
}

EBTNodeResult::Type UMeleeAttackTask::ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory)
{
    //get NPC
    AAIController* const Controller = OwnerComp.GetAIOwner();

    if (AMonsterCharacter* const Npc = Cast<AMonsterCharacter>(Controller->GetPawn()))
    {
        if (MontageHasFinished(Npc))
        {
        	auto* Target = Cast<APlayerCharacter>(OwnerComp.GetBlackboardComponent()->GetValueAsObject(BlackboardKey.SelectedKeyName));
        	if(Target)
        	{
				Npc->MeleeAttack(Target);
            }
        }
    }

    FinishLatentTask(OwnerComp, EBTNodeResult::Succeeded);
    return EBTNodeResult::Succeeded;
}

bool UMeleeAttackTask::MontageHasFinished(AMonsterCharacter* const Npc)
{
    return Npc->GetMesh()->GetAnimInstance()->Montage_GetIsStopped(Npc->GetMontage());
}
