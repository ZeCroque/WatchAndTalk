// Fill out your copyright notice in the Description page of Project Settings.


#include "AI/Services/StimulusManagerBTS.h"

#include "Perception/AIPerceptionComponent.h"
#include "AIController.h"
#include "AI/AITags.h"
#include "Misc/TextFilterExpressionEvaluator.h"
#include "BehaviorTree/BehaviorTreeComponent.h"
#include "BehaviorTree/BehaviorTree.h"
#include "BehaviorTree/BlackboardComponent.h"
#include "Perception/AISense_Sight.h"

class UNavigationSystemV1;
class AMonsterAIController;

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
        PerceptionComponent->GetCurrentlyPerceivedActors(UAISense_Sight::StaticClass(), Actors);

        for (AActor* Actor : Actors)
        {
            FActorPerceptionBlueprintInfo Infos;
            PerceptionComponent->GetActorsPerception(Actor, Infos);

            for (FAIStimulus const Stimulus : Infos.LastSensedStimuli)
            {
                Blackboard->SetValueAsVector(CurrentSeenLocation.SelectedKeyName, Stimulus.StimulusLocation);
                Blackboard->ClearValue(NoiseLocation.SelectedKeyName);
                Blackboard->ClearValue(RandomLocation.SelectedKeyName);
                Blackboard->SetValueAsBool(HasSightStimulus.SelectedKeyName, true);
                return;
            }
        }

        Blackboard->ClearValue(CurrentSeenLocation.SelectedKeyName);
        Blackboard->ClearValue(LastSeenLocation.SelectedKeyName);
        PerceptionComponent->GetKnownPerceivedActors(nullptr, Actors);
        for (AActor* Actor : Actors)
        {
            FActorPerceptionBlueprintInfo Infos;
            PerceptionComponent->GetActorsPerception(Actor, Infos);

            for (FAIStimulus const& Stimulus : Infos.LastSensedStimuli)
            {
                if (Stimulus.Type.Name == "Default__AISense_Hearing")
                {
                    if (Stimulus.Tag == AITags::NoiseTag)
                    {
                        Blackboard->SetValueAsVector(NoiseLocation.SelectedKeyName, Stimulus.StimulusLocation);
                        Blackboard->ClearValue(RandomLocation.SelectedKeyName);
                    }
                }
            	if (Stimulus.Type.Name == "Default__AISense_Sight")
                {
                    //TODO consider stimulus age
                    Blackboard->SetValueAsVector(LastSeenLocation.SelectedKeyName, Stimulus.StimulusLocation);
                }
            }
        }

        if (!Blackboard->IsVectorValueSet(LastSeenLocation.SelectedKeyName))
        {
            Blackboard->SetValueAsBool(HasSightStimulus.SelectedKeyName, false);
            Blackboard->ClearValue(LastSeenLocation.SelectedKeyName);
        }
    }
}
