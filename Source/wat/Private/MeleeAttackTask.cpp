// Fill out your copyright notice in the Description page of Project Settings.


#include "MeleeAttackTask.h"
#include "AIController.h"

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
            Npc->MeleeAttack();
        }
    }

    FinishLatentTask(OwnerComp, EBTNodeResult::Succeeded);
    return EBTNodeResult::Succeeded;
}

bool UMeleeAttackTask::MontageHasFinished(AMonsterCharacter* const Npc)
{
    return Npc->GetMesh()->GetAnimInstance()->Montage_GetIsStopped(Npc->GetMontage());
}
