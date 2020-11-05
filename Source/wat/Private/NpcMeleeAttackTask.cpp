// Fill out your copyright notice in the Description page of Project Settings.


#include "NpcMeleeAttackTask.h"
#include "AIController.h"

UNpcMeleeAttackTask::UNpcMeleeAttackTask(FObjectInitializer const& ObjectInitializer)
{
    NodeName = TEXT("Melee Attack");
}

EBTNodeResult::Type UNpcMeleeAttackTask::ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory)
{
    //get NPC
    AAIController* const Controller = OwnerComp.GetAIOwner();

    if (ANpcCharacter* const Npc = Cast<ANpcCharacter>(Controller->GetPawn()))
    {
        if (MontageHasFinished(Npc))
        {
            Npc->MeleeAttack();
        }
    }

    FinishLatentTask(OwnerComp, EBTNodeResult::Succeeded);
    return EBTNodeResult::Succeeded;
}

bool UNpcMeleeAttackTask::MontageHasFinished(ANpcCharacter* const Npc)
{
    return Npc->GetMesh()->GetAnimInstance()->Montage_GetIsStopped(Npc->GetMontage());
}
