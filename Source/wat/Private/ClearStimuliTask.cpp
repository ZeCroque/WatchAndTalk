// Fill out your copyright notice in the Description page of Project Settings.

#include "ClearStimuliTask.h"
#include "AIController.h"
#include "Perception/AIPerceptionComponent.h"


UClearStimuliTask::UClearStimuliTask(const FObjectInitializer& ObjectInitializer)
{
	NodeName = "Clear Stimuli";
}

EBTNodeResult::Type UClearStimuliTask::ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory)
{
	OwnerComp.GetAIOwner()->GetAIPerceptionComponent()->ForgetAll();
	FinishLatentTask(OwnerComp, EBTNodeResult::Succeeded);
	return EBTNodeResult::Succeeded;
}
