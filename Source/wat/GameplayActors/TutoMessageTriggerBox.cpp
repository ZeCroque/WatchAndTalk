// Fill out your copyright notice in the Description page of Project Settings.


#include "TutoMessageTriggerBox.h"


#include "Blueprint/UserWidget.h"
#include "Components/BoxComponent.h"
#include "wat/Core/WatPlayerCharacter.h"
#include "wat/Core/WatPlayerController.h"

ATutoMessageTriggerBox::ATutoMessageTriggerBox()
{
	RootComponent = CreateDefaultSubobject<USceneComponent>("RootComponent");

	CollisionBox = CreateDefaultSubobject<UBoxComponent>("CollisionBox");
	CollisionBox->SetupAttachment(RootComponent);

	CollisionBox->OnComponentBeginOverlap.AddDynamic(this, &ATutoMessageTriggerBox::OnOverlapBegin);

	bShown = false;
}

void ATutoMessageTriggerBox::OnOverlapBegin(UPrimitiveComponent* OverlappedComp, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	auto* PlayerCharacter = Cast<AWatPlayerCharacter>(OtherActor);
    if (!bShown && PlayerCharacter && PlayerCharacter->IsLocallyControlled())
    {	
    	auto* PlayerController = Cast<AWatPlayerController>(PlayerCharacter->GetController());

    	if(PlayerController->TutoMessageWidget)
    	{
    		PlayerController->TutoMessageWidget->RemoveFromParent();
    		PlayerController->TutoMessageWidget = nullptr;
    	}
    	PlayerController->TutoMessageWidget = CreateWidget<UTutoMessageWidget>(PlayerController, TutoMessageClass);
    	PlayerController->TutoMessageWidget->Controller = PlayerController;
    	PlayerController->TutoMessageWidget->Character = PlayerCharacter;
    	PlayerController->TutoMessageWidget->AddToViewport();
    	PlayerController->TutoMessageWidget->Init();
    	bShown = true;
    }
}
