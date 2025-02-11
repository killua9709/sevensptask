#include "MyPawn.h"
#include "SpartaPlayerController.h"
#include "Components/CapsuleComponent.h"
#include "Components/SkeletalMeshComponent.h"
#include "Camera/CameraComponent.h"
#include "EnhancedInputComponent.h"
#include "GameFramework/SpringArmComponent.h"
#include "GameFramework//CharacterMovementComponent.h"

AMyPawn::AMyPawn()
{
	PrimaryActorTick.bCanEverTick = true;
	CapsuleComponent = CreateDefaultSubobject<UCapsuleComponent>(TEXT("CapsuleComponent"));
	SetRootComponent(CapsuleComponent);
	CapsuleComponent->SetSimulatePhysics(false);

	SkeletalMeshComponent = CreateDefaultSubobject<USkeletalMeshComponent>(TEXT("SkeletalMesh"));
	SkeletalMeshComponent->SetupAttachment(CapsuleComponent);
	SkeletalMeshComponent->SetSimulatePhysics(false);

	SpringArmComponent = CreateDefaultSubobject<USpringArmComponent>(TEXT("SpringArm"));
	SpringArmComponent->SetupAttachment(CapsuleComponent);
	SpringArmComponent->TargetArmLength = 300.0f;
	SpringArmComponent->bUsePawnControlRotation = false;

	CameraComponent = CreateDefaultSubobject<UCameraComponent>(TEXT("CameraComponet"));
	CameraComponent->SetupAttachment(SpringArmComponent, USpringArmComponent::SocketName); 
	//springarm 부분중 socketname이라는 곳에 붙겠다

	// 입력 값 초기화
	MovementInput = FVector2D::ZeroVector;
	LookInput = FVector2D::ZeroVector;
	Velocity = FVector::ZeroVector;
}

void AMyPawn::BeginPlay()
{
	Super::BeginPlay();
	
}

void AMyPawn::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
	// 1) 카메라의 현재 회전 값에서 Yaw만 추출
	const FRotator CameraRotation = CameraComponent->GetComponentRotation();
	const FRotator YawOnlyRotation(0.f, CameraRotation.Yaw, 0.f);

	// 2) 카메라 기준으로 Forward, Right 벡터 계산
	// Unreal 기준: X = Forward, Y = Right, Z = Up
	const FVector CameraForward = FRotationMatrix(YawOnlyRotation).GetUnitAxis(EAxis::X);
	const FVector CameraRight = FRotationMatrix(YawOnlyRotation).GetUnitAxis(EAxis::Y);

	// 3) 입력값에 따른 이동 방향 결정
	//    MovementInput.X: 전후 (W/S), MovementInput.Y: 좌우 (A/D)
	const FVector InputDirection = (CameraForward * MovementInput.X) + (CameraRight * MovementInput.Y);

	// 4) 목표 이동 속도 계산 (정규화하여 최대 속도 적용)
	const FVector DesiredVelocity = InputDirection.GetSafeNormal() * MaxWalkSpeed;

	// 5) 현재 속도에 보간(interpolation) 적용하여 부드러운 가감속 구현
	Velocity = FMath::VInterpTo(Velocity, DesiredVelocity, DeltaTime, VelocityInterpSpeed);

	// 6) 월드 좌표계에서 이동 처리 (충돌 체크 포함)
	AddActorWorldOffset(Velocity * DeltaTime, /*bSweep=*/true);

	// 7) Pawn 자체 회전 처리 (마우스 좌우 입력 기반)
	if (!FMath::IsNearlyZero(LookInput.X))
	{
		const float YawRotation = LookInput.X * TurnRate * DeltaTime;
		AddActorLocalRotation(FRotator(0.f, YawRotation, 0.f));
	}

	// 8) 스프링암 회전 처리 (마우스 상하 입력 기반: 카메라 Pitch 제어)
	if (!FMath::IsNearlyZero(LookInput.Y))
	{
		// 현재 스프링암의 로컬 회전값을 가져온다.
		FRotator CurrentArmRotation = SpringArmComponent->GetRelativeRotation();
		// 마우스 입력에 따라 Pitch 조정 (LookUpRate 적용)
		float NewPitch = CurrentArmRotation.Pitch - (LookInput.Y * LookUpRate * DeltaTime);
		// Pitch 값을 최소/최대 값으로 제한
		NewPitch = FMath::Clamp(NewPitch, MinPitch, MaxPitch);
		CurrentArmRotation.Pitch = NewPitch;
		SpringArmComponent->SetRelativeRotation(CurrentArmRotation);
	}
}

void AMyPawn::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);
	
	if (UEnhancedInputComponent* EnhancedInput = Cast< UEnhancedInputComponent>(PlayerInputComponent))
	{
		if (ASpartaPlayerController* PlayerController = Cast< ASpartaPlayerController>(GetController()))
		{
			if (PlayerController->MoveAction)
			{
				EnhancedInput->BindAction(
					PlayerController->MoveAction,
					ETriggerEvent::Triggered,
					this,
					&AMyPawn::Move
				);

				EnhancedInput->BindAction(
					PlayerController->MoveAction,
					ETriggerEvent::Completed,
					this,
					&AMyPawn::StopMove
				);
			}

			if (PlayerController->LookAction)
			{
				EnhancedInput->BindAction(
					PlayerController->LookAction,
					ETriggerEvent::Triggered,
					this,
					&AMyPawn::Look
				);

				EnhancedInput->BindAction(
					PlayerController->LookAction,
					ETriggerEvent::Completed,
					this,
					&AMyPawn::StopLook
				);
			}
		}
	}

}

void AMyPawn::Move(const FInputActionValue& value)
{
	MovementInput = value.Get<FVector2D>();
}

void AMyPawn::Look(const FInputActionValue& value)
{
	LookInput = value.Get<FVector2D>();
}

void AMyPawn::StopMove(const FInputActionValue& Value)
{
	MovementInput = FVector2D::ZeroVector;
}

void AMyPawn::StopLook(const FInputActionValue& Value)
{
	LookInput = FVector2D::ZeroVector;
}

