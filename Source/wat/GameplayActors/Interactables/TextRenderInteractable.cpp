// Fill out your copyright notice in the Description page of Project Settings.


#include "TextRenderInteractable.h"
#include "Components/TextRenderComponent.h"

ATextRenderInteractable::ATextRenderInteractable()
{
	TextRender = CreateDefaultSubobject<UTextRenderComponent>("TextRender");
}
