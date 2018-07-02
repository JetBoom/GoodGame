#include "GoodGame.h"
#include "WidgetBlueprintLibrary.h"
#include "SlateBlueprintLibrary.h"
//#include "DragDropOperation.h"
#include "CanvasPanelSlot.h"
#include "PanelWidget.h"
#include "WgtDraggable.h"

void UWgtDraggable::NativeConstruct()
{
	Super::NativeConstruct();
	
	if (GConfig) {
		auto path = GetClass()->GetPathName();

		FVector2D vec;

		if (GConfig->GetVector2D(*path, TEXT("Anchors"), vec, GGameIni) && vec.X >= 0.f && vec.Y >= 0.f && vec.X <= 1.f && vec.Y <= 1.f) {
			SetAnchorsInViewport(FAnchors(vec.X, vec.Y));
			bLoadedAnchors = true;
		}

		if (GConfig->GetVector2D(*path, TEXT("Alignment"), vec, GGameIni) && vec.X >= 0.f && vec.Y >= 0.f && vec.X <= 1.f && vec.Y <= 1.f) {
			SetAlignmentInViewport(vec);
			bLoadedAlignment = true;
		}
	}
}

void UWgtDraggable::SetAndSaveAnchorsInViewport(const FVector2D& anchors)
{
	SetAnchorsInViewport(FAnchors(anchors.X, anchors.Y));
	
	if (GConfig) {
		auto path = GetClass()->GetPathName();
		GConfig->SetVector2D(*path, TEXT("Anchors"), anchors, GGameIni);
		GConfig->Flush(false, GGameIni);
	}
}

void UWgtDraggable::SetAndSaveAlignmentInViewport(const FVector2D& alignment)
{
	SetAlignmentInViewport(alignment);
	
	if (GConfig) {
		auto path = GetClass()->GetPathName();
		GConfig->SetVector2D(*path, TEXT("Alignment"), alignment, GGameIni);
		GConfig->Flush(false, GGameIni);
	}
}

void UWgtDraggable::SetDefaultAnchorsInViewport(const FVector2D& anchors)
{
	if (!bLoadedAnchors)
		SetAnchorsInViewport(FAnchors(anchors.X, anchors.Y));
}

void UWgtDraggable::SetDefaultAlignmentInViewport(const FVector2D& alignment)
{
	if (!bLoadedAlignment)
		SetAlignmentInViewport(alignment);
}

FReply UWgtDraggable::NativeOnMouseButtonDown(const FGeometry& InGeometry, const FPointerEvent& InMouseEvent)
{
	return FReply::Handled().DetectDrag(TakeWidget(), EKeys::LeftMouseButton);
}

void UWgtDraggable::NativeOnDragDetected(const FGeometry& InGeometry, const FPointerEvent& InMouseEvent, UDragDropOperation*& InOperation)
{
	InOperation = UWidgetBlueprintLibrary::CreateDragDropOperation(UDragDropOperation::StaticClass());
	if (InOperation) {
		ColorAndOpacity.A /= 2.f;
		SetColorAndOpacity(ColorAndOpacity);

		InOperation->DefaultDragVisual = this;

		// ahhhhhhhh
		FVector2D alignment = GetFullScreenAlignment();
		if (alignment.X == 0.f && alignment.Y == 0.f)
			InOperation->Pivot = EDragPivot::TopLeft;
		else if (alignment.X == 0.f && alignment.Y == 0.5f)
			InOperation->Pivot = EDragPivot::CenterLeft;
		else if (alignment.X == 0.f && alignment.Y == 1.f)
			InOperation->Pivot = EDragPivot::BottomLeft;
		else if (alignment.X == 0.5f && alignment.Y == 0.f)
			InOperation->Pivot = EDragPivot::TopCenter;
		else if (alignment.X == 0.5f && alignment.Y == 0.5f)
			InOperation->Pivot = EDragPivot::CenterCenter;
		else if (alignment.X == 0.5f && alignment.Y == 1.f)
			InOperation->Pivot = EDragPivot::BottomCenter;
		else if (alignment.X == 1.0f && alignment.Y == 0.f)
			InOperation->Pivot = EDragPivot::TopRight;
		else if (alignment.X == 1.0f && alignment.Y == 0.5f)
			InOperation->Pivot = EDragPivot::CenterRight;
		else if (alignment.X == 1.0f && alignment.Y == 1.f)
			InOperation->Pivot = EDragPivot::BottomRight;
	}
}

void UWgtDraggable::NativeOnDragCancelled(const FDragDropEvent& InDragDropEvent, UDragDropOperation* InOperation)
{
	ColorAndOpacity.A *= 2.f;
	SetColorAndOpacity(ColorAndOpacity);

	FVector2D pixelPosition;
	FVector2D viewportPosition;
	FVector2D viewportSize;

	USlateBlueprintLibrary::AbsoluteToViewport(GetWorld(), InDragDropEvent.GetScreenSpacePosition(), pixelPosition, viewportPosition);
	GetOwningLocalPlayer()->ViewportClient->GetViewportSize(viewportSize);

	pixelPosition.X = FMath::Clamp(pixelPosition.X / viewportSize.X, 0.f, 1.f);
	// Leave some room for the static top/bottom bars.
	pixelPosition.Y = FMath::Clamp(pixelPosition.Y / viewportSize.Y, 0.02f, 0.98f);

	SetAndSaveAnchorsInViewport(pixelPosition);
}
