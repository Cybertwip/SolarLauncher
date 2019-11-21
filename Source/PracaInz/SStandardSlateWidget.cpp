// Fill out your copyright notice in the Description page of Project Settings.


#include "SStandardSlateWidget.h"
#include "Widgets/Input/SEditableTextBox.h"
#include "SlateOptMacros.h"

BEGIN_SLATE_FUNCTION_BUILD_OPTIMIZATION

#define LOCTEXT_NAMESPACE "SStandardSlateWidget"

void SStandardSlateWidget::Construct(const FArguments& InArgs)
{
	OwnerHUD = InArgs._OwnerHUDArg;
	ChildSlot
		[
			SNew(SEditableTextBox)
			.OnTextCommitted(this, &SStandardSlateWidget::OnCommitted)
		];

	/*
	ChildSlot
		.VAlign(VAlign_Fill)
		.HAlign(HAlign_Fill)
		[
			SNew(SOverlay)
			+ SOverlay::Slot()
		.VAlign(VAlign_Top)
		.HAlign(HAlign_Center)
		[
			// Inside lies a text block with these settings
			SNew(STextBlock)
			.ShadowColorAndOpacity(FLinearColor::Black)
		.ColorAndOpacity(FLinearColor::Red)
		.ShadowOffset(FIntPoint(-1, 1))
		.Font(FSlateFontInfo("Veranda", 16))
		// localized text to be translated with a generic name HelloSlateText
		.Text(LOCTEXT("HelloSlateText", "Hello, Slate!"))
		]
		];
		*/
}

void SStandardSlateWidget::OnCommitted(const FText& InText, ETextCommit::Type InCommitType)
{

}


#undef LOCTEXT_NAMESPACE
END_SLATE_FUNCTION_BUILD_OPTIMIZATION
