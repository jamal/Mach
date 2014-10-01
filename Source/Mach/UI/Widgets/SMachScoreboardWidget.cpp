#include "Mach.h"
#include "SMachScoreboardWidget.h"

#define LOCTEXT_NAMESPACE "MachScoreboard"

BEGIN_SLATE_FUNCTION_BUILD_OPTIMIZATION
void SMachScoreboardWidget::Construct(const FArguments& InArgs)
{
	PCOwner = InArgs._PCOwner;

	ScoreboardTint = FLinearColor(0.0f, 0.0f, 0.0f, 0.4f);
	BoxPadding = 2.5f;
	PlayerNameWidth = 190.0f;
	ScoreBoxWidth = 35.0f;
	ScoreCountUpTime = 2.0f;

	Columns.Add(FColumnData(LOCTEXT("KillsColumn", "K").ToString(),
		FSlateColor(FLinearColor::White),
		FOnGetPlayerStateAttribute::CreateSP(this, &SMachScoreboardWidget::GetAttributeValue_Kills)));

	Columns.Add(FColumnData(LOCTEXT("DeathsColumn", "D").ToString(),
		FSlateColor(FLinearColor::White),
		FOnGetPlayerStateAttribute::CreateSP(this, &SMachScoreboardWidget::GetAttributeValue_Deaths)));

	Columns.Add(FColumnData(LOCTEXT("AssistsColumn", "A").ToString(),
		FSlateColor(FLinearColor::White),
		FOnGetPlayerStateAttribute::CreateSP(this, &SMachScoreboardWidget::GetAttributeValue_Assists)));

	Columns.Add(FColumnData(LOCTEXT("AssistsColumn", "Ping").ToString(),
		FSlateColor(FLinearColor::White),
		FOnGetPlayerStateAttribute::CreateSP(this, &SMachScoreboardWidget::GetAttributeValue_Ping)));

	// Build header row
	const TSharedRef<SHorizontalBox> HeaderCols = SNew(SHorizontalBox)
	+ SHorizontalBox::Slot()
	[
		SNew(SBox)
		.Padding(BoxPadding)
		.WidthOverride(PlayerNameWidth)
		.HAlign(HAlign_Left)
		[
			SNew(STextBlock)
			.Text(LOCTEXT("PlayerName", "Player Name").ToString())
		]
	];

	for (uint8 ColIdx = 0; ColIdx < Columns.Num(); ColIdx++)
	{
		//Header constant sized columns
		HeaderCols->AddSlot() .VAlign(VAlign_Center) .HAlign(HAlign_Center) .AutoWidth()
		[
			SNew(SBox)
			.Padding(BoxPadding)
			.WidthOverride(ScoreBoxWidth)
			.HAlign(HAlign_Center)
			[
				SNew(STextBlock)
				.Text(Columns[ColIdx].Name)
				.ColorAndOpacity(Columns[ColIdx].Color)
			]
		];
	}

	const TSharedRef<SHorizontalBox> Scoreboard = SNew(SHorizontalBox)
	+ SHorizontalBox::Slot() .AutoWidth() .Padding(5)
	[
		SNew(SBorder)
		[
			SNew(SVerticalBox)
			+ SVerticalBox::Slot() .AutoHeight()
			[
				SNew(SBox)
				.Padding(BoxPadding)
				.HAlign(HAlign_Left)
				[
					SNew(STextBlock)
					.Text(LOCTEXT("TeamA", "Team A").ToString())
				]
			]
			+ SVerticalBox::Slot() .AutoHeight()
			[
				HeaderCols
			]
			+ SVerticalBox::Slot() .AutoHeight()
			[
				SAssignNew(TeamAScoreboard, SVerticalBox)
			]
		]
	]
	+ SHorizontalBox::Slot() .AutoWidth() .Padding(5)
	[
		SNew(SBorder)
		[
			SNew(SVerticalBox)
			+ SVerticalBox::Slot() .AutoHeight()
			[
				SNew(SBox)
				.Padding(BoxPadding)
				.HAlign(HAlign_Left)
				[
					SNew(STextBlock)
					.Text(LOCTEXT("TeamB", "Team B").ToString())
				]
			]
			+ SVerticalBox::Slot() .AutoHeight()
			[
				HeaderCols
			]
			+ SVerticalBox::Slot() .AutoHeight()
			[
				SAssignNew(TeamBScoreboard, SVerticalBox)
			]
		]
	];

	UpdatePlayerStates();
	UpdateScoreboard();

	ChildSlot
	[
		Scoreboard
	];
}

