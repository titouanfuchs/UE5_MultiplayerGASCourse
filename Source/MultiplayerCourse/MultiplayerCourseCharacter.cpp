// Copyright Epic Games, Inc. All Rights Reserved.

#include "MultiplayerCourseCharacter.h"
#include "Camera/CameraComponent.h"
#include "Components/CapsuleComponent.h"
#include "Components/InputComponent.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "GameFramework/Controller.h"
#include "GameFramework/SpringArmComponent.h"
#include "EnhancedInputComponent.h"
#include "EnhancedInputSubsystems.h"
#include "AbilitySystemComponent.h"
#include "AbilitySystemBlueprintLibrary.h"
#include "AbilitySys/Attributes/AG_AttributeSetBase.h"
#include "AbilitySys/Components/AG_AbilitySystemComponentBase.h"
#include "Actors/Components/AG_CharacterMovementComponent.h"
#include "DataAssets/CharacterDataAsset.h"
#include "Net/UnrealNetwork.h"
#include "Actors/Components/AG_CharacterMovementComponent.h"


//////////////////////////////////////////////////////////////////////////
// AMultiplayerCourseCharacter

AMultiplayerCourseCharacter::AMultiplayerCourseCharacter(const FObjectInitializer& ObjectInitializer) :
	Super(ObjectInitializer.SetDefaultSubobjectClass<UAG_CharacterMovementComponent>(CharacterMovementComponentName))
{
		// Set size for collision capsule
	GetCapsuleComponent()->InitCapsuleSize(42.f, 96.0f);
		
	// Don't rotate when the controller rotates. Let that just affect the camera.
	bUseControllerRotationPitch = false;
	bUseControllerRotationYaw = false;
	bUseControllerRotationRoll = false;

	// Configure character movement
	GetCharacterMovement()->bOrientRotationToMovement = true; // Character moves in the direction of input...	
	GetCharacterMovement()->RotationRate = FRotator(0.0f, 500.0f, 0.0f); // ...at this rotation rate

	// Note: For faster iteration times these variables, and many more, can be tweaked in the Character Blueprint
	// instead of recompiling to adjust them
	GetCharacterMovement()->JumpZVelocity = 700.f;
	GetCharacterMovement()->AirControl = 0.35f;
	GetCharacterMovement()->MaxWalkSpeed = 500.f;
	GetCharacterMovement()->MinAnalogWalkSpeed = 20.f;
	GetCharacterMovement()->BrakingDecelerationWalking = 2000.f;

	// Create a camera boom (pulls in towards the player if there is a collision)
	CameraBoom = CreateDefaultSubobject<USpringArmComponent>(TEXT("CameraBoom"));
	CameraBoom->SetupAttachment(RootComponent);
	CameraBoom->TargetArmLength = 400.0f; // The camera follows at this distance behind the character	
	CameraBoom->bUsePawnControlRotation = true; // Rotate the arm based on the controller

	// Create a follow camera
	FollowCamera = CreateDefaultSubobject<UCameraComponent>(TEXT("FollowCamera"));
	FollowCamera->SetupAttachment(CameraBoom, USpringArmComponent::SocketName); // Attach the camera to the end of the boom and let the boom adjust to match the controller orientation
	FollowCamera->bUsePawnControlRotation = false; // Camera does not rotate relative to arm

	//Ability SYstem

	AbilitySystemComponent = CreateDefaultSubobject<UAG_AbilitySystemComponentBase>(TEXT("AbilitySystemComponent"));
	AbilitySystemComponent->SetIsReplicated(true);
	AbilitySystemComponent->SetReplicationMode(EGameplayEffectReplicationMode::Mixed);

	AttributeSet = CreateDefaultSubobject<UAG_AttributeSetBase>(TEXT("AttributeSet"));
	
	// Note: The skeletal mesh and anim blueprint references on the Mesh component (inherited from Character) 
	// are set in the derived blueprint asset named ThirdPersonCharacter (to avoid direct content references in C++)
}


void AMultiplayerCourseCharacter::PostInitializeComponents()
{
	Super::PostInitializeComponents();

	if (!IsValid(CharacterDataAsset)) {return;}

	SetCharacterData(CharacterDataAsset->CharacterData);
}

bool AMultiplayerCourseCharacter::ApplyGameplayEffectToSelf(TSubclassOf<UGameplayEffect> Effect,
                                                            FGameplayEffectContextHandle InEffectContext)
{
	if (!Effect.Get()) {return false;}

	FGameplayEffectSpecHandle SpecHandle = AbilitySystemComponent->MakeOutgoingSpec(Effect, 1, InEffectContext);
	if (!SpecHandle.IsValid()) {return false;}

	FActiveGameplayEffectHandle ActiveGEHandle = AbilitySystemComponent->ApplyGameplayEffectSpecToSelf(*SpecHandle.Data.Get());

	return ActiveGEHandle.IsValid();
	
}

UAbilitySystemComponent* AMultiplayerCourseCharacter::GetAbilitySystemComponent() const
{
	return AbilitySystemComponent;
}

FCharacterData AMultiplayerCourseCharacter::GetCharacterData() const
{
	return CharacterData;
}

