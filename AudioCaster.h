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

UCLASS()
class AUDIOTEST_API AAudioCaster : public AActor
{
	GENERATED_BODY()
	
public:	
	AAudioCaster();

protected:
	virtual void BeginPlay() override;

public:	
	virtual void Tick(float DeltaTime) override;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "AudioCaster")
	UAkAudioEvent* AkEvent;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "AudioCaster")
	UAkRtpc* AkRtpc;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "AudioCaster")
	float RtpcValue;

	UPROPERTY(EditAnywhere, Category = "AudioCaster")
	UAkSwitchValue* AkSwitchValue;

	//Only works during runtime
	UPROPERTY(EditAnywhere, Category = "AudioCaster")
	bool bIfLoop = false;

	//Only change the debug sphere radius, will not change attenuation
	UPROPERTY(EditAnywhere, Category = "AudioCaster")
	float RadiosScalingFactor = 1.f;

	//Declare delegate
	class FOnAkPostEventCallback PostEventCallback;

	//Declare a delegate callback function
	UFUNCTION()
	void OnEventEndFunction();

	UFUNCTION(BlueprintCallable, Category = "AudioCaster")
	void OnPlaySoundButtonClicked();

	UFUNCTION(BlueprintCallable, Category = "AudioCaster")
	void OnSetRTPCButtonClicked();

	UFUNCTION(BlueprintCallable, Category = "AudioCaster")
	void OnSetSwitchButtonClicked();

	UFUNCTION(BlueprintCallable, Category = "AudioCaster")
	void OnShowRadiusClicked();

	UFUNCTION(BlueprintCallable, Category = "AudioCaster")
	void SetRTPCOnChanged();

	UFUNCTION(BlueprintCallable, Category = "AudioCaster")
	void SetSwitchValueOnChanged();

private:
	//Temporarily store Rtpc value
	UPROPERTY()
	float _tempRtpcValue;

	//Temporarily store Switch name
	UPROPERTY()
	FName _tempSwitchName;

	UPROPERTY(VisibleAnywhere)
	UAkComponent* AkComponent;

};
