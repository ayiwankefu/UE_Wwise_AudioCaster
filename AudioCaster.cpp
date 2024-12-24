#include "AudioCaster.h"
#include "AkGameplayStatics.h"
#include "AkAudioEvent.h"
#include "AkSwitchValue.h"
#include "AkComponent.h"
#include "AkAmbientSound.h"
#include "DrawDebugHelpers.h"
#include "TimerManager.h"


AAudioCaster::AAudioCaster()
{

	PrimaryActorTick.bCanEverTick = true;
	AkAudioDevice = FAkAudioDevice::Get();
	AkComponent = CreateDefaultSubobject<UAkComponent>(TEXT("AkComponent"));
	RootComponent = AkComponent;
	
}


void AAudioCaster::BeginPlay()
{
	Super::BeginPlay();
	
}

void AAudioCaster::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
	SetRTPCOnChanged();
	SetSwitchValueOnChanged();
}

void AAudioCaster::AC_PlayEvent(UAkAudioEvent* AkEvent, class FOnAkPostEventCallback PostEventCallback)
{
	int32 playingid = UAkGameplayStatics::PostEvent(AkEvent, this, 0, PostEventCallback, true);
	if (GEngine)
	{
		if (playingid == 0)
		{
			GEngine->AddOnScreenDebugMessage(1, 2.f, FColor::Cyan, TEXT("AK_INVALID_PLAYING_ID"));
		}
	}
}


void AAudioCaster::AC_SetRtpc(UAkRtpc* AkRtpc, float Value, int32 InterpolationTimeMs)
{
	UAkGameplayStatics::SetRTPCValue(AkRtpc, Value, InterpolationTimeMs, this, "");
}

void AAudioCaster::AC_SetSwitch(UAkSwitchValue* AkSwitchValue)
{
	UAkGameplayStatics::SetSwitch(AkSwitchValue, this, "", "");
}

void AAudioCaster::AC_PlayEventArray(
	UAkAudioEvent* AkEvent,
	UAkRtpc* AkRtpc,
	float Value,
	int32 InterpolationTimeMs,
	UAkSwitchValue* AkSwitchValue,
	class FOnAkPostEventCallback PostEventCallback)
{
	//AC_SetRtpc and AC_SetSwitch needs to be before AC_PlayEvent
	AC_SetRtpc(AkRtpc, Value, InterpolationTimeMs);
	AC_SetSwitch(AkSwitchValue);
	AC_PlayEvent(AkEvent, PostEventCallback);
}

void AAudioCaster::OnEventEndFunction()
{
	if (AudioCasterArray.Num() != 0)
	{
		if (AudioCasterArray[0].bIfLoop)
		{
			AC_PlayEvent(AudioCasterArray[0].AkEvent, AudioCasterArray[0].PostEventCallback);
		}
	}
}

void AAudioCaster::SetRTPCOnChanged()
{
	for (FAudioCasterStruct CasterSection : AudioCasterArray)
	{
		if (CasterSection.RtpcValue != CasterSection._tempRtpcValue)
		{
			CasterSection._tempRtpcValue = CasterSection.RtpcValue;
			UAkGameplayStatics::SetRTPCValue(CasterSection.AkRtpc, CasterSection.RtpcValue, 0, this, "");
		}
	}

}

void AAudioCaster::SetSwitchValueOnChanged()
{
	for (FAudioCasterStruct CasterSection : AudioCasterArray)
	{
		//Check if pointer exist, because we're using GetWwiseName() function to store switch name
		if (CasterSection.AkSwitchValue)
		{
			FName AkSwitchName = CasterSection.AkSwitchValue->GetWwiseName();
			if (AkSwitchName != CasterSection._tempSwitchName)
			{
				UAkGameplayStatics::SetSwitch(CasterSection.AkSwitchValue, this, CasterSection.AkSwitchValue->GetWwiseGroupName(), AkSwitchName);
				CasterSection._tempSwitchName = AkSwitchName;
			}
		}
	}
}

