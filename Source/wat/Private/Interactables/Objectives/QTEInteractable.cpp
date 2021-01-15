// Fill out your copyright notice in the Description page of Project Settings.


#include "Interactables/Objectives/QTEInteractable.h"

#include "QTEWidget.h"
#include "Core/MainPlayerController.h"

void AQTEInteractable::Interact()
{
    AInteractable::Interact();
    AMainPlayerController* PC = Cast<AMainPlayerController>(PlayerController);
    PC->Widget = CreateWidget<UQTEWidget>(PC, EndGameWidgetTemplate);
    PC->Widget->AddToViewport();
    PC->Widget->ParentInteractable = this;
    PC->bIsInMiniGame = true;
}
