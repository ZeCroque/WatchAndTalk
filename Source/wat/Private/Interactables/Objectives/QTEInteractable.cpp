// Fill out your copyright notice in the Description page of Project Settings.


#include "Interactables/Objectives/QTEInteractable.h"
#include "QTEWidget.h"
#include "Core/MainPlayerController.h"

void AQTEInteractable::OnInteractionBegin()
{
	Super::OnInteractionBegin();
	Cast<UQTEWidget>(MiniGameWidget->GetUserWidgetObject())->Start();
        PlayerController->bIsInMiniGame = true;
}
