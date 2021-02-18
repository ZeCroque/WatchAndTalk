// Fill out your copyright notice in the Description page of Project Settings.


#include "wat/AI/Tasks/GetRandomLocationTask.h"

#include "BehaviorTree/BlackboardComponent.h"
#include "Runtime/NavigationSystem/Public/NavigationSystem.h"
#include "wat/AI/MonsterAIController.h"
#include "wat/Core/WatGameState.h"
#include "Kismet/GameplayStatics.h"

UGetRandomLocationTask::UGetRandomLocationTask(const FObjectInitializer& ObjectInitializer)
{
	NodeName = TEXT("Find Random Location");
}

EBTNodeResult::Type UGetRandomLocationTask::ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory)
{
	if (!GridManager)
	{
		GridManager = Cast<AWatGameState>(UGameplayStatics::GetGameState(GetWorld()))->GridManager;
	}

	if (!Controller)
	{
		Controller = Cast<AMonsterAIController>(OwnerComp.GetAIOwner());
	}

	if (GridManager && Controller)
	{
		FVector Location;

		if (!GridManager->RandomHotWorldCoordinate(Location))
		{
			auto const NPC = Controller->GetPawn();
			FVector const Origin = NPC->GetActorLocation();

			GridManager->SetValueAround(Origin, SearchRadius);
			GridManager->RandomHotWorldCoordinate(Location);
		}


		FVector NavLocation;
		UNavigationSystemV1* const NavSys = UNavigationSystemV1::GetCurrent(GetWorld());

		auto i = 0;
		do
		{
			++i;
			NavSys->K2_GetRandomReachablePointInRadius(GetWorld(),Location,NavLocation,1000);
		}
		while (FVector::Dist(FVector(0.f, 0.f, Location.Z), FVector(0.f, 0.f, (NavLocation.Z))) > 300
			|| i < 100);

		Controller->GetBlackboard()->SetValueAsVector(BlackboardKey.SelectedKeyName, NavLocation);

		FinishLatentTask(OwnerComp, EBTNodeResult::Succeeded);
		return EBTNodeResult::Succeeded;
	}


	FinishLatentTask(OwnerComp, EBTNodeResult::Failed);
	return EBTNodeResult::Failed;
}
