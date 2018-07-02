#pragma once

#include "Blueprint/UserWidget.h"
#include "DragDropOperation.h"
#include "WgtDraggable.generated.h"


UCLASS(Config = Game)
class GOODGAME_API UWgtDraggable : public UUserWidget
{
	GENERATED_BODY()
	
public:
	/*UPROPERTY(Config, EditAnywhere, BlueprintReadWrite, Category = Draggable)
	TEnumAsByte<EDragPivot> Pivot;

	UPROPERTY(Config, EditAnywhere, BlueprintReadWrite, Category = Draggable)
	FName DraggableUID;*/

	UFUNCTION(BlueprintCallable, Category = Draggable)
	void SetAndSaveAnchorsInViewport(const FVector2D& anchors);

	UFUNCTION(BlueprintCallable, Category = Draggable)
	void SetAndSaveAlignmentInViewport(const FVector2D& alignment);

	UFUNCTION(BlueprintCallable, Category = Draggable)
	void SetDefaultAnchorsInViewport(const FVector2D& anchors);

	UFUNCTION(BlueprintCallable, Category = Draggable)
	void SetDefaultAlignmentInViewport(const FVector2D& alignment);

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Draggable)
	uint32 bLoadedAnchors:1;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Draggable)
	uint32 bLoadedAlignment:1;
	
	virtual void NativeConstruct() override;
	virtual FReply NativeOnMouseButtonDown(const FGeometry& InGeometry, const FPointerEvent& InMouseEvent) override;
	virtual void NativeOnDragDetected(const FGeometry& InGeometry, const FPointerEvent& InMouseEvent, UDragDropOperation*& InOperation) override;
	virtual void NativeOnDragCancelled(const FDragDropEvent& InDragDropEvent, UDragDropOperation* InOperation) override;
};
