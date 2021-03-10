// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"

#include "wat/AI/GridManager.h"
#include "wat/AI/MonsterAIController.h"
#include "BehaviorTree/Tasks/BTTask_BlackboardBase.h"
#include "BehaviorTree/BehaviorTreeTypes.h"
#include "wat/Core/WatGameState.h"

#include "GetRandomLocationTask.generated.h"

/**
 * 
 */
UCLASS(Blueprintable)
class WAT_API UGetRandomLocationTask final : public UBTTask_BlackboardBase
{
	GENERATED_BODY()
public:
	explicit UGetRandomLocationTask(const FObjectInitializer& ObjectInitializer);
	virtual EBTNodeResult::Type ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory) override;
private:
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="WAT - Search", meta= (AllowPrivateAccess = "true"))
	int DefaultFillRadius = 2;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="WAT - Search", meta= (AllowPrivateAccess = "true"))
	float RandomPointInNavmeshRadiusOffset = 50;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="WAT - Search", meta= (AllowPrivateAccess = "true"))
	int TryForBestWeight = 10;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="WAT - Search", meta= (AllowPrivateAccess = "true"))
	int RadiusForBestWeight = 10;

	UPROPERTY()
	AGridManager* GridManager;

	UPROPERTY()
	AWatGameState* WatGameState;

	UPROPERTY()
	AMonsterAIController* Controller;
};
