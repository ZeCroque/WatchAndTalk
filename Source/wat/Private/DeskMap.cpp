// Fill out your copyright notice in the Description page of Project Settings.


#include "DeskMap.h"

// Sets default values
ADeskMap::ADeskMap()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	RootComponent = CreateDefaultSubobject<USceneComponent>(TEXT("Root"));
	
	MapStaticMesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("MapMesh"));
	MapStaticMesh->SetupAttachment(RootComponent);

	BlendTarget = CreateDefaultSubobject<UCameraComponent>(TEXT("BlendTarget"));
	BlendTarget->SetupAttachment(RootComponent);
}

// Called when the game starts or when spawned
void ADeskMap::BeginPlay()
{
	Super::BeginPlay();
	
}

// Called every frame
void ADeskMap::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

