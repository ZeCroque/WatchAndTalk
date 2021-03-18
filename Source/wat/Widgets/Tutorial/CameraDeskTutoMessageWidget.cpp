#include "CameraDeskTutoMessageWidget.h"
#include "wat/Core/WatPlayerController.h"
#include "wat/Core/WatStaticUtils.h"
#include "wat/GameplayActors/Interactables/Desk/CameraDesk.h"

void UCameraDeskTutoMessageWidget::OnInput(FKey PressedInput, EInputEvent EventType)
{
	if(!Controller->GetPawn()->IsA(ACameraDesk::StaticClass()) && PressedInput == UWatStaticUtils::GetKeyForDevice(ActionName, Controller->bIsUsingController))
	{
		RemoveFromParent();
		Controller->TutoMessageWidget = nullptr;		
	}
}
