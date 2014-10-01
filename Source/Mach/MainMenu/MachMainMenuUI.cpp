#include "Mach.h"


BEGIN_SLATE_FUNCTION_BUILD_OPTIMIZATION
void SMachMainMenuUI::Construct(const FArguments& InArgs)
{
	MenuHUD = InArgs._MenuHUD;

	ChildSlot
	[
		SNew(SOverlay)
		+ SOverlay::Slot().HAlign(HAlign_Left).VAlign(VAlign_Top).Padding(100.f)
		[
			SNew(SVerticalBox)
			+ SVerticalBox::Slot().FillHeight(2).Padding(0, 0, 0, 10.f)
			[
				SNew(STextBlock)
				.ColorAndOpacity(FLinearColor::White)
				.ShadowColorAndOpacity(FLinearColor::Black)
				.ShadowOffset(FIntPoint(-1, 1))
				.Font(FSlateFontInfo("Helvetica", 26))
				.Text(FText::FromString("Main Menu"))
			]
			+ SVerticalBox::Slot().Padding(0, 5.f)
			[
				SNew(SButton)
				.Text(FText::FromString("Play Game"))
				.OnClicked(this, &SMachMainMenuUI::PlayClicked)
			]
			+ SVerticalBox::Slot().Padding(0, 5.f)
			[
				SNew(SButton)
				.Text(FText::FromString("Customize Gear"))
				.OnClicked(this, &SMachMainMenuUI::PlayClicked)
			]
			+ SVerticalBox::Slot().Padding(0, 5.f)
			[
				SNew(SButton)
				.Text(FText::FromString("Quit Game"))
				.OnClicked(this, &SMachMainMenuUI::QuitClicked)
			]
		]
		/*
		SNew(SOverlay)
		+ SOverlay::Slot()
			.HAlign(HAlign_Left)
			.VAlign(VAlign_Top)
			.Padding(FMargin(100.f))
			[
				SNew(STextBlock)
				.ColorAndOpacity(FLinearColor::White)
				.ShadowColorAndOpacity(FLinearColor::Black)
				.ShadowOffset(FIntPoint(-1, 1))
				.Font(FSlateFontInfo("Arial", 26))
				.Text(FText::FromString("Main Menu"))
			]
		+ SOverlay::Slot() 
			.HAlign(HAlign_Left)
			.VAlign(VAlign_Top)
			.Padding(FMargin(100.f, 0))
			[
				SNew(SVerticalBox)
				+ SVerticalBox::Slot()
					[
						SNew(SButton)
						.Text(FText::FromString("Play Game"))
						.OnClicked(this, &SMachMainMenuUI::PlayClicked)
					]
				+ SVerticalBox::Slot()
					[
						SNew(SButton)
						.Text(FText::FromString("Customize Gear"))
						.OnClicked(this, &SMachMainMenuUI::PlayClicked)
					]
				+ SVerticalBox::Slot()
					[
						SNew(SButton)
						.Text(FText::FromString("Quit Game"))
						.OnClicked(this, &SMachMainMenuUI::QuitClicked)
					]
			]
		*/
	];
}

FReply SMachMainMenuUI::PlayClicked()
{
	MenuHUD->PlayClicked();
	return FReply::Handled();
}

FReply SMachMainMenuUI::QuitClicked()
{
	MenuHUD->QuitClicked();
	return FReply::Handled();
}
END_SLATE_FUNCTION_BUILD_OPTIMIZATION
