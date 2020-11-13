// Fill out your copyright notice in the Description page of Project Settings.


#include "StimulusManagerBTS.h"

#include "Perception/AIPerceptionComponent.h"
#include "AIController.h"
#include "AITags.h"
#include "NavigationSystem.h"
#include "Misc/TextFilterExpressionEvaluator.h"
#include "BehaviorTree/BehaviorTreeComponent.h"
#include "BehaviorTree/BehaviorTree.h"
#include "BehaviorTree/BlackboardComponent.h"

class UNavigationSystemV1;
class ANpcAIController;

UStimulusManagerBTS::UStimulusManagerBTS()
{
    NodeName = TEXT("Stimulus Manager");
}

FString UStimulusManagerBTS::GetStaticServiceDescription() const
{
    return FString("Setup actual perception from stimulus state");
}

void UStimulusManagerBTS::TickNode(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory, float DeltaSeconds)
{
    Super::TickNode(OwnerComp, NodeMemory, DeltaSeconds);

    AAIController* const Controller = OwnerComp.GetAIOwner();
    UBlackboardComponent* Blackboard = OwnerComp.GetBlackboardComponent();
    if (Controller)
    {
        TArray<AActor*> Actors;
        UAIPerceptionComponent* PerceptionComponent = Controller->GetPerceptionComponent();
        PerceptionComponent->GetKnownPerceivedActors(nullptr, Actors);

        for (AActor* Actor : Actors)
        {
            FActorPerceptionBlueprintInfo Infos;
            PerceptionComponent->GetActorsPerception(Actor, Infos);

            for (FAIStimulus const Stimulus : Infos.LastSensedStimuli)
            {
                if (Stimulus.Type.Name == "Default__AISense_Sight")
                {
                    Blackboard->SetValueAsVector(SightLocation.SelectedKeyName, Stimulus.StimulusLocation);
                    Blackboard->ClearValue(HearingLocation.SelectedKeyName);
                    Blackboard->ClearValue(RandomLocation.SelectedKeyName);
                    break;
                }
                if (Stimulus.Type.Name == "Default__AISense_Hearing")
                {
                    if (Stimulus.Tag == AITags::NoiseTag)
                    {
                        Blackboard->SetValueAsVector(HearingLocation.SelectedKeyName, Stimulus.StimulusLocation);
                        Blackboard->ClearValue(SightLocation.SelectedKeyName);
                        Blackboard->ClearValue(RandomLocation.SelectedKeyName);
                        break;
                    }
                }
            }
        }

        if (!Blackboard->IsVectorValueSet(RandomLocation.SelectedKeyName) && !Blackboard->
            IsVectorValueSet(SightLocation.SelectedKeyName) && !Blackboard->IsVectorValueSet(
                HearingLocation.SelectedKeyName))
        {
            auto const NPC = Controller->GetPawn();

            // obtain npc location to use as an origin location
            FVector const Origin = NPC->GetActorLocation();
            FNavLocation NavLocation;

            // get the navigation system and generate a random location on the navMesh
            UNavigationSystemV1* const NavSys = UNavigationSystemV1::GetCurrent(GetWorld());
            if (NavSys->GetRandomPointInNavigableRadius(Origin, SearchRadius, NavLocation, nullptr))
            {
                Blackboard->SetValueAsVector(RandomLocation.SelectedKeyName, NavLocation.Location);
            }
            Blackboard->ClearValue(SightLocation.SelectedKeyName);
            Blackboard->ClearValue(HearingLocation.SelectedKeyName);
        }
    }
}
