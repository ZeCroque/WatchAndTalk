#include "wat/GameplayActors/Interactables/Objectives/Objective.h"
#include "wat/AI/MonsterCharacter.h"
#include "wat/Core/WatGameState.h"
#include "Kismet/GameplayStatics.h"
#include "wat/Core//WatPlayerCharacter.h"
#include "wat/Core/WatPlayerController.h"
#include "wat/Widgets/QTEWidget.h"

AObjective::AObjective()
{
	MiniGameWidgetComponent = CreateDefaultSubobject<UWidgetComponent>(TEXT("MiniGameWidget"));
	MiniGameWidgetComponent->SetupAttachment(RootComponent);
}

void AObjective::BeginPlay()
{
        Super::BeginPlay();
        GameState = Cast<AWatGameState>(GetWorld()->GetGameState());
        MiniGameWidget = Cast<UQTEWidget>(MiniGameWidgetComponent->GetUserWidgetObject());
	MiniGameWidget->ParentInteractable = this;
}

void AObjective::EndInteraction(bool bSuccess, bool bShouldWarnMonster)
{
    if (PlayerController)
    {
        bInUse = false;
    	MiniGameWidget->Stop();
    	BlendToPlayer();
    	if(bSuccess)
    	{
                bCompleted = true;
    		bCanInteract = false;
    		PlayerCharacter->RemoveInteractable(this);
    	}
        if(bShouldWarnMonster)
        {
	        GameState->MonsterCharacter->StartGoalReaction(PlayerCharacter);
        }
    }
}

void AObjective::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const 
{
    Super::GetLifetimeReplicatedProps(OutLifetimeProps);
    DOREPLIFETIME(AObjective, bCompleted);
    DOREPLIFETIME(AObjective, MiniGameWidgetComponent);
    DOREPLIFETIME(AObjective, MiniGameWidget);
}
