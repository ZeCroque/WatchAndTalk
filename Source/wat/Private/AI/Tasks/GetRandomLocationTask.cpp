// Fill out your copyright notice in the Description page of Project Settings.


#include "AI/Tasks/GetRandomLocationTask.h"

#include "BehaviorTree/BlackboardComponent.h"
#include "Runtime/NavigationSystem/Public/NavigationSystem.h"
#include "AI/MonsterAIController.h"
#include "Kismet/GameplayStatics.h"

UGetRandomLocationTask::UGetRandomLocationTask(const FObjectInitializer& ObjectInitializer)
{
	NodeName = TEXT("Find Random Location");
}

EBTNodeResult::Type UGetRandomLocationTask::ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory)
{
	// get AI controller and its NPC
	auto const Controller = Cast<AMonsterAIController>(OwnerComp.GetAIOwner());
	auto const NPC = Controller->GetPawn();

	// obtain npc location to use as an origin location
	FVector const Origin = NPC->GetActorLocation();
	FNavLocation NavLocation;

	// get the navigation system and generate a random location on the navMesh
	UNavigationSystemV1* const NavSys = UNavigationSystemV1::GetCurrent(GetWorld());

	AGridManager* GridManager = Cast<AGridManager>(
		UGameplayStatics::GetActorOfClass(GetWorld(), AGridManager::StaticClass()));


	FVector Location;
	if (!GridManager->RandomHotWorldCoordinate(Location))
	{
		GridManager->SetValueAround(Origin, SearchRadius);
		GridManager->RandomHotWorldCoordinate(Location);
	}

	if (NavSys->GetRandomPointInNavigableRadius(Location, 100, NavLocation,
	                                            nullptr))
	{
		Controller->GetBlackboard()->SetValueAsVector(BlackboardKey.SelectedKeyName, NavLocation.Location);
	}

	// finish with success
	FinishLatentTask(OwnerComp, EBTNodeResult::Succeeded);

	return EBTNodeResult::Succeeded;
}
