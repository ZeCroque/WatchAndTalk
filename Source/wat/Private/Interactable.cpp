// Fill out your copyright notice in the Description page of Project Settings.


#include "Interactable.h"


#include "Kismet/GameplayStatics.h"
#include "PlayerCharacter.h"

// Sets default values
AInteractable::AInteractable()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;
	bIsOverlappingWithPlayer = false;

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

	PlayerCharacter = Cast<APlayerCharacter>(UGameplayStatics::GetPlayerPawn(GetWorld(), 0));
	PlayerController = UGameplayStatics::GetPlayerController(this, 0);
}

void AInteractable::OnOverlapBegin(UPrimitiveComponent* OverlappedComp, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	if(OtherActor == PlayerCharacter)
	{
		if(PlayerCharacter->AddInteractable(this))
		{
			InteractableWidget->GetUserWidgetObject()->SetVisibility(ESlateVisibility::Visible);
		}
		bIsOverlappingWithPlayer = true;
	}

}

void AInteractable::OnOverlapEnd(UPrimitiveComponent* OverlappedComp, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex)
{	
	if (OtherActor == PlayerCharacter)
	{
		PlayerCharacter->RemoveInteractable(this);
		InteractableWidget->GetUserWidgetObject()->SetVisibility(ESlateVisibility::Hidden);
		bIsOverlappingWithPlayer = false;
	}
}

void AInteractable::Interact()
{
	APlayerController* OurPlayerController = UGameplayStatics::GetPlayerController(this, 0);
	OurPlayerController->SetViewTargetWithBlend(this, BlendTime);
}

// Called every frame
void AInteractable::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	if(bIsOverlappingWithPlayer)
	{
		if(PlayerCharacter->IsClosestInteractable(this))
		{
			InteractableWidget->GetUserWidgetObject()->SetVisibility(ESlateVisibility::Visible);
		}
		else
		{
			InteractableWidget->GetUserWidgetObject()->SetVisibility(ESlateVisibility::Hidden);
		}
	}
}

