//A simple soundcaster for UE editor
//Author: Leafxie

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "AkGameplayTypes.h"
#include "AudioCaster.generated.h"

class UAkAudioEvent;
class UAkRtpc;
class UAkStateValue;
class UAkSwitchValue;
class AAkAmbientSound;
struct FTimerHandle;


USTRUCT(BlueprintType)
struct FAudioCasterStruct
{
	GENERATED_BODY()

	//Value has to greater than 0, for it is used in a timer
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "AudioCaster")
	float ExecuteDelayTime = 0.f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "AudioCaster")
	UAkAudioEvent* AkEvent;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "AudioCaster")
	UAkRtpc* AkRtpc;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "AudioCaster")
	float RtpcValue;

	//Duration during which the Game Parameter is interpolated towards its default value(in ms)
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "AudioCaster")
	int32 InterpolationTimeMs;

	UPROPERTY(EditAnywhere, Category = "AudioCaster")
	UAkSwitchValue* AkSwitchValue;

	//Temporarily store Rtpc value
	UPROPERTY()
	float _tempRtpcValue;

	//Temporarily store Switch name
	UPROPERTY()
	FName _tempSwitchName;

	//Only works during runtime
	UPROPERTY(EditAnywhere, Category = "AudioCaster")
	bool bIfLoop = false;

	//Declare delegate
	class FOnAkPostEventCallback PostEventCallback;

	FTimerHandle TimeHandle;
};

UCLASS()
class AUDIOTEST_API AAudioCaster : public AActor
{
	GENERATED_BODY()

public:
	AAudioCaster();

protected:
	virtual void BeginPlay() override;
	void AC_PlayEvent(UAkAudioEvent* AkEvent, class FOnAkPostEventCallback PostEventCallback);
	void AC_SetRtpc(UAkRtpc* AkRtpc, float Value, int32 InterpolationTimeMs);
	void AC_SetSwitch(UAkSwitchValue* AkSwitchValue);
	void AC_PlayEventArray(
		UAkAudioEvent* AkEvent,
		UAkRtpc* AkRtpc,
		float Value,
		int32 InterpolationTimeMs,
		UAkSwitchValue* AkSwitchValue,
		class FOnAkPostEventCallback PostEventCallback);
public:
	virtual void Tick(float DeltaTime) override;

	UPROPERTY(EditAnywhere, Category = "AudioCaster")
	TArray<FAudioCasterStruct> AudioCasterArray;

	//Only change the debug sphere radius, will not change attenuation
	UPROPERTY(EditAnywhere, Category = "AudioCaster")
	float RadiosScalingFactor = 1.f;
	//Only execute the first element in the array
	UFUNCTION(BlueprintCallable, CallInEditor, Category = "AudioCaster")
	void PlayButton();
	//Stop all sound on this actor
	UFUNCTION(BlueprintCallable, CallInEditor, Category = "AudioCaster")
	void StopButton();
	//Execute the elements in the array, following the ExecuteDelayTime
	UFUNCTION(BlueprintCallable, CallInEditor, Category = "AudioCaster")
	void PlayArrayButton();
	//Only show the radius of the first element
	UFUNCTION(BlueprintCallable, CallInEditor, Category = "AudioCaster")
	void ShowRadius();
	//Enable monitoring emitter using multiple listeners and routing
	UFUNCTION(BlueprintCallable, CallInEditor, Category = "AudioCaster")
	void EnableMonitoring();

	//Declare a delegate callback function
	UFUNCTION()
	void OnEventEndFunction();

	UFUNCTION(BlueprintCallable, Category = "AudioCaster")
	void SetRTPCOnChanged();

	UFUNCTION(BlueprintCallable, Category = "AudioCaster")
	void SetSwitchValueOnChanged();

private:
	UPROPERTY(VisibleAnywhere)
	UAkComponent* AkComponent;
	FAkAudioDevice* AkAudioDevice;

	UPROPERTY(EditAnywhere, Category = "Spy-Microphone")
	AAkAmbientSound* Microphone;
	UPROPERTY(EditAnywhere, Category = "Spy-Loudspeaker")
	AAkAmbientSound* Loudspeaker;

	UAkComponent* MicrophoneAkComponent;
	UAkComponent* LoudspeakerAkComponent;
};