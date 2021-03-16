// Fill out your copyright notice in the Description page of Project Settings.


#include "DoorTutoMessageWidget.h"

#include "Kismet/GameplayStatics.h"
#include "wat/GameplayActors/TutoMessageTriggerBox.h"

void UDoorTutoMessageWidget::Init()
{
	Super::Init();

	TArray<AActor*> TutoMessageTriggers;
	UGameplayStatics::GetAllActorsOfClass(GetWorld(), ATutoMessageTriggerBox::StaticClass(), TutoMessageTriggers);

	for(auto* TutoMessageTrigger : TutoMessageTriggers)
	{
		auto* CastedTutoMessageTrigger = Cast<ATutoMessageTriggerBox>(TutoMessageTrigger);

		if(CastedTutoMessageTrigger->TutoMessageClass->IsChildOf(UDoorTutoMessageWidget::StaticClass()))
		{
			CastedTutoMessageTrigger->bShown = true;
		}
	}
}
