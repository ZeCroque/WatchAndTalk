// Fill out your copyright notice in the Description page of Project Settings.


#include "StartChooserInteractable.h"


#include "wat/Core/WatGameState.h"
#include "wat/Core/WatPlayerCharacter.h"

void AStartChooserInteractable::OnInteraction()
{
	bInUse = false;
	PlayerCharacter->SwitchStartMode();
}