void AMultiplayerCourseCharacter::SetCharacterData(const FCharacterData& InCharacterData)
{
	CharacterData = InCharacterData;

	InitFromCharacterData(CharacterData);
}

void AMultiplayerCourseCharacter::BeginPlay()
{
	// Call the base class  
	Super::BeginPlay();

	//Add Input Mapping Context
	if (APlayerController* PlayerController = Cast<APlayerController>(Controller))
	{
		if (UEnhancedInputLocalPlayerSubsystem* Subsystem = ULocalPlayer::GetSubsystem<UEnhancedInputLocalPlayerSubsystem>(PlayerController->GetLocalPlayer()))
		{
			Subsystem->AddMappingContext(DefaultMappingContext, 0);
		}
	}
}

//////////////////////////////////////////////////////////////////////////
// Input

void AMultiplayerCourseCharacter::SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent)
{
	// Set up action bindings
	if (UEnhancedInputComponent* EnhancedInputComponent = CastChecked<UEnhancedInputComponent>(PlayerInputComponent)) {
		
		//Jumping
		EnhancedInputComponent->BindAction(JumpAction, ETriggerEvent::Triggered, this, &AMultiplayerCourseCharacter::OnJumpStarted);
		EnhancedInputComponent->BindAction(JumpAction, ETriggerEvent::Completed, this, &AMultiplayerCourseCharacter::OnJumpEnded);

		//Moving
		EnhancedInputComponent->BindAction(MoveAction, ETriggerEvent::Triggered, this, &AMultiplayerCourseCharacter::Move);

		//Looking
		EnhancedInputComponent->BindAction(LookAction, ETriggerEvent::Triggered, this, &AMultiplayerCourseCharacter::Look);
	}
}

void AMultiplayerCourseCharacter::OnRep_CharacterData()
{
	InitFromCharacterData(CharacterData, true);
}

void AMultiplayerCourseCharacter::InitFromCharacterData(const FCharacterData& InCharacterData, bool bFromReplication)
{
	
}

void AMultiplayerCourseCharacter::Landed(const FHitResult& Hit)
{
	Super::Landed(Hit);
	if (!AbilitySystemComponent){return;}

	AbilitySystemComponent->RemoveActiveEffectsWithTags(InAirTag);
}

void AMultiplayerCourseCharacter::Move(const FInputActionValue& Value)
{
	// input is a Vector2D
	FVector2D MovementVector = Value.Get<FVector2D>();

	if (Controller != nullptr)
	{
		// find out which way is forward
		const FRotator Rotation = Controller->GetControlRotation();
		const FRotator YawRotation(0, Rotation.Yaw, 0);

		// get forward vector
		const FVector ForwardDirection = FRotationMatrix(YawRotation).GetUnitAxis(EAxis::X);
	
		// get right vector 
		const FVector RightDirection = FRotationMatrix(YawRotation).GetUnitAxis(EAxis::Y);

		// add movement 
		AddMovementInput(ForwardDirection, MovementVector.Y);
		AddMovementInput(RightDirection, MovementVector.X);
	}
}

void AMultiplayerCourseCharacter::Look(const FInputActionValue& Value)
{
	// input is a Vector2D
	FVector2D LookAxisVector = Value.Get<FVector2D>();

	if (Controller != nullptr)
	{
		// add yaw and pitch input to controller
		AddControllerYawInput(LookAxisVector.X);
		AddControllerPitchInput(LookAxisVector.Y);
	}
}



void AMultiplayerCourseCharacter::OnJumpStarted()
{
	FGameplayEventData Payload;
	Payload.Instigator = this;
	Payload.EventTag = JumpEventTag;

	UAbilitySystemBlueprintLibrary::SendGameplayEventToActor(this, JumpEventTag, Payload);
}

void AMultiplayerCourseCharacter::OnJumpEnded()
{
	StopJumping();
}

void AMultiplayerCourseCharacter::GiveAbilities()
{
	if (!(HasAuthority() && AbilitySystemComponent)) {return;}

	for (auto DefaultAbility:CharacterData.Abilities)
	{
		AbilitySystemComponent->GiveAbility(FGameplayAbilitySpec(DefaultAbility));
	}
}

void AMultiplayerCourseCharacter::ApplyStartupEffects()
{
	if (!(GetLocalRole() == ROLE_Authority))
	{
		return;
	}

	FGameplayEffectContextHandle EffectContext = AbilitySystemComponent->MakeEffectContext();
	EffectContext.AddSourceObject(this);

	for (auto CharacterEffect:CharacterData.Effects)
	{
		ApplyGameplayEffectToSelf(CharacterEffect, EffectContext);
	}
}

void AMultiplayerCourseCharacter::PossessedBy(AController* NewController)
{
	Super::PossessedBy(NewController);

	AbilitySystemComponent->InitAbilityActorInfo(this, this);
	GiveAbilities();
	ApplyStartupEffects();
}

void AMultiplayerCourseCharacter::OnRep_PlayerState()
{
	Super::OnRep_PlayerState();

	AbilitySystemComponent->InitAbilityActorInfo(this,this);
}

void AMultiplayerCourseCharacter::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME(AMultiplayerCourseCharacter, CharacterData);
}


