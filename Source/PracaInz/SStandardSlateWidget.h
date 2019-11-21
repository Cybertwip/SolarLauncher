// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Widgets/SCompoundWidget.h"

/**
 * 
 */
class PRACAINZ_API SStandardSlateWidget : public SCompoundWidget
{
public:
	SLATE_BEGIN_ARGS(SStandardSlateWidget)
	{}

	SLATE_ARGUMENT(TWeakObjectPtr<class APracaInzHUD>, OwnerHUDArg);

	SLATE_END_ARGS()

	/** Constructs this widget with InArgs */
	void Construct(const FArguments& InArgs);

	void OnCommitted(const FText& InText, ETextCommit::Type InCommitType);

private:
	TWeakObjectPtr<class APracaInzHUD> OwnerHUD;
};
