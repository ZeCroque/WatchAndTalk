// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "BehaviorTree/Services/BTService_BlackboardBase.h"
#include "GoalReactionManagerBTS.generated.h"

/**
 * 
 */
UCLASS()
class WAT_API UGoalReactionManagerBTS final : public UBTService_BlackboardBase
{
	GENERATED_BODY()
	UGoalReactionManagerBTS();

protected:
	virtual FString GetStaticServiceDescription() const override;

private:
	virtual void TickNode(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory, float DeltaSeconds) override;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Blackboard key", meta = (AllowPrivateAccess = "true"))
	FBlackboardKeySelector PlayerLocation;

	UPROPERTY()
	class AMonsterCharacter* Self;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Blackboard key", meta=(AllowPrivateAccess="true"))
	FBlackboardKeySelector IsInGoalReactionMode;
};
