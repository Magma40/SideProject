#include "CameraManagerMain.h"

#include "CameraPoint.h"
#include "PlayerCamera.h"
#include "ActionStack/CameraActionStack/CameraActionStack.h"
#include "Kismet/GameplayStatics.h"

ACameraManagerMain::ACameraManagerMain()  { PrimaryActorTick.bCanEverTick = true; }


void ACameraManagerMain::BeginPlay()
{
	AActor* Camera = UGameplayStatics::GetActorOfClass(GetWorld(), APlayerCamera::StaticClass());
	if (IsValid(Camera))
	{
		if (APlayerCamera* CameraActor = Cast<APlayerCamera>(Camera)) PlayerCamera = CameraActor;
	}
	if (!IsValid(PlayerCamera)) return;
	
	TArray<UCameraActionStack* >Components;
	PlayerCamera->GetComponents<UCameraActionStack>(Components, true);
	if (!Components.IsEmpty() && IsValid(Components[0])) CameraActionStack = Components[0];
	if (!IsValid(CameraActionStack)) return;

	UGameplayStatics::GetPlayerController(GetWorld(), 0)->SetViewTargetWithBlend(PlayerCamera);
	if (IsValid(BaseCameraPoint)) CameraActionStack->PushToCameraPointAction(BaseCameraPoint);
}

void ACameraManagerMain::SetNewCameraPoint(ACameraPoint* NewCameraPoint) const
{
	if (IsValid(NewCameraPoint)) CameraActionStack->PushToCameraPointAction(NewCameraPoint);
}
