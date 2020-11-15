// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "BehaviorTree/Services/BTService_BlackboardBase.h"
#include "StimulusManagerBTS.generated.h"

/**
 * 
 */
UCLASS()
class WAT_API UStimulusManagerBTS final : public UBTService_BlackboardBase
{
    GENERATED_BODY()
    UStimulusManagerBTS();

protected:
    virtual FString GetStaticServiceDescription() const override;

private:
    virtual void TickNode(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory, float DeltaSeconds) override;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Blackboard key", meta=(AllowPrivateAccess="true"))
    FBlackboardKeySelector RandomLocation;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Blackboard key", meta=(AllowPrivateAccess="true"))
    FBlackboardKeySelector NoiseLocation;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Blackboard key", meta=(AllowPrivateAccess="true"))
    FBlackboardKeySelector CurrentSeenLocation;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Blackboard key", meta = (AllowPrivateAccess = "true"))
    FBlackboardKeySelector LastSeenLocation;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Blackboard key", meta = (AllowPrivateAccess = "true"))
    FBlackboardKeySelector HasSightStimulus;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Blackboard key", meta=(AllowPrivateAccess="true"))
     FBlackboardKeySelector Player;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Random Location Tweak", meta=(AllowPrivateAccess="true"))
    float SearchRadius = 1500.0f;

    
};
