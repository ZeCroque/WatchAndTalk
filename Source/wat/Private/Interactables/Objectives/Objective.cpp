#include "Interactables/Objectives/Objective.h"
#include "Characters/MonsterCharacter.h"
#include "Core/MainGameState.h"
#include "Kismet/GameplayStatics.h"
#include "Characters/PlayerCharacter.h"
#include "Core/MainPlayerController.h"


void AObjective::EndGame_Implementation()
{
    auto* EndGameWidget = NewObject<UUserWidget>(this, EndGameWidgetTemplate);
    if (EndGameWidget != nullptr && !EndGameWidget->GetIsVisible())
    {
    	GameState->bGameEnded = true;
        EndGameWidget->AddToViewport();
        PlayerController->SetInputMode(FInputModeUIOnly());
        PlayerController->bShowMouseCursor = true;
    }
}

AObjective::AObjective()
{
	MiniGameWidgetComponent = CreateDefaultSubobject<UWidgetComponent>(TEXT("MiniGameWidget"));
	MiniGameWidgetComponent->SetupAttachment(RootComponent);
}

void AObjective::BeginPlay()
{
        Super::BeginPlay();
        GameState = Cast<AMainGameState>(GetWorld()->GetGameState());      
}

void AObjective::OnInteractionBegin()
{
	Super::OnInteractionBegin();
	if(!MiniGameWidget)
	{
	    MiniGameWidget = Cast<UQTEWidget>(MiniGameWidgetComponent->GetUserWidgetObject());
		MiniGameWidget->ParentInteractable = this;
    }
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
	        if (GameState->AreAllObjectivesCompleted() == true)
	        {
	            EndGame();
	        }
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
}
