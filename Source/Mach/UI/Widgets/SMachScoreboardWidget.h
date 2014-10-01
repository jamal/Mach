#pragma once

#include "Slate.h"

DECLARE_DELEGATE_RetVal_OneParam(uint32, FOnGetPlayerStateAttribute, AMachPlayerState*);

struct FColumnData
{
	/** Column name */
	FString Name;

	/** Column color */
	FSlateColor Color;

	/** Stat value getter delegate */
	FOnGetPlayerStateAttribute AttributeGetter;

	/** defaults */
	FColumnData()
	{
		Color = FLinearColor::White;
	}

	FColumnData(FString InName, FSlateColor InColor, FOnGetPlayerStateAttribute InAtrGetter)
	{
		Name = InName;
		Color = InColor;
		AttributeGetter = InAtrGetter;
	}
};

/**
 * 
 */
class MACH_API SMachScoreboardWidget : public SCompoundWidget
{
public:
	SLATE_BEGIN_ARGS(SMachScoreboardWidget)
	{}

	SLATE_ARGUMENT(TWeakObjectPtr<APlayerController>, PCOwner)
	
	SLATE_END_ARGS()

	/** Constructs this widget with InArgs */
	void Construct(const FArguments& InArgs);

protected:
	/** scoreboard tint color */
	FLinearColor ScoreboardTint;

	float ScoreBoxWidth;
	float BoxPadding;
	float PlayerNameWidth;

	/** scoreboard count up time */
	float ScoreCountUpTime;

	/** stat columns data */
	TArray<FColumnData> Columns;

	/** TODO: This should be sorted by score or something */
	TArray<AMachPlayerState*> PlayerStates;

	TWeakObjectPtr<class APlayerController> PCOwner;

	TSharedPtr<SVerticalBox> TeamAScoreboard;
	TSharedPtr<SVerticalBox> TeamBScoreboard;

	void UpdatePlayerStates();
	void UpdateScoreboard();

	TSharedRef<SWidget> MakePlayerRow(AMachPlayerState* PlayerState);
	FString GetStat(FOnGetPlayerStateAttribute Getter, AMachPlayerState* PlayerState) const;

	uint32 GetAttributeValue_Kills(class AMachPlayerState* PlayerState) const;
	uint32 GetAttributeValue_Deaths(class AMachPlayerState* PlayerState) const;
	uint32 GetAttributeValue_Assists(class AMachPlayerState* PlayerState) const;
	uint32 GetAttributeValue_Ping(AMachPlayerState* PlayerState) const;

};
