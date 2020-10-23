// Fill out your copyright notice in the Description page of Project Settings.


#include "NpcAIController.h"

#include "BlackboardKeys.h"
#include "BehaviorTree/BehaviorTreeComponent.h"
#include "BehaviorTree/BehaviorTree.h"
#include "BehaviorTree/BlackboardComponent.h"
#include "UObject/ConstructorHelpers.h"
#include "PlayerCharacter.h"
#include  "Perception/AISenseConfig_Sight.h"
#include "Kismet/GameplayStatics.h"
#include "Perception/AIPerceptionComponent.h"

ANpcAIController::ANpcAIController(const FObjectInitializer& ObjectInitializer)
{
    static ConstructorHelpers::FObjectFinder<UBehaviorTree> OBJ(
        TEXT("BehaviorTree'/Game/Developers/VRELIN/AI/NpcBT.NpcBT'"));
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

// ReSharper disable once CppMemberFunctionMayBeConst
void ANpcAIController::OnTargetDetected(AActor* Actor, FAIStimulus const Stimulus)
{
    if (Cast<APlayerCharacter>(Actor))
    {
        GetBlackboard()->SetValueAsBool(BBKeys::CanSeePlayer, Stimulus.WasSuccessfullySensed());
    }
}

void ANpcAIController::SetupPerceptionSystem()
{
    // create and initialise sight configuration object
    SightConfig = CreateDefaultSubobject<UAISenseConfig_Sight>(TEXT("Sight Config"));
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
    GetPerceptionComponent()->OnTargetPerceptionUpdated.AddDynamic(this, &ANpcAIController::OnTargetDetected);
    GetPerceptionComponent()->ConfigureSense(*SightConfig);
}
