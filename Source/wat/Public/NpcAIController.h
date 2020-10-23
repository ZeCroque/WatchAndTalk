// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "AIController.h"
#include  "Perception/AIPerceptionTypes.h"
#include "NpcAIController.generated.h"

/**
 * 
 */
UCLASS()
class WAT_API ANpcAIController final : public AAIController
{
    GENERATED_BODY()

public:
    explicit ANpcAIController(const FObjectInitializer& ObjectInitializer = FObjectInitializer::Get());

    virtual void BeginPlay() override;
    virtual void OnPossess(APawn* InPawn) override;
    class UBlackboardComponent* GetBlackboard() const;
private:
    UPROPERTY(EditInstanceOnly, BlueprintReadWrite, Category= "AI", meta = (AllowPrivateAccess = "true"))
    class UBehaviorTreeComponent* BehaviorTreeComponent;

    UPROPERTY(EditInstanceOnly, BlueprintReadWrite, Category= "AI", meta = (AllowPrivateAccess = "true"))
    class UBehaviorTree* BTree;

    class UBlackboardComponent* Blackboard;

    class UAISenseConfig_Sight* SightConfig;

    UFUNCTION()
    void OnTargetDetected(AActor* Actor, FAIStimulus const Stimulus);

    void SetupPerceptionSystem();
};