void SMachScoreboardWidget::UpdatePlayerStates()
{
	if (PCOwner.IsValid())
	{
		AMachGameState* GameState = Cast<AMachGameState>(PCOwner->GetWorld()->GameState);
		if (GameState)
		{
			bool bWidgetRequiresUpdate = false;
			for (APlayerState* PS : GameState->PlayerArray)
			{
				AMachPlayerState* MachPS = Cast<AMachPlayerState>(PS);
				if (MachPS)
				{
					if (!PlayerStates.Contains(MachPS))
					{
						bWidgetRequiresUpdate = true;
						PlayerStates.Add(MachPS);
					}
				}
			}

			if (bWidgetRequiresUpdate)
			{
				UpdateScoreboard();
			}
		}
	}
}

void SMachScoreboardWidget::UpdateScoreboard()
{
	TeamAScoreboard->ClearChildren();
	TeamBScoreboard->ClearChildren();

	TSharedRef<SVerticalBox> TeamAPlayerRows = SNew(SVerticalBox);
	TSharedRef<SVerticalBox> TeamBPlayerRows = SNew(SVerticalBox);

	for (AMachPlayerState* PlayerState : PlayerStates)
	{
		if (PlayerState->Team == ETeam::A)
		{
			TeamAPlayerRows->AddSlot() .AutoHeight()
			[
				MakePlayerRow(PlayerState)
			];
		}
		else
		{
			TeamBPlayerRows->AddSlot() .AutoHeight()
			[
				MakePlayerRow(PlayerState)
			];
		}
	}
	
	TeamAScoreboard->AddSlot(). AutoHeight()
	[
		TeamAPlayerRows
	];

	TeamBScoreboard->AddSlot(). AutoHeight()
	[
		TeamBPlayerRows
	];
}

TSharedRef<SWidget> SMachScoreboardWidget::MakePlayerRow(AMachPlayerState* PlayerState)
{
	TSharedPtr<SHorizontalBox> PlayerRow;

	SAssignNew(PlayerRow, SHorizontalBox)
	+SHorizontalBox::Slot() .FillWidth(PlayerNameWidth)
	[
		SNew(SBox)
		.Padding(BoxPadding)
		.WidthOverride(PlayerNameWidth)
		.HAlign(HAlign_Left)
		[
			SNew(STextBlock)
			.Text(PlayerState->PlayerName)
		]
	];

	for (uint8 ColIdx = 0; ColIdx < Columns.Num(); ColIdx++)
	{
		PlayerRow->AddSlot() .AutoWidth() .HAlign(HAlign_Center) .VAlign(VAlign_Center)
		[
			SNew(SBox)
			.Padding(BoxPadding)
			.WidthOverride(ScoreBoxWidth)
			.HAlign(HAlign_Center)
			[
				SNew(STextBlock)
				.Text(this, &SMachScoreboardWidget::GetStat, Columns[ColIdx].AttributeGetter, PlayerState)
				.ColorAndOpacity(Columns[ColIdx].Color)
			]
		];
	}

	return PlayerRow.ToSharedRef();
}

FString SMachScoreboardWidget::GetStat(FOnGetPlayerStateAttribute Getter, AMachPlayerState* PlayerState) const
{
	return FText::AsNumber(Getter.Execute(PlayerState)).ToString();
}

uint32 SMachScoreboardWidget::GetAttributeValue_Deaths(AMachPlayerState* PlayerState) const
{
	return PlayerState->GetDeaths();
}

uint32 SMachScoreboardWidget::GetAttributeValue_Kills(AMachPlayerState* PlayerState) const
{
	return PlayerState->GetKills();
}

uint32 SMachScoreboardWidget::GetAttributeValue_Assists(AMachPlayerState* PlayerState) const
{
	return PlayerState->GetAssists();
}

uint32 SMachScoreboardWidget::GetAttributeValue_Ping(AMachPlayerState* PlayerState) const
{
	return PlayerState->Ping;
}

END_SLATE_FUNCTION_BUILD_OPTIMIZATION

#undef LOCKTEXT_NAMESPACE