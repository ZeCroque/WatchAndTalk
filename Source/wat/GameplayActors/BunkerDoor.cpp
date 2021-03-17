// Fill out your copyright notice in the Description page of Project Settings.


#include "BunkerDoor.h"


#include "Components/AudioComponent.h"
#include "Components/BoxComponent.h"
#include "Kismet/GameplayStatics.h"

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
}

// Called when the game starts or when spawned
void ABunkerDoor::BeginPlay()
{
    Super::BeginPlay();
    MeshHeight = DoorMesh->GetStaticMesh()->GetBoundingBox().Max.Z * DoorMesh->GetRelativeScale3D().Z + ZOffset;
    InitialZ = DoorMesh->GetRelativeLocation().Z;
    OverlappingPlayerCount = 0;
    bHasMove = false;
}

// Called every frame
void ABunkerDoor::Tick(float DeltaTime)
{
    Super::Tick(DeltaTime);

    if (OverlappingPlayerCount)
    {
        if (DoorMesh->GetRelativeLocation().Z > (InitialZ + MeshHeight) * -1.f)
        {
            if (AudioComponentAC && AudioComponentAC->IsPlaying() || AudioComponentC && AudioComponentC->IsPlaying())
            {
                if (AudioComponentAC)
                    AudioComponentAC->Stop();
                if (AudioComponentC)
                    AudioComponentC->Stop();
                AudioComponentB =
                    UGameplayStatics::SpawnSoundAtLocation(this, DoorDownFromNotTop, DoorMesh->GetComponentLocation());
            }
            else if (DoorMesh->GetRelativeLocation().Z == InitialZ)
            {
                AudioComponentAB = UGameplayStatics::SpawnSoundAtLocation(
                    this, DoorDownFromTop, DoorMesh->GetComponentLocation());
            }
            DoorMesh->AddRelativeLocation(GetActorUpVector() * -1.f * DoorMovingSpeed * DeltaTime);
            bHasMove = true;
        }
        else if (bHasMove)
        {
            DoorMesh->SetRelativeLocation(GetActorUpVector() * -1.f * (InitialZ + MeshHeight));
            bHasMove = false;
            AudioComponentD =
                UGameplayStatics::SpawnSoundAtLocation(this, DoorEnd, DoorMesh->GetComponentLocation());
        }
    }
    else
    {
        if (DoorMesh->GetRelativeLocation().Z < InitialZ)
        {
            if (AudioComponentAB && AudioComponentAB->IsPlaying() || AudioComponentB && AudioComponentB->IsPlaying())
            {
                if (AudioComponentAB)
                    AudioComponentAB->Stop();
                if (AudioComponentB)
                    AudioComponentB->Stop();
                AudioComponentC =
                    UGameplayStatics::SpawnSoundAtLocation(this, DoorUpFromNotBottom, DoorMesh->GetComponentLocation());
            }
            else if (DoorMesh->GetRelativeLocation().Z <= (InitialZ + MeshHeight) * -1.f)
            {
                AudioComponentAC =
                    UGameplayStatics::SpawnSoundAtLocation(this, DoorUpFromBottom, DoorMesh->GetComponentLocation());
            }
            DoorMesh->AddRelativeLocation(GetActorUpVector() * DoorMovingSpeed * DeltaTime);
            bHasMove = true;
        }
        else if (bHasMove)
        {
            bHasMove = false;
            DoorMesh->SetRelativeLocation(GetActorUpVector() * InitialZ);
            AudioComponentD =
                UGameplayStatics::SpawnSoundAtLocation(this, DoorEnd, DoorMesh->GetComponentLocation());
        }
    }
}

void ABunkerDoor::OnOverlapBegin(UPrimitiveComponent* OverlappedComp, AActor* OtherActor,
                                 UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep,
                                 const FHitResult& SweepResult)
{
    ++OverlappingPlayerCount;
    if (OverlappingPlayerCount == 1)
    {
        UGameplayStatics::SpawnSoundAtLocation(this, ButtonOn, OverlappedComp->GetComponentLocation());
    }
}

void ABunkerDoor::OnOverlapEnd(UPrimitiveComponent* OverlappedComp, AActor* OtherActor, UPrimitiveComponent* OtherComp,
                               int32 OtherBodyIndex)
{
    --OverlappingPlayerCount;
    if (OverlappingPlayerCount == 0)
    {
        UGameplayStatics::SpawnSoundAtLocation(this, ButtonOn, OverlappedComp->GetComponentLocation());
    }
}
