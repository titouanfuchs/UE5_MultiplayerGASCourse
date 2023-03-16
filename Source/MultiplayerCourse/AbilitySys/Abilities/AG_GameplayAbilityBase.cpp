// Fill out your copyright notice in the Description page of Project Settings.


#include "AbilitySys/Abilities/AG_GameplayAbilityBase.h"

#include "AbilitySystemComponent.h"
#include "MultiplayerCourseCharacter.h"

void UAG_GameplayAbilityBase::ActivateAbility(const FGameplayAbilitySpecHandle Handle,
                                              const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo,
                                              const FGameplayEventData* TriggerEventData)
{
	Super::ActivateAbility(Handle, ActorInfo, ActivationInfo, TriggerEventData);

	FGameplayEffectContextHandle EffectContext = ActorInfo->AbilitySystemComponent->MakeEffectContext();

	for(auto GameplayEffect : OnGoingEffectsToApply)
	{
		if (!GameplayEffect.Get()) continue;
		
		FGameplayEffectSpecHandle SpecHandle = ActorInfo->AbilitySystemComponent->MakeOutgoingSpec(GameplayEffect, 1, EffectContext);

		if (!SpecHandle.IsValid()) continue;

		FActiveGameplayEffectHandle ActiveGEHandle = ActorInfo->AbilitySystemComponent->ApplyGameplayEffectSpecToSelf(*SpecHandle.Data.Get());
		if (!ActiveGEHandle.WasSuccessfullyApplied())
		{
			UE_LOG(LogTemp,Warning, TEXT("Failed to apply %s to %s"), *GetNameSafe(GameplayEffect), *GetNameSafe(ActorInfo->AvatarActor.Get()));
			continue;
		}

		RemoveOnEndEffectHandles.Add(ActiveGEHandle);
	}
}

void UAG_GameplayAbilityBase::EndAbility(const FGameplayAbilitySpecHandle Handle,
	const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo,
	bool bReplicateEndAbility, bool bWasCancelled)
{
	if(!IsInstantiated()) return;

	for(auto ActiveEffectHandle : RemoveOnEndEffectHandles)
	{
		if (!ActiveEffectHandle.IsValid()) continue;
		
		ActorInfo->AbilitySystemComponent->RemoveActiveGameplayEffect(ActiveEffectHandle);
	}

	RemoveOnEndEffectHandles.Empty();
	
	Super::EndAbility(Handle, ActorInfo, ActivationInfo, bReplicateEndAbility, bWasCancelled);
}

AMultiplayerCourseCharacter* UAG_GameplayAbilityBase::GetCharacterFromActorInfo() const
{
	return CastChecked<AMultiplayerCourseCharacter>(GetAvatarActorFromActorInfo());
}
