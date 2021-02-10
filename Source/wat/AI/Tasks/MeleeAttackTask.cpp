// Fill out your copyright notice in the Description page of Project Settings.


#include "wat/AI/Tasks/MeleeAttackTask.h"
#include "AIController.h"
#include "BehaviorTree/BlackboardComponent.h"
#include "wat/AI/MonsterCharacter.h"
#include "wat/Core/WatPlayerCharacter.h"

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
        	auto* Target = Cast<AWatPlayerCharacter>(OwnerComp.GetBlackboardComponent()->GetValueAsObject(BlackboardKey.SelectedKeyName));
        	if(Target && Target->IsAlive())
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
