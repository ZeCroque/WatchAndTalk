// Fill out your copyright notice in the Description page of Project Settings.


#include "NpcAIController.h"
#include "BehaviorTree/BehaviorTreeComponent.h"
#include "BehaviorTree/BehaviorTree.h"
#include "BehaviorTree/BlackboardComponent.h"
#include "UObject/ConstructorHelpers.h"
#include "NpcCharacter.h"

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

UBlackboardComponent* ANpcAIController::Get_Blackboard() const
{
    return Blackboard;
}
