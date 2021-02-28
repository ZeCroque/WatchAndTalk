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

	ObjectiveIndex = 0;
	bIsOnlyForMechanist = true;
}

bool AObjective::CanInteract(AWatPlayerCharacter* Player)
{
	return Super::CanInteract(Player) && !bCompleted && GameState->ObjectiveIndex == ObjectiveIndex && bIsOnlyForMechanist == !GameState->bScientistTurn && bIsOnlyForMechanist == Player->IsMechanist();
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
    	MiniGameWidget->Stop();  	
    	if(bSuccess)
    	{
            bCompleted = true;
    		PlayerCharacter->RemoveInteractable(this);
    	}
        if(bShouldWarnMonster)
        {
	        GameState->MonsterCharacter->StartGoalReaction(PlayerCharacter);
        }
    	BlendToPlayer();
    }
}

void AObjective::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const 
{
    Super::GetLifetimeReplicatedProps(OutLifetimeProps);
    DOREPLIFETIME(AObjective, bCompleted);
    DOREPLIFETIME(AObjective, MiniGameWidgetComponent);
    DOREPLIFETIME(AObjective, MiniGameWidget);
}
