// Fill out your copyright notice in the Description page of Project Settings.


#include "BunkerDoor.h"

#include "Components/BoxComponent.h"

// Sets default values
ABunkerDoor::ABunkerDoor()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	RootComponent = CreateDefaultSubobject<USceneComponent>(TEXT("Root"));

	DoorMesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("Door Mesh"));
        DoorMesh->SetupAttachment(RootComponent);

	DoorTrigger = CreateDefaultSubobject<UBoxComponent>(TEXT("Door Trigger"));
	DoorTrigger->SetupAttachment(RootComponent);

	DoorMovingSpeed = 175.f;

	DoorTrigger->OnComponentBeginOverlap.AddDynamic(this, &ABunkerDoor::OnOverlapBegin);
	DoorTrigger->OnComponentEndOverlap.AddDynamic(this, &ABunkerDoor::OnOverlapEnd);

	bIsActivated = false;
}

// Called when the game starts or when spawned
void ABunkerDoor::BeginPlay()
{
	Super::BeginPlay();
	MeshHeight = DoorMesh->GetStaticMesh()->GetBoundingBox().Max.Z * 2.f * DoorMesh->GetRelativeScale3D().Z;
        InitialZ = DoorMesh->GetRelativeLocation().Z;
}

// Called every frame
void ABunkerDoor::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
	if(bIsActivated)
	{
		if(DoorMesh->GetRelativeLocation().Z > (InitialZ + MeshHeight) * -1.f) 
		{
		    DoorMesh->AddRelativeLocation(GetActorUpVector() * -1.f * DoorMovingSpeed * DeltaTime);
		}
		else if(DoorMesh->GetRelativeLocation().Z < (InitialZ + MeshHeight) * -1.f)
		{
		    DoorMesh->SetRelativeLocation(GetActorUpVector() * -1.f * (InitialZ + MeshHeight));
		}
	}
	else if(!bIsActivated)	
	{
		if(DoorMesh->GetRelativeLocation().Z < InitialZ) 
		{
		    DoorMesh->AddRelativeLocation(GetActorUpVector() * DoorMovingSpeed * DeltaTime);
		}
		else 
		{
		    DoorMesh->SetRelativeLocation(GetActorUpVector() * InitialZ);
		}
		
	}

}

void ABunkerDoor::OnOverlapBegin(UPrimitiveComponent* OverlappedComp, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	bIsActivated = true;
}

void ABunkerDoor::OnOverlapEnd(UPrimitiveComponent* OverlappedComp, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex)
{
	bIsActivated = false;
}

