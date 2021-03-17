// Fill out your copyright notice in the Description page of Project Settings.


#include "SprintTutoMessageWidget.h"
#include "Kismet/GameplayStatics.h"
#include "wat/Core/WatPlayerCharacter.h"

void USprintTutoMessageWidget::Init()
{
    Super::Init();
    UGameplayStatics::PlaySound2D(GetWorld(), MonsterScreamSound);
    Character->bSprintEnabled = true;
}
