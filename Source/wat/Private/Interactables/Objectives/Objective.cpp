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
        EndGameWidget->AddToViewport();
        PlayerController->SetInputMode(FInputModeUIOnly());
        PlayerController->bShowMouseCursor = true;
    }
}

AObjective::AObjective()
{
	MiniGameWidget = CreateDefaultSubobject<UWidgetComponent>(TEXT("MiniGameWidget"));
	MiniGameWidget->SetupAttachment(RootComponent);
}

void AObjective::BeginPlay()
{
        Super::BeginPlay();
        GameState = Cast<AMainGameState>(GetWorld()->GetGameState());      
}

void AObjective::OnInteractionBegin()
{
	PlayerController->Possess(this);
	PlayerController->SetViewTargetWithBlend(this, BlendTime);
        Cast<UQTEWidget>(MiniGameWidget->GetUserWidgetObject())->ParentInteractable = this;
}

void AObjective::EndInteraction(bool bSuccess, bool bShouldWarnMonster)
{
    if (PlayerController)
    {
        bInUse = false;
    	if(bSuccess)
    	{
    		bCompleted = true;
    		bCanInteract = false;
	        if (GameState->AreAllObjectivesCompleted() == true)
	        {
	            EndGame();
	        }
    	}
        if(bShouldWarnMonster)
        {
	        GameState->MonsterCharacter->StartGoalReaction(PlayerCharacter);
        }
        PlayerController->Possess(PlayerCharacter);
    	PlayerController->SetViewTargetWithBlend(PlayerCharacter, BlendTime);
    	PlayerController->SetInputMode(FInputModeGameOnly());
    }
}

void AObjective::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const 
{
    Super::GetLifetimeReplicatedProps(OutLifetimeProps);
    DOREPLIFETIME(AObjective, bCompleted);
}
