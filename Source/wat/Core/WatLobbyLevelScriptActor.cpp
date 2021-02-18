// Fill out your copyright notice in the Description page of Project Settings.


#include "WatLobbyLevelScriptActor.h"

void AWatLobbyLevelScriptActor::PostInitializeComponents()
{
  Super::PostInitializeComponents();
  auto* PlayerController = GetWorld()->GetFirstPlayerController();
  if (PlayerController)
  {
      PlayerController->EnableInput(nullptr);
      PlayerController->SetInputMode(FInputModeGameOnly());
  }
}
