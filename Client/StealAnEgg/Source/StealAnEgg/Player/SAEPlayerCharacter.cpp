#include "SAEPlayerCharacter.h"

#include "AbilitySystemComponent.h"
#include "Components/CapsuleComponent.h"
#include "Camera/CameraComponent.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "GameFramework/InputSettings.h"
#include "GameFramework/SpringArmComponent.h"
#include "SAEPlayerAttributeComponent.h"
#include "GameFramework/SAEPlayerState.h"
#include "AbilitySystem/SAEPlayerAttributeSet.h"
#include "EnhancedInputComponent.h"
#include "InputActionValue.h"
#include "SAEHeldItemPresentationComponent.h"
#include "AbilitySystem/SAEAbilitySystemComponent.h"
#include "AbilitySystem/SAEGameplayTags.h"
#include "Transform/TransformConstraintUtil.h"


ASAEPlayerCharacter::ASAEPlayerCharacter()
{
	GetCapsuleComponent()->SetCapsuleSize(42.f, 96.f);

	GetCharacterMovement()->bOrientRotationToMovement = true;
	
	CameraBoom = CreateDefaultSubobject<USpringArmComponent>(TEXT("CameraBoom"));
	CameraBoom->SetupAttachment(RootComponent);
	CameraBoom->TargetArmLength = 300.f;
	CameraBoom->bUsePawnControlRotation = true;

	FollowCamera = CreateDefaultSubobject<UCameraComponent>(TEXT("FollowCamera"));
	FollowCamera->SetupAttachment(CameraBoom,USpringArmComponent::SocketName);
	FollowCamera->bUsePawnControlRotation = false;

	PlayerAttribute = CreateDefaultSubobject<USAEPlayerAttributeComponent>(TEXT("PlayerAttribute"));
	
	HeldItemPresentationComponent = CreateDefaultSubobject<USAEHeldItemPresentationComponent>(TEXT("HeldItemPresentationComponent"));

	FrontCarryAnchor = CreateDefaultSubobject<USceneComponent>(TEXT("FrontCarryAnchor"));
	FrontCarryAnchor->SetupAttachment(GetRootComponent());
	FrontCarryAnchor->SetRelativeLocation(FVector(70.0f, 0.0f, 45.0f));
}


void ASAEPlayerCharacter::BeginPlay()
{
	Super::BeginPlay();
}

void ASAEPlayerCharacter::EndPlay(const EEndPlayReason::Type EndPlayReason)
{
	UninitializeAbilitySystem();
	
	Super::EndPlay(EndPlayReason);
}

void ASAEPlayerCharacter::PossessedBy(AController* NewController)
{
	Super::PossessedBy(NewController);
	//服务端初始化
	InitializeAbilitySystem();
	//
	if(HeldItemPresentationComponent)
	{
		HeldItemPresentationComponent->RefreshPresentation();
	}
}

void ASAEPlayerCharacter::OnRep_PlayerState()
{	
	Super::OnRep_PlayerState();
	//客户端初始化
	InitializeAbilitySystem();
	//
	if(HeldItemPresentationComponent)
	{
		HeldItemPresentationComponent->RefreshPresentation();
	}
}

UAbilitySystemComponent* ASAEPlayerCharacter::GetAbilitySystemComponent() const
{
	const ASAEPlayerState* PS = GetPlayerState<ASAEPlayerState>();
	return PS ? PS->GetAbilitySystemComponent() : nullptr;
}

//初始化AbilitySystem组件，在服务端PossessedBy时和客户端PlayerState复制完毕时调用
void ASAEPlayerCharacter::InitializeAbilitySystem()
{
	ASAEPlayerState* PS = GetPlayerState<ASAEPlayerState>();
	if(!PS)
	{
		return;
	}

	PS->InitAbilitySystemAvatar(this);

	//先让前后端都监听属性变化，保证流程正确。
	BindMovementAttributes(PS->GetAbilitySystemComponent());
}

void ASAEPlayerCharacter::UninitializeAbilitySystem()
{
	UnbindMovementAttributes();
	if(ASAEPlayerState* PS = GetPlayerState<ASAEPlayerState>())
	{
		if(UAbilitySystemComponent* ASC = PS->GetAbilitySystemComponent())
		{
			if(ASC->GetAvatarActor() == this)
			{
				if(ASC->GetOwnerActor())
				{
					ASC->SetAvatarActor(nullptr);
				}
				else
				{
					ASC->ClearActorInfo();
				}
			}
		}
	}
}

void ASAEPlayerCharacter::BindMovementAttributes(UAbilitySystemComponent* ASC)
{
	UnbindMovementAttributes();
	if(!ASC)
	{
		return;
	}
	BoundASC = ASC;
	BaseMoveSpeedChangedHandle = ASC->GetGameplayAttributeValueChangeDelegate(
		USAEPlayerAttributeSet::GetBaseMoveSpeedAttribute()
		)
	.AddUObject(
			this,&ASAEPlayerCharacter::HandleMovementAttributeChanged
			);

	MoveSpeedBounsChangedHandle = ASC->GetGameplayAttributeValueChangeDelegate(
		USAEPlayerAttributeSet::GetMoveSpeedBonusRateAttribute()
		)
	.AddUObject(
			this,&ASAEPlayerCharacter::HandleMovementAttributeChanged
			);

	RefrehsMovementSpeed();
}

