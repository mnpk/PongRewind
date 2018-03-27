#pragma once
#include "SharedStruct.h"
#include <array>

template <class BASETYPE, uint32_t MAX_REWIND>
class RewindObject
{
public:
	RewindObject()
	{
		Clear();
	}

	void SetObjectForFrame(uint32_t frame, const BASETYPE& obj)
	{
		/// ���� �ð����� ���ſ� ������ �ȵ�
		if (frame < mHeadFrame)
			return;

		/// ���� �ð��� Rewind�� ���� ������ �� �ִ� �ִ븦 �Ѵ� ���
		if (frame >= mHeadFrame + MAX_REWIND)
		{
			mHeadFrame = frame;
			mHeadIndex = 0;

			mHistory.fill(obj);

			return;
		}

		const BASETYPE& prevObj = mHistory[mHeadIndex];

		/// ���� ������ �������� �ٷ��� �ֽŰ� ä���
		while (mHeadFrame < frame)
		{
			++mHeadIndex;
			if (mHeadIndex == MAX_REWIND)
				mHeadIndex = 0;

			mHistory[mHeadIndex] = prevObj;

			++mHeadFrame;
		}

		mHistory[mHeadIndex] = obj;
	}

	BASETYPE GetObjectForFrame(uint32_t frame) const
	{
		/// ���� ������ �ִ°ͺ��� �̷��� ���� �䱸�� ���� �׳� �ֽŰ� ����
		if (frame > mHeadFrame)
			return mHistory[mHeadIndex];

		/// ������ ������ ���� �䱸�Ҷ��� ���� ���Ű� ����
		auto delta = mHeadFrame - frame;
		if (delta >= MAX_REWIND)
		{
			return mHistory[(mHeadIndex + 1) % MAX_REWIND];
		}

		return mHistory[(mHeadIndex + MAX_REWIND - delta) % MAX_REWIND];
	}

	void Clear()
	{
		mHistory.fill(BASETYPE());
		mHeadFrame = 0;
		mHeadIndex = 0;
	}

private:
	uint32_t mHeadFrame; ///< ���� ������
	uint32_t mHeadIndex; ///< �迭�� ��ġ

	std::array<BASETYPE, MAX_REWIND> mHistory;

};

class GameLogic
{
public:
	GameLogic();

	void GiveUp(PlayerType player);
	
	bool OnServerUpdate();

	/// return true if the game status changed
	static bool ChangeGameStatus(GameStatus& stat, PlayerType player, float posDiff, bool shoot);

	/// return true when the game ends
	static bool GetUpdatedGameStatus(GameStatus& stat);

	static unsigned __int64 GetGameStatusHash(const GameStatus& stat);
	
	void ResetGameStatus();

	unsigned int GetWorldFrame() const { return mCurrentWorldFrame; }

	void StartPlaying();

	bool IsPlaying() const;


	void SetGameStatus(unsigned int frame, const GameStatus& stat);
	GameStatus GetGameStatus(unsigned int frame) const;
	
	void SetCurrentGameStatus(const GameStatus& stat);
	GameStatus GetCurrentGameStatus() const;
	


private:

	GameStatus mGameStatus;
	
	RewindObject<GameStatus, MAX_WORLD_FRAME_HISTORY> mFrameHistory;
	unsigned int mCurrentWorldFrame;
};