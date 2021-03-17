#include "wat/GameplayActors/Interactables/Objectives/Objective.h"
#include "wat/AI/MonsterCharacter.h"
#include "wat/Core/WatGameState.h"
#include "Kismet/GameplayStatics.h"
#include "wat/Core//WatPlayerCharacter.h"
#include "wat/Core/WatPlayerController.h"
#include "wat/Widgets/MiniGames/QTEWidget.h"

AObjective::AObjective()
{
    ObjectiveIndex = 0;
    bIsOnlyForMechanist = true;
}

bool AObjective::CanInteract(AWatPlayerCharacter* Player)
{
    return Super::CanInteract(Player) && !bCompleted && GameState && GameState->ObjectiveIndex == ObjectiveIndex &&
        bIsOnlyForMechanist == !GameState->bScientistTurn && bIsOnlyForMechanist == Player->IsMechanist();
}

void AObjective::BeginPlay()
{
    Super::BeginPlay();
    GameState = Cast<AWatGameState>(GetWorld()->GetGameState());
}

void AObjective::Tick(float DeltaSeconds)
{
    Super::Tick(DeltaSeconds);

    StaticMesh->SetRenderCustomDepth(CanInteract(LocalPlayerCharacter));
}

void AObjective::EndInteraction(bool bSuccess, bool bShouldWarnMonster)
{
    if (PlayerController)
    {
        if (PlayerController->MiniGameWidget)
        {
            PlayerController->MiniGameWidget->Stop();
        }

        if (bSuccess)
        {
            bCompleted = true;
            PlayerCharacter->RemoveInteractable(this);
        }
        if (bShouldWarnMonster)
        {
            GameState->MonsterCharacter->StartGoalReaction(PlayerCharacter);
        }
        BlendToPlayer();
    }
    UGameplayStatics::SpawnSoundAtLocation(this, EndInteractionSound, GetActorLocation());
}

void AObjective::OnNotFinalBlend()
{
    Super::OnNotFinalBlend();
    PlayerController->MiniGameWidget = CreateWidget<UMiniGameWidget>(PlayerController, MiniGameWidgetClass);
    PlayerController->MiniGameWidget->AddToViewport();
    PlayerController->MiniGameWidget->ParentInteractable = this;
    PlayerController->MiniGameWidget->Start();
}

void AObjective::OnFinalBlend()
{
    if (bCompleted)
    {
        PlayerController->NotifyObjectiveCompleted(this);
    }

    if (!GameState->bGameEnded)
    {
        PlayerController->SetInputMode(FInputModeGameOnly());
    }

    Super::OnFinalBlend();
}

int AObjective::GetStencilIndex()
{
    if (bCompleted)
    {
        return 3;
    }

    if (CanInteract(LocalPlayerCharacter))
    {
        return 1;
    }

    return 2;
}

void AObjective::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
    Super::GetLifetimeReplicatedProps(OutLifetimeProps);
    DOREPLIFETIME(AObjective, bCompleted);
    DOREPLIFETIME(AObjective, bIsOnlyForMechanist);
}
