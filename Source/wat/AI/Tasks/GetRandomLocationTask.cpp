// Fill out your copyright notice in the Description page of Project Settings.


#include "wat/AI/Tasks/GetRandomLocationTask.h"

#include "BehaviorTree/BlackboardComponent.h"
#include "Runtime/NavigationSystem/Public/NavigationSystem.h"
#include "wat/AI/MonsterAIController.h"
#include "wat/Core/WatGameState.h"
#include "Kismet/GameplayStatics.h"
#include "wat/GameplayActors/Triggers/BunkerTriggerBox.h"

UGetRandomLocationTask::UGetRandomLocationTask(const FObjectInitializer& ObjectInitializer)
{
	NodeName = TEXT("Find Random Location");
}

EBTNodeResult::Type UGetRandomLocationTask::ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory)
{
	if (!GridManager)
	{
		WatGameState = Cast<AWatGameState>(UGameplayStatics::GetGameState(GetWorld()));
		GridManager = WatGameState->GridManager;
	}

	if (!Controller)
	{
		Controller = Cast<AMonsterAIController>(OwnerComp.GetAIOwner());
	}

	if (GridManager && Controller)
	{
		FVector Location;
		if (!GridManager->SmartRandomHotWorldCoordinate(Location, TryForBestWeight, RadiusForBestWeight))
		{
			auto const NPC = Controller->GetPawn();
			FVector const Origin = WatGameState->NumberOfActorInBunker != 1 && WatGameState->NumberOfActorInBunker != 3
				                       ? WatGameState->BunkerTriggerBox->GetActorLocation()
				                       : NPC->GetActorLocation();
			const auto Radius = WatGameState->NumberOfActorInBunker != 1 && WatGameState->NumberOfActorInBunker != 3
				                    ? WatGameState->BunkerTriggerBox->GridDrawRay
				                    : DefaultFillRadius;

			GridManager->SetValueAround(Origin, Radius);
			GridManager->SmartRandomHotWorldCoordinate(Location, TryForBestWeight, RadiusForBestWeight);
		}
#if WITH_EDITOR
		GridManager->PickedHotPointCoordinate = GridManager->GetGridCoordinate(Location);
#endif
		FVector NavLocation;
		UNavigationSystemV1* const NavSys = UNavigationSystemV1::GetCurrent(GetWorld());

		auto i = 0;
		do
		{
			++i;
			NavSys->K2_GetRandomReachablePointInRadius(GetWorld(), Location, NavLocation,
			                                           FMath::Max(GridManager->CellHeight, GridManager->CellWidth) +
			                                           RandomPointInNavmeshRadiusOffset);
		}
		while (FVector::Dist(FVector(0.f, 0.f, Location.Z), FVector(0.f, 0.f, (NavLocation.Z))) > 300
			&& i < 100);
#if WITH_EDITOR
		GridManager->PickedNavMeshCoordinate = GridManager->GetGridCoordinate(NavLocation);
#endif

		Controller->GetBlackboard()->SetValueAsVector(BlackboardKey.SelectedKeyName, NavLocation);

		FinishLatentTask(OwnerComp, EBTNodeResult::Succeeded);
		return EBTNodeResult::Succeeded;
	}


	FinishLatentTask(OwnerComp, EBTNodeResult::Failed);
	return EBTNodeResult::Failed;
}
