// Fill out your copyright notice in the Description page of Project Settings.


#include "AbilitySys/Abilities/AG_GameplayAbilityThrowBullet.h"

#include "MultiplayerCourseCharacter.h"
#include "GameFramework/Character.h"

bool UAG_GameplayAbilityThrowBullet::CanActivateAbility(const FGameplayAbilitySpecHandle Handle,
                                                        const FGameplayAbilityActorInfo* ActorInfo, const FGameplayTagContainer* SourceTags,
                                                        const FGameplayTagContainer* TargetTags, FGameplayTagContainer* OptionalRelevantTags) const
{
	return Super::CanActivateAbility(Handle, ActorInfo, SourceTags, TargetTags, OptionalRelevantTags);
}

void UAG_GameplayAbilityThrowBullet::ActivateAbility(const FGameplayAbilitySpecHandle Handle,
	const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo,
	const FGameplayEventData* TriggerEventData)
{
	if (!HasAuthorityOrPredictionKey(ActorInfo, &ActivationInfo))
	{
		return;
	}

	if (!CommitAbility(Handle, ActorInfo, ActivationInfo))
	{
		return;
	}

	Super::ActivateAbility(Handle, ActorInfo, ActivationInfo, TriggerEventData);
	
	AMultiplayerCourseCharacter* Character = CastChecked<AMultiplayerCourseCharacter>(ActorInfo->AvatarActor.Get(), ECastCheckedType::NullAllowed);
	
	Character->ThrowBullet();
}
