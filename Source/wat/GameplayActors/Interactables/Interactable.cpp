// Fill out your copyright notice in the Description page of Project Settings.


#include "wat/GameplayActors/Interactables/Interactable.h"
#include "Kismet/GameplayStatics.h"
#include "wat/Core//WatPlayerCharacter.h"
#include "wat/Core/WatPlayerController.h"
#include "Engine/AssetManager.h"
#include "GameFramework/InputSettings.h"
#include "wat/Widgets/KeyIconWidget.h"
#include "Net/UnrealNetwork.h"
#include "wat/Core/WatStaticUtils.h"

// Sets default values
AInteractable::AInteractable()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	AutoPossessAI = EAutoPossessAI::Disabled;
	AIControllerClass = nullptr;

	bInUse = false;

	RootComponent = CreateDefaultSubobject<USceneComponent>(TEXT("Root"));
	
	CollisionBox = CreateDefaultSubobject <UBoxComponent>(TEXT("CollisionBox"));
	CollisionBox->SetCollisionEnabled(ECollisionEnabled::QueryOnly);
	CollisionBox->SetGenerateOverlapEvents(true);
	CollisionBox->OnComponentBeginOverlap.AddDynamic(this, &AInteractable::OnOverlapBegin);
	CollisionBox->OnComponentEndOverlap.AddDynamic(this, &AInteractable::OnOverlapEnd);
	CollisionBox->SetupAttachment(RootComponent);

	StaticMesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("Renderer"));
	StaticMesh->SetupAttachment(RootComponent);

	KeyIconWidget = CreateDefaultSubobject<UWidgetComponent>(TEXT("InteractionIcon"));
	KeyIconWidget->SetupAttachment(RootComponent);
}

// Called when the game starts or when spawned
void AInteractable::BeginPlay()
{
	Super::BeginPlay();

	bCanInteract = true;

	RefreshIconWidgetForCurrentDevice(false);
}

void AInteractable::OnOverlapBegin(UPrimitiveComponent* OverlappedComp, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	if (!PlayerCharacter && bCanInteract)
	{
		PlayerCharacter = Cast<AWatPlayerCharacter>(OtherActor);
		if (PlayerCharacter)
		{
			PlayerCharacter->AddInteractable(this);
			PlayerController = Cast<AWatPlayerController>(PlayerCharacter->GetController());
			if(PlayerController)
			{
			    RefreshIconWidgetForCurrentDevice(PlayerController->bIsUsingController);
			}
		}
	}
}

void AInteractable::OnOverlapEnd(UPrimitiveComponent* OverlappedComp, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex)
{
	if (PlayerCharacter == OtherActor)
	{
		PlayerCharacter->RemoveInteractable(this);
		PlayerCharacter = nullptr;
		PlayerController = nullptr;
	}
}

void AInteractable::OnInteractionBegin()
{
	PlayerCharacter->OnInteractDispatcher.Broadcast(this);
}

void AInteractable::Interact()
{
	if (PlayerCharacter && PlayerController)
	{
		bInUse = true;
		OnInteractionBegin();
		OnInteraction();		
	}
}

void AInteractable::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);
	DOREPLIFETIME(AInteractable, bInUse);
	DOREPLIFETIME(AInteractable, PlayerCharacter);
	DOREPLIFETIME(AInteractable, PlayerController);
}

void AInteractable::RefreshIconWidgetForCurrentDevice(bool bIsGamePad) const
{
	Cast<UKeyIconWidget>(KeyIconWidget->GetUserWidgetObject())->SetBrushTexture(UWatStaticUtils::GetKeyIcon("Interact", bIsGamePad));
}

// Called every frame
void AInteractable::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	if(PlayerCharacter && bCanInteract && !bInUse && PlayerCharacter->IsClosestInteractable(this))
	 {
              KeyIconWidget->GetUserWidgetObject()->SetVisibility(ESlateVisibility::Visible);
			  KeyIconWidget->SetWorldRotation((PlayerCharacter->GetActorForwardVector() * -1.f).Rotation());
              return;
	 }

	KeyIconWidget->GetUserWidgetObject()->SetVisibility(ESlateVisibility::Hidden);

}

