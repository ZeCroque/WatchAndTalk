// Fill out your copyright notice in the Description page of Project Settings.


#include "CrouchTutoMessageWidget.h"

#include "wat/Core/WatPlayerCharacter.h"

void UCrouchTutoMessageWidget::Init()
{
	Super::Init();
	Character->bCrouchEnabled = true;
}
