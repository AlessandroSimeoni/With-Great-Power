

#include "Patrol/PatrolSplineComponent.h"
#include "Patrol/PatrolPath.h"


void UPatrolSplineComponent::BeginPlay()
{
	Super::BeginPlay();
	SetUpCommand();
	
}

void UPatrolSplineComponent::SetUpCommand()
{
	if (!IsValid(CommandReference))
	{
		return;
	}
	
	UObject* CommandInstance = NewObject<UObject>(GetWorld(), CommandReference);
	if (!CommandInstance->Implements<USplineBehaviorCommand>()) return;
	
	Command = CommandInstance;
	SetCurrentSplineActor();
}

FVector UPatrolSplineComponent::GetNextPosition()
{
	if (!IsPathAssigned())
		return FVector::ZeroVector;

	Command.GetInterface()->IncrementIndex(AssignedPatrolPath->Spline,CurrentIndex);
	return Command.GetInterface()->Execute(AssignedPatrolPath->Spline,CurrentIndex);
}

FVector UPatrolSplineComponent::GetPosition()
{
	if (!IsPathAssigned())
		return FVector::ZeroVector;
	
	if (CurrentIndex >= 0 && CurrentIndex < AssignedPatrolPath->Spline->GetNumberOfSplinePoints())
	{
		//return Command.GetInterface()->Execute(AssignedPatrolPath->Spline,CurrentIndex);
		FVector LocalPosition = Command.GetInterface()->Execute(AssignedPatrolPath->Spline, CurrentIndex);
		
		AActor* OwnerActor = AssignedPatrolPath->Spline->GetOwner();
		if (OwnerActor)
		{
			FVector WorldPosition = OwnerActor->GetActorTransform().TransformPosition(LocalPosition);
			return WorldPosition;
		}
		else
		{
			//UE_LOG(LogTemp, Warning, TEXT("UPatrolSplineComponent::GetPosition - Owner not found for Spline."));
			return FVector::ZeroVector;
		}
		
	}
	else
	{
		//UE_LOG(LogTemp,Warning,TEXT("UPatrolSplineComponent::GetPosition Outside of bounds"));
		return FVector::ZeroVector;
	}
	
}

void UPatrolSplineComponent::IncrementIndex()
{
	if (!IsPathAssigned()) return; 
	Command.GetInterface()->IncrementIndex(AssignedPatrolPath->Spline,CurrentIndex);
}

FVector UPatrolSplineComponent::GetCurrentPosition()
{
	if (!IsPathAssigned())
		return FVector::ZeroVector;

	return  AssignedPatrolPath->Spline->GetSplinePointAt(CurrentIndex,ESplineCoordinateSpace::World).Position;
}

FVector UPatrolSplineComponent::IncrementAndGetCurrentPosition()
{
	if (!IsPathAssigned())
		return FVector::ZeroVector;

	int32 PreviousIndex = CurrentIndex;
	IncrementIndex();
	OnNewPosition.Broadcast(PreviousIndex,CurrentIndex);
	return GetCurrentPosition();
	
}

bool UPatrolSplineComponent::IncrementIndexAndSendScriptedActionEvent()
{
	if (!IsPathAssigned())
	{
		return false; 
	}
	UE_LOG(LogTemp, Display, TEXT("PatrolSplineComponent::IncrementIndex"));
	int PreviousIndex = CurrentIndex;
	Command.GetInterface()->IncrementIndex(AssignedPatrolPath->Spline,CurrentIndex);
	
		
		
		return false;
	}


bool UPatrolSplineComponent::IsPathAssigned()
{
	return IsValid(AssignedPatrolPath);
		
}

void UPatrolSplineComponent::SetCurrentSplineActor(APatrolPath* PatrolPath)
{
	if (IsValid(PatrolPath))return;

	AssignedPatrolPath = PatrolPath;
	SetCurrentSplineActor();	
}

void UPatrolSplineComponent::SetCurrentSplineActor()
{
	if (!IsPathAssigned())
		return;

	CurrentIndex = AssignedPatrolPath->Spline->FindInputKeyClosestToWorldLocation(GetOwner()->GetActorLocation());
	
}
