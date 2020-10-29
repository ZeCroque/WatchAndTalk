// Fill out your copyright notice in the Description page of Project Settings.


#include "NpcMeleeAttackTask.h"
//#include "CombatInterface.h"
#include "AIController.h"
#include "Kismet/KismetSystemLibrary.h"
#include "Runtime/Engine/Classes/Engine/World.h"
#include "Engine/LatentActionManager.h"

UNpcMeleeAttackTask::UNpcMeleeAttackTask(FObjectInitializer const& ObjectInitializer)
{
    NodeName = TEXT("Melee Attack");
}

EBTNodeResult::Type UNpcMeleeAttackTask::ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory)
{
    // AAIController* const Controller = OwnerComp.GetAIOwner();
    // ANpcCharacter* const Npc = Cast<ANpcCharacter>(Controller->GetPawn());
    // if (ICombatInterface* const ICombat = Cast<ICombatInterface>(Npc))
    // {
    //     if (MontageHasFinished(Npc))
    //     {
    //         ICombat->ExecuteMeleeAttack(Npc);
    //     }
    // }

    if (GEngine)
    {
        GEngine->AddOnScreenDebugMessage(-1, 1.0f, FColor::Yellow, "Attack !");
    }

    FinishLatentTask(OwnerComp, EBTNodeResult::Succeeded);
    return EBTNodeResult::Succeeded;
}

bool UNpcMeleeAttackTask::MontageHasFinished(ANpcCharacter* const Npc)
{
    return true;
    // return Npc->GetMesh()->GetAnimInstance()->Montage_GetIsStopped(Npc->GetMontage());
}
