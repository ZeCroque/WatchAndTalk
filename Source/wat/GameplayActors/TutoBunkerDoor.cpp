// Fill out your copyright notice in the Description page of Project Settings.


#include "TutoBunkerDoor.h"
#include "Components/BoxComponent.h"

// Sets default values
ATutoBunkerDoor::ATutoBunkerDoor()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	RootComponent = CreateDefaultSubobject<USceneComponent>(TEXT("Root"));

	DoorMesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("Door Mesh"));
    DoorMesh->SetupAttachment(RootComponent);

	DoorTrigger = CreateDefaultSubobject<UBoxComponent>(TEXT("Door Trigger"));
	DoorTrigger->SetupAttachment(RootComponent);

	DoorMovingSpeed = 175.f;

	DoorTrigger->OnComponentBeginOverlap.AddDynamic(this, &ATutoBunkerDoor::OnOverlapBegin);
	DoorTrigger->OnComponentEndOverlap.AddDynamic(this, &ATutoBunkerDoor::OnOverlapEnd);

}

// Called when the game starts or when spawned
void ATutoBunkerDoor::BeginPlay()
{
	Super::BeginPlay();
	OverlappingPlayerCount = 0;
}

// Called every frame
void ATutoBunkerDoor::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
	if(OverlappingPlayerCount == 2)
	{
		if(DoorMesh->GetRelativeLocation().Z < 0) 
		{
		    DoorMesh->AddRelativeLocation(GetActorUpVector() * DoorMovingSpeed * DeltaTime);
		}
		else if(DoorMesh->GetRelativeLocation().Z >= 0)
		{
		    DoorMesh->SetRelativeLocation(FVector(0.f,0.f,0.f));
		}
	}
}

void ATutoBunkerDoor::OnOverlapBegin(UPrimitiveComponent* OverlappedComp, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	++OverlappingPlayerCount;
}

void ATutoBunkerDoor::OnOverlapEnd(UPrimitiveComponent* OverlappedComp, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex)
{
	--OverlappingPlayerCount;
}