void ASAEPlayerCharacter::UnbindMovementAttributes()
{
	if(UAbilitySystemComponent* ASC = BoundASC.Get())
	{
		ASC->GetGameplayAttributeValueChangeDelegate(
			USAEPlayerAttributeSet::GetBaseMoveSpeedAttribute())
		.Remove(BaseMoveSpeedChangedHandle);
		ASC->GetGameplayAttributeValueChangeDelegate(
			USAEPlayerAttributeSet::GetMoveSpeedBonusRateAttribute())
		.Remove(MoveSpeedBounsChangedHandle);
	}
	BaseMoveSpeedChangedHandle.Reset();
	MoveSpeedBounsChangedHandle.Reset();
	BoundASC.Reset();
}

void ASAEPlayerCharacter::HandleMovementAttributeChanged(const FOnAttributeChangeData& Data)
{
	RefrehsMovementSpeed();
}

void ASAEPlayerCharacter::RefrehsMovementSpeed()
{
	UAbilitySystemComponent* ASC = BoundASC.Get();
	if(!ASC || ASC->GetAvatarActor() != this)
	{
		return;
	}
	const float BaseSpeed = ASC->GetNumericAttribute(USAEPlayerAttributeSet::GetBaseMoveSpeedAttribute());
	const float BonusRate = ASC->GetNumericAttribute(USAEPlayerAttributeSet::GetMoveSpeedBonusRateAttribute());
	const float EffectiveSpeed = FMath::Max(0.0f,BaseSpeed + ( 1+ BonusRate));
	GetCharacterMovement()->MaxWalkSpeed = EffectiveSpeed;
}

void ASAEPlayerCharacter::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);

	UEnhancedInputComponent* EnhancedInputComponent = CastChecked<UEnhancedInputComponent>(PlayerInputComponent);

	EnhancedInputComponent->BindAction(MoveAction,ETriggerEvent::Triggered,this,&ThisClass::HandleMoveInput);

	EnhancedInputComponent->BindAction(LookAction,ETriggerEvent::Triggered,this,&ThisClass::HandleLookInput);
	EnhancedInputComponent->BindAction(MouseLookAction, ETriggerEvent::Triggered, this, &ThisClass::HandleLookInput);

	EnhancedInputComponent->BindAction(JumpAction,ETriggerEvent::Started,this,&ACharacter::Jump);
	
	EnhancedInputComponent->BindAction(JumpAction,ETriggerEvent::Completed,this,&ACharacter::StopJumping);

	EnhancedInputComponent->BindAction(InteractionAction,ETriggerEvent::Started,this,&ThisClass::HandleInteractionStarted);
	EnhancedInputComponent->BindAction(InteractionAction,ETriggerEvent::Completed,this,&ThisClass::HandleInteractionReleased);
	EnhancedInputComponent->BindAction(InteractionAction,ETriggerEvent::Canceled,this,&ThisClass::HandleInteractionReleased);
}

void ASAEPlayerCharacter::HandleMoveInput(const FInputActionValue& Value)
{
	const FVector2D Movement = Value.Get<FVector2D>();

	const AController* CurrentController = GetController();
	if(!CurrentController)
	{
		return;
	}
	const FRotator ControlRotation = CurrentController->GetControlRotation();
	const FRotator YawRotation(0.0f,ControlRotation.Yaw,0.0f);
	const FVector ForwardDirection = FRotationMatrix(YawRotation).GetUnitAxis(EAxis::X);
	const FVector RightDirection = FRotationMatrix(ControlRotation).GetUnitAxis(EAxis::Y);
	AddMovementInput(ForwardDirection,Movement.Y);
	AddMovementInput(RightDirection,Movement.X);
}

void ASAEPlayerCharacter::HandleLookInput(const FInputActionValue& LookInput)
{
	const FVector2D& LookAxis = LookInput.Get<FVector2D>();
	AddControllerYawInput(LookAxis.X);
	AddControllerPitchInput(LookAxis.Y);
}

bool ASAEPlayerCharacter::ResolveHeldItemSlot(FGameplayTag SlotTag, USceneComponent*& OutComponent, FName& OutSocketName, FTransform& OutRelativeTransform) const
{
	if(SlotTag.MatchesTagExact(SAEGameplayTags::EquipmentSlot_Carry_Front))
	{
		OutComponent = FrontCarryAnchor;
		OutSocketName = NAME_None;
		OutRelativeTransform = FTransform::Identity;
		return true;
	}
	const FSAECharacterAttachmentSlot* Slot = AttachmentSlots.FindByPredicate(
		[&SlotTag] (const FSAECharacterAttachmentSlot& Candidate)
		{
			return Candidate.SlotTag.MatchesTagExact(SlotTag);
		});
	if(!Slot || !GetMesh())
	{
		return false;
	}
	OutComponent = GetMesh();
	OutSocketName = Slot->SocketName;
	OutRelativeTransform = Slot->RelativeTransform;
	return true;
}

void ASAEPlayerCharacter::HandleInteractionStarted()
{
	//输入事件转发给AbilitySystem组件
	if(USAEAbilitySystemComponent* ASC = Cast<USAEAbilitySystemComponent>(GetAbilitySystemComponent()))
	{
		ASC->AbilityInputTagPressed(SAEGameplayTags::Input_Interact);
	}
}

void ASAEPlayerCharacter::HandleInteractionReleased()
{
	//输入事件转发给AbilitySystem组件
	if(USAEAbilitySystemComponent* ASC = Cast<USAEAbilitySystemComponent>(GetAbilitySystemComponent()))
	{
		ASC->AbilityInputTagReleased(SAEGameplayTags::Input_Interact);
	}
}
