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
	AutoPossessAI = EAutoPossessAI::Disabled;
	AIControllerClass = nullptr;

	RootComponent = CreateDefaultSubobject<USceneComponent>(TEXT("Root"));

	CollisionBox = CreateDefaultSubobject<UBoxComponent>(TEXT("CollisionBox"));
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

	bIsShowingGamepadIcon = true;
	RefreshIconWidgetForCurrentDevice(false);

	LocalPlayerCharacter = Cast<AWatPlayerCharacter>(GetWorld()->GetFirstPlayerController()->GetPawn());
	StaticMesh->SetRenderCustomDepth(true);
}

void AInteractable::OnOverlapBegin(UPrimitiveComponent* OverlappedComp, AActor* OtherActor,
                                   UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep,
                                   const FHitResult& SweepResult)
{
	if (HasAuthority())
	{
		const auto PlayerLocalRef = Cast<AWatPlayerCharacter>(OtherActor);
		if (PlayerLocalRef)
		{
			OverlappingPlayers.Add(PlayerLocalRef);
			if (CanInteract(PlayerLocalRef))
			{
				PlayerLocalRef->AddInteractable(this);
			}
		}
	}
}

void AInteractable::OnOverlapEnd(UPrimitiveComponent* OverlappedComp, AActor* OtherActor,
                                 UPrimitiveComponent* OtherComp, int32 OtherBodyIndex)
{
	const auto PlayerLocalRef = Cast<AWatPlayerCharacter>(OtherActor);

	if (PlayerLocalRef && !PlayerLocalRef->IsPendingKill())
	{
		if (KeyIconWidget)
		{
			auto* UserWidgetObject = KeyIconWidget->GetUserWidgetObject();
			if (UserWidgetObject)
			{
				UserWidgetObject->SetVisibility(ESlateVisibility::Hidden);
			}
		}
		if (HasAuthority())
		{
			OverlappingPlayers.Remove(PlayerLocalRef);
			PlayerLocalRef->RemoveInteractable(this);
		}
	}
}

void AInteractable::OnInteraction()
{
	PlayerCharacter->OnInteractDispatcher.Broadcast(this);
}

void AInteractable::Interact(AWatPlayerCharacter* Player)
{
	PlayerCharacter = Player;
	PlayerController = Cast<AWatPlayerController>(Player->GetController());
	OnInteraction();
}

void AInteractable::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);
	DOREPLIFETIME(AInteractable, PlayerCharacter);
	DOREPLIFETIME(AInteractable, PlayerController);
}

bool AInteractable::CanInteract(AWatPlayerCharacter* Player)
{
	return Player != nullptr;
}

int AInteractable::GetStencilIndex()
{
	return 4;
}

void AInteractable::Tick(float DeltaSeconds)
{
	Super::Tick(DeltaSeconds);
	StaticMesh->SetCustomDepthStencilValue(GetStencilIndex()); 
}

void AInteractable::RefreshIconWidgetForCurrentDevice(bool bIsGamePad)
{
	if (bIsShowingGamepadIcon != bIsGamePad)
	{
		Cast<UKeyIconWidget>(KeyIconWidget->GetUserWidgetObject())->SetBrushTexture(
			UWatStaticUtils::GetKeyIcon("Interact", bIsGamePad));
		bIsShowingGamepadIcon = bIsGamePad;
	}
}
