// Fill out your copyright notice in the Description page of Project Settings.


#include "ObjectiveTutoMessageWidget.h"

#include "wat/Core/WatPlayerCharacter.h"

void UObjectiveTutoMessageWidget::Init()
{
	Super::Init();
	Character->bObjectiveInputEnabled = true;
}
