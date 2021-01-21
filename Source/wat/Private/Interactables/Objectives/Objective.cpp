#include "Interactables/Objectives/Objective.h"
#include "Characters/MonsterCharacter.h"
#include "Core/MainGameState.h"
#include "Kismet/GameplayStatics.h"
#include "Characters/PlayerCharacter.h"


void AObjective::EndGame_Implementation()
{
    auto* EndGameWidget = NewObject<UUserWidget>(this, EndGameWidgetTemplate);
    if (EndGameWidget != nullptr && !EndGameWidget->GetIsVisible())
    {
        EndGameWidget->AddToViewport();
        PlayerController = UGameplayStatics::GetPlayerController(GetWorld(), 0);
        PlayerController->SetInputMode(FInputModeUIOnly());
        PlayerController->bShowMouseCursor = true;
    }
}

void AObjective::Interact()
{
    Super::Interact();
    bCanInteract = false;
	bCompleted = true;
	InteractionEnd();
}

AObjective::AObjective()
{
    bCanInteract = true;
}

void AObjective::InteractionEnd()
{
    if (PlayerController)
    {
        bCanInteract = false;
        if (GameState->AreAllObjectivesCompleted() == true)
        {
            EndGame();
        }
        else
        {
	        // ReSharper disable once CppExpressionWithoutSideEffects
	        OnObjectiveInteractionEnd.ExecuteIfBound(PlayerCharacter);
        }
    }
}

void AObjective::BeginPlay()
{
	Super::BeginPlay();
	
    GameState = Cast<AMainGameState>(GetWorld()->GetGameState());
	OnObjectiveInteractionEnd.BindUObject(GameState->MonsterCharacter, &AMonsterCharacter::StartGoalReaction);	
}

void AObjective::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const 
{
    Super::GetLifetimeReplicatedProps(OutLifetimeProps);
    DOREPLIFETIME(AObjective, bCompleted);
}
