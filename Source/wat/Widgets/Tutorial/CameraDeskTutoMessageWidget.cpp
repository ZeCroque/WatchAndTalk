#include "CameraDeskTutoMessageWidget.h"
#include "wat/Core/WatPlayerController.h"
#include "wat/GameplayActors/Interactables/Desk/CameraDesk.h"

void UCameraDeskTutoMessageWidget::OnInput(FKey PressedInput)
{
	if(!Controller->GetPawn()->IsA(ACameraDesk::StaticClass()))
	{
		RemoveFromParent();
		Controller->TutoMessageWidget = nullptr;		
	}
}
