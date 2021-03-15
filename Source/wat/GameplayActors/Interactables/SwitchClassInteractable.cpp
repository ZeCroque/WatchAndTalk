// Fill out your copyright notice in the Description page of Project Settings.


#include "SwitchClassInteractable.h"
#include "wat/Core/WatPlayerCharacter.h"

void ASwitchClassInteractable::OnInteraction()
{
	PlayerCharacter->SwitchPlayersClasses();
}
