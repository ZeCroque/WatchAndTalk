// Fill out your copyright notice in the Description page of Project Settings.


#include "Interactables/Interactable.h"


#include "Kismet/GameplayStatics.h"
#include "Characters/PlayerCharacter.h"
#include "Net/UnrealNetwork.h"

// Sets default values
AInteractable::AInteractable()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;
	bInUse = false;

	BlendTime = 1.0f;
	
	Root = CreateDefaultSubobject<USceneComponent>(TEXT("Root"));
	SetRootComponent(Root);
	
	CollisionBox = CreateDefaultSubobject <UBoxComponent>(TEXT("CollisionBox"));
	CollisionBox->SetCollisionEnabled(ECollisionEnabled::QueryOnly);
	CollisionBox->SetGenerateOverlapEvents(true);
	CollisionBox->OnComponentBeginOverlap.AddDynamic(this, &AInteractable::OnOverlapBegin);
	CollisionBox->OnComponentEndOverlap.AddDynamic(this, &AInteractable::OnOverlapEnd);
	CollisionBox->SetupAttachment(RootComponent);

	StaticMesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("Renderer"));
	StaticMesh->SetupAttachment(RootComponent);

	InteractableWidget = CreateDefaultSubobject<UWidgetComponent>(TEXT("InteractionIcon"));
	InteractableWidget->SetupAttachment(RootComponent);
	
	InteractableCamera = CreateDefaultSubobject<UCameraComponent>(TEXT("InteractableCamera"));
	InteractableCamera->SetupAttachment(RootComponent);
	
	
}

// Called when the game starts or when spawned
void AInteractable::BeginPlay()
{
	Super::BeginPlay();
}

void AInteractable::OnOverlapBegin(UPrimitiveComponent* OverlappedComp, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	if (!PlayerCharacter)
	{
		PlayerCharacter = Cast<APlayerCharacter>(OtherActor);
		if (PlayerCharacter)
		{
			PlayerCharacter->AddInteractable(this);
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

void AInteractable::Interact()
{
	if (PlayerCharacter)
	{
		PlayerController = Cast<APlayerController>(PlayerCharacter->GetController());
		if (PlayerController)
		{
			bInUse = true;
			PlayerController->SetViewTargetWithBlend(this, BlendTime);
		}
	}
}

void AInteractable::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);
	DOREPLIFETIME(AInteractable, bInUse);
}

// Called every frame
void AInteractable::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	if(PlayerCharacter && !bInUse && PlayerCharacter->IsClosestInteractable(this))
	 {
              InteractableWidget->GetUserWidgetObject()->SetVisibility(ESlateVisibility::Visible);
              return;
	 }

	InteractableWidget->GetUserWidgetObject()->SetVisibility(ESlateVisibility::Hidden);

}

