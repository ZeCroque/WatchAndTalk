// Fill out your copyright notice in the Description page of Project Settings.


#include "AI/Services/GoalReactionManagerBTS.h"
#include "AI/MonsterAIController.h"
#include "Kismet/GameplayStatics.h"
#include "BehaviorTree/BlackboardComponent.h"
#include "Characters/MonsterCharacter.h"
#include "Characters/PlayerCharacter.h"

UGoalReactionManagerBTS::UGoalReactionManagerBTS()
{
	NodeName = TEXT("Goal Reaction Manager");
}

FString UGoalReactionManagerBTS::GetStaticServiceDescription() const
{
	return FString("Setup actual perception from stimulus state");
}

void UGoalReactionManagerBTS::TickNode(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory, float DeltaSeconds)
{
	if (!Self)
	{
		Self = Cast<AMonsterCharacter>(OwnerComp.GetAIOwner()->GetPawn());
	}

	auto* const Controller = OwnerComp.GetAIOwner();;
	if (Self && Self->PlayerToChase)
	{
		Controller->GetBlackboardComponent()->SetValueAsBool(IsInGoalReactionMode.SelectedKeyName, true);
		Controller->GetBlackboardComponent()->SetValueAsVector(PlayerLocation.SelectedKeyName,
		                                                       Self->PlayerToChase->GetActorLocation());
	}
	else if (Self)
	{
		Controller->GetBlackboardComponent()->SetValueAsBool(IsInGoalReactionMode.SelectedKeyName, false);
		Controller->GetBlackboardComponent()->ClearValue(PlayerLocation.SelectedKeyName);
	}
}
