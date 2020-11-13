// Fill out your copyright notice in the Description page of Project Settings.


#include "NpcAIController.h"

#include "AITags.h"
#include "BlackboardKeys.h"
#include "BehaviorTree/BehaviorTreeComponent.h"
#include "BehaviorTree/BehaviorTree.h"
#include "BehaviorTree/BlackboardComponent.h"
#include "UObject/ConstructorHelpers.h"
#include "Perception/AISenseConfig_Sight.h"
#include "Perception/AISenseConfig_Hearing.h"
#include "Kismet/GameplayStatics.h"
#include "Perception/AIPerceptionComponent.h"

ANpcAIController::ANpcAIController(const FObjectInitializer& ObjectInitializer)
{
    static ConstructorHelpers::FObjectFinder<UBehaviorTree> OBJ(
        TEXT("BehaviorTree'/Game/Blueprints/AI/NpcBT.NpcBT'"));
    if (OBJ.Succeeded())
    {
        BTree = OBJ.Object;
    }
    BehaviorTreeComponent = ObjectInitializer.CreateDefaultSubobject<UBehaviorTreeComponent>(
        this, TEXT("BehaviorComp"));
    Blackboard = ObjectInitializer.CreateDefaultSubobject<UBlackboardComponent>(this, TEXT("BlackboardComp"));
    SetupPerceptionSystem();
}

void ANpcAIController::BeginPlay()
{
    Super::BeginPlay();
    RunBehaviorTree(BTree);
    BehaviorTreeComponent->StartTree(*BTree);
}

void ANpcAIController::OnPossess(APawn* InPawn)
{
    Super::OnPossess(InPawn);
    if (Blackboard)
    {
        Blackboard->InitializeBlackboard(*BTree->BlackboardAsset);
    }
}

UBlackboardComponent* ANpcAIController::GetBlackboard() const
{
    return Blackboard;
}

void ANpcAIController::SetupPerceptionSystem()
{
    // create and initialise sight configuration object
    SightConfig = CreateDefaultSubobject<UAISenseConfig_Sight>(TEXT("Sight Config"));
    if (SightConfig)
    {
        SetPerceptionComponent(*CreateDefaultSubobject<UAIPerceptionComponent>(TEXT("Perception Component")));
        SightConfig->SightRadius = 500.0f;
        SightConfig->LoseSightRadius = SightConfig->SightRadius + 50.0f;
        SightConfig->PeripheralVisionAngleDegrees = 90.0f;
        SightConfig->SetMaxAge(5.0f);
        SightConfig->AutoSuccessRangeFromLastSeenLocation = 900.0f;
        SightConfig->DetectionByAffiliation.bDetectEnemies = true;
        SightConfig->DetectionByAffiliation.bDetectFriendlies = true;
        SightConfig->DetectionByAffiliation.bDetectNeutrals = true;

        // add sight configuration component to perception component
        GetPerceptionComponent()->SetDominantSense(*SightConfig->GetSenseImplementation());
        GetPerceptionComponent()->ConfigureSense(*SightConfig);
    }

    // create and initialise sight configuration object
    HearingConfig = CreateDefaultSubobject<UAISenseConfig_Hearing>(TEXT("Hearing Config"));
    if (HearingConfig)
    {
        HearingConfig->HearingRange = 3000.0f;
        HearingConfig->DetectionByAffiliation.bDetectEnemies = true;
        HearingConfig->DetectionByAffiliation.bDetectFriendlies = true;
        HearingConfig->DetectionByAffiliation.bDetectNeutrals = true;

        // add hearing configuration component to perception component
        GetPerceptionComponent()->ConfigureSense(*HearingConfig);
    }
}