//Buttons
void AAudioCaster::PlayButton()
{
	if (AudioCasterArray.Num() != 0)
	{
		FAudioCasterStruct FirstCasterSection = AudioCasterArray[0];
		FirstCasterSection.PostEventCallback.BindUFunction(this, "OnEventEndFunction");
		//AC_SetRtpc and AC_SetSwitch needs to be before AC_PlayEvent
		AC_SetRtpc(FirstCasterSection.AkRtpc, FirstCasterSection.RtpcValue, FirstCasterSection.InterpolationTimeMs);
		AC_SetSwitch(FirstCasterSection.AkSwitchValue);
		AC_PlayEvent(FirstCasterSection.AkEvent, FirstCasterSection.PostEventCallback);
	}
}


void AAudioCaster::PlayArrayButton()
{
	for (FAudioCasterStruct CasterSection : AudioCasterArray)
	{
		FTimerDelegate Delegate = FTimerDelegate::CreateUObject(
			this,
			&AAudioCaster::AC_PlayEventArray,
			CasterSection.AkEvent,
			CasterSection.AkRtpc,
			CasterSection.RtpcValue,
			CasterSection.InterpolationTimeMs,
			CasterSection.AkSwitchValue,
			CasterSection.PostEventCallback);
		if (GetWorld())
		{
			if (CasterSection.ExecuteDelayTime < 0)
			{
				UE_LOG(LogTemp, Warning, TEXT("ExexuteDelayTime can't be a negative number!!! Please check!!!"))
			}
			else
			{
				//SetTimer() param requires a float type InRate, which has to greater than zero.
				float ExexuteDelayTime = 0.01f + CasterSection.ExecuteDelayTime;
				GetWorld()->GetTimerManager().ClearTimer(CasterSection.TimeHandle);
				GetWorld()->GetTimerManager().SetTimer(CasterSection.TimeHandle, Delegate, ExexuteDelayTime, false);
			}
		}
	}
}

void AAudioCaster::StopButton()
{
	UAkGameplayStatics::StopActor(this);
	
}


void AAudioCaster::ShowRadius()
{
	if (AudioCasterArray.Num() != 0)
	{
		float radius = AudioCasterArray[0].AkEvent->MaxAttenuationRadius;
		UWorld* World = GetWorld();
		if (World)
		{
			DrawDebugSphere(GetWorld(), this->GetActorLocation(), radius * RadiosScalingFactor, 12, FColor::Red, false, 3.f, 0, 20.f * RadiosScalingFactor);
		}
	}
}

void AAudioCaster::EnableMonitoring()
{
	if (Microphone != nullptr && Loudspeaker != nullptr)
	{
		MicrophoneAkComponent = Microphone->AkComponent;
		LoudspeakerAkComponent = Loudspeaker->AkComponent;

		//发声体的听者设为PlayerCamera
		UAkComponentSet& DefaultListeners = AkAudioDevice->GetDefaultListeners();
		TArray<UAkComponent*> ListenerArray;
		for (const TWeakObjectPtr<UAkComponent>& Listener : DefaultListeners)
		{
			ListenerArray.Add(Listener.Get());
		}
		AkComponent->SetListeners(ListenerArray);


		//发声体的听者设为Microphone，通过SetAuxSends
		TArray<AkAuxSendValue> AuxSendValues;
		AkGameObjectID MicrophoneID = Microphone->AkComponent->GetAkGameObjectID();
		AkAuxSendValue _auxSend;
		_auxSend.auxBusID = AK::SoundEngine::GetIDFromString("Microphone");
		_auxSend.fControlValue = 1.f;
		_auxSend.listenerID = MicrophoneID;
		AuxSendValues.Add(_auxSend);
		AkAudioDevice->SetAuxSends(AkComponent, AuxSendValues);

		//Microphone的听者设为Loudspeaker
		TArray<UAkComponent*> listenerMicrophoneToLoudspeaker;
		listenerMicrophoneToLoudspeaker.Add(LoudspeakerAkComponent);
		MicrophoneAkComponent->SetListeners(listenerMicrophoneToLoudspeaker);

		//Loudspeaker的听者设为PlayerCamera
		LoudspeakerAkComponent->SetListeners(ListenerArray);
		LoudspeakerAkComponent->SetListeners(ListenerArray);
	}
}
