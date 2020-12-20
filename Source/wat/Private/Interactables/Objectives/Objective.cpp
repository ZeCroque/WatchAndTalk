#include "Interactables/Objectives/Objective.h"
#include "Characters/MonsterCharacter.h"
#include "Core/MainGameState.h"
#include "Kismet/GameplayStatics.h"

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

	bCompleted = true;
	InteractionEnd();
}

void AObjective::InteractionEnd()
{
    if (PlayerController)
    {
        if (GameState->AreAllObjectivesCompleted() == true)
        {
            EndGame();
        }
        else
        {
	        // ReSharper disable once CppExpressionWithoutSideEffects
	        OnObjectiveInteractionEnd.ExecuteIfBound();
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
