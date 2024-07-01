#include "AudioCaster.h"
#include "AkGameplayStatics.h"
#include "AkAudioEvent.h"
#include "AkSwitchValue.h"
#include "AkComponent.h"
#include "DrawDebugHelpers.h"

AAudioCaster::AAudioCaster()
{
 	
	PrimaryActorTick.bCanEverTick = true;
	AkComponent = CreateDefaultSubobject<UAkComponent>(TEXT("AkComponent"));
	RootComponent = AkComponent;
}


void AAudioCaster::BeginPlay()
{
	Super::BeginPlay();
	ERTPCValueType OutputValueType = ERTPCValueType::Default;
	UAkGameplayStatics::GetRTPCValue(AkRtpc, 0, ERTPCValueType::Default, RtpcValue, OutputValueType, this, "");
	_tempRtpcValue = RtpcValue;

}


void AAudioCaster::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
	SetRTPCOnChanged();
	SetSwitchValueOnChanged();
}

void AAudioCaster::OnEventEndFunction()
{
	if (bIfLoop)
	{
		OnPlaySoundButtonClicked();
	}
}

void AAudioCaster::OnPlaySoundButtonClicked()
{

	PostEventCallback.BindUFunction(this, "OnEventEndFunction");
	int32 playingid = UAkGameplayStatics::PostEvent(this->AkEvent, this, 1, PostEventCallback, true);
	if (GEngine)
	{
		if (playingid == 0)
		{
			GEngine->AddOnScreenDebugMessage(1, 2.f, FColor::Cyan, TEXT("AK_INVALID_PLAYING_ID"));
		}
	}
}

void AAudioCaster::OnSetRTPCButtonClicked()
{
	UAkGameplayStatics::SetRTPCValue(AkRtpc, RtpcValue, 0, this, "");
}

void AAudioCaster::OnSetSwitchButtonClicked()
{
	UAkGameplayStatics::SetSwitch(AkSwitchValue, this, "", "");
}

void AAudioCaster::OnShowRadiusClicked()
{
	if (AkEvent)
	{
		float radius = AkEvent->MaxAttenuationRadius;
		UWorld* World = GetWorld();
		if (World)
		{
			DrawDebugSphere(GetWorld(), this->GetActorLocation(), radius*RadiosScalingFactor, 12, FColor::Red, false, 3.f, 0, 20.f*RadiosScalingFactor);
		}
	}
}

void AAudioCaster::SetRTPCOnChanged()
{
	if (RtpcValue != _tempRtpcValue)
	{
		_tempRtpcValue = RtpcValue;
		UAkGameplayStatics::SetRTPCValue(AkRtpc, RtpcValue, 0, this, "");
	}
}

void AAudioCaster::SetSwitchValueOnChanged()
{
	//check if pointer exist, because we're using GetWwiseName() function to store switch name
	if (AkSwitchValue)
	{
		FName AkSwitchName = AkSwitchValue->GetWwiseName();
		if (AkSwitchName != _tempSwitchName)
		{
			UAkGameplayStatics::SetSwitch(AkSwitchValue, this, AkSwitchValue->GetWwiseGroupName(), AkSwitchName);
			_tempSwitchName = AkSwitchName;
		}
	}
}





