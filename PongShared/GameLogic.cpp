
#include "GameLogic.h"
#include <math.h>
#include <functional>

template <class T>
inline void hash_combine(unsigned __int64& s, const T& v)
{
	std::hash<T> h;
	s ^= h(v) + 0x9e3779b9 + (s << 6) + (s >> 2);
}

GameLogic::GameLogic()
{
	ResetGameStatus();
}

void GameLogic::ResetGameStatus()
{
	mCurrentWorldFrame = 0;
	mGameStatus.ResetStatus();
	mFrameHistory.Clear();
}

void GameLogic::StartPlaying()
{
	GameStatus gs;
	gs.mCurrentStatus = CurrentGameStatus::CGS_STARTED;
	SetCurrentGameStatus(gs);
}

bool GameLogic::IsPlaying() const
{
	return GetCurrentGameStatus().mCurrentStatus == CurrentGameStatus::CGS_STARTED;
}

unsigned __int64 GameLogic::GetGameStatusHash(const GameStatus& stat)
{
	unsigned __int64 result = 0;

	/// �ݵ�� ��ġ�ؾ� �ϴ� ������ �ؽ��Ѵ�.

	hash_combine(result, stat.mCurrentStatus);
	hash_combine(result, stat.mWorldFrame);
	//hash_combine(result, stat.mLeftRacketPosY);
	//hash_combine(result, stat.mRightRacketPosY);
	hash_combine(result, stat.mLeftShoot);
	hash_combine(result, stat.mRightShoot);
	hash_combine(result, stat.mLeftScore);
	hash_combine(result, stat.mRightScore);
	
	return result;
}

void GameLogic::GiveUp(PlayerType player)
{
	GameStatus gs = GetCurrentGameStatus();

	if (PlayerType::PLAYER_LEFT == player)
	{
		gs.mCurrentStatus = CurrentGameStatus::CGS_GAME_OVER_RIGHT_WIN;
	}
	else
	{
		gs.mCurrentStatus = CurrentGameStatus::CGS_GAME_OVER_LEFT_WIN;
	}

	SetCurrentGameStatus(gs);
}

bool GameLogic::OnServerUpdate()
{
	GameStatus gs = GetCurrentGameStatus();

	gs.mWorldFrame = ++mCurrentWorldFrame;

	bool res = GetUpdatedGameStatus(gs);

	SetCurrentGameStatus(gs);
	
	return res;
}

bool GameLogic::ChangeGameStatus(GameStatus& stat, PlayerType player, float posDiff, bool shoot)
{
	bool isNewShoot = false;

	if (PlayerType::PLAYER_LEFT == player)
	{
		stat.mLeftRacketPosY += posDiff;
		
		/// �̹� �ߵ����� �ƴ� ��츸 (�̹� �ߵ����̶�� ������ ��Ÿ�� ����)
		if (shoot && stat.mLeftShoot == 0)
		{
			isNewShoot = true;
			stat.mLeftShoot = SHOOT_FRAME_LENGTH;
		}
	}
	else
	{
		stat.mRightRacketPosY += posDiff;

		if (shoot && stat.mRightShoot == 0)
		{
			isNewShoot = true;
			stat.mRightShoot = SHOOT_FRAME_LENGTH;
		}
	}

	return (posDiff != 0) || isNewShoot;
}

bool GameLogic::GetUpdatedGameStatus(GameStatus& stat)
{

	if (stat.mLeftShoot > 0 && stat.mRightShoot > 0)
	{
		/// �Ѵ� �� ���� ��·�ų� ��ȿ�� �Ǿ� ���� ��ȭ ����
		stat.mLeftShoot = 0;
		stat.mRightShoot = 0;
		return false;
	}

	/// ���ʸ� �� ��� �������� HIT�Ǿ����� ����
	if (stat.mLeftShoot > 0)
	{
		float laserPosY = stat.mLeftRacketPosY + RACKET_HEIGHT / 2.f;
		if ( stat.mRightRacketPosY < laserPosY && laserPosY < stat.mRightRacketPosY + RACKET_HEIGHT )
		{
			stat.mLeftShoot = 0; ///< HIT�Ǿ����� ���� ����
			++stat.mLeftScore; ///< ����
		}
		else
		{
			--stat.mLeftShoot; ///< HIT �ȵǾ����� ���ӽð� ������ ����
		}
		

		if (stat.mLeftScore == SCORE_FOR_WIN)
		{
			stat.mCurrentStatus = CurrentGameStatus::CGS_GAME_OVER_LEFT_WIN;
			return true;
		}
	}

	/// �����ʸ� �� ��� ������ HIT�Ǿ����� ����
	if (stat.mRightShoot > 0)
	{
		float laserPosY = stat.mRightRacketPosY + RACKET_HEIGHT / 2.f;
		if (stat.mLeftRacketPosY < laserPosY && laserPosY < stat.mLeftRacketPosY + RACKET_HEIGHT)
		{
			stat.mRightShoot = 0; ///< HIT�Ǿ����� ���� ����
			++stat.mRightScore; ///< ����
		}
		else
		{
			--stat.mRightShoot; ///< HIT �ȵǾ����� ���ӽð� ������ ����
		}


		if (stat.mRightScore == SCORE_FOR_WIN)
		{
			stat.mCurrentStatus = CurrentGameStatus::CGS_GAME_OVER_RIGHT_WIN;
			return true;
		}
	}
	

	/// ������� ���� ���º�ȭ ���� �׳� ���� ���������� ����
	return false;
}


void GameLogic::SetGameStatus(unsigned int frame, const GameStatus& stat)
{
	mFrameHistory.SetObjectForFrame(frame, stat);
	mCurrentWorldFrame = frame;
}

GameStatus GameLogic::GetGameStatus(unsigned int frame) const
{
	return mFrameHistory.GetObjectForFrame(frame);
}

void GameLogic::SetCurrentGameStatus(const GameStatus& stat)
{
	SetGameStatus(stat.mWorldFrame, stat);
}

GameStatus GameLogic::GetCurrentGameStatus() const
{
	return mFrameHistory.GetObjectForFrame(mCurrentWorldFrame);
}