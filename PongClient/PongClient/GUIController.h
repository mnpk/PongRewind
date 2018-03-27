#pragma once

#include "SharedStruct.h"
#include "GameLogic.h"

class GUIController
{
public:
	GUIController();

	void Initialize(int* argcp, char** argv);
	
	void Reset();

	void GetRacketInput(float& posDiff, bool& isFire);

	void OnMenuEvent(int menuId);

	void OnRender();
	void OnUpdate();

	void DoEventLoop();

	void SetTextMessage(const std::string& str) { mTextMessage = str;  }

	void OnGameStart(PlayerType myType);
	void OnStatusChange(const GameStatusBroadcast& gs);

	PlayerType GetMyPlayerType() const { return mMyPlayerType; }
	void SetMyPlayerType(PlayerType player) { mMyPlayerType = player; }
	
private:
	void DrawRect(float x, float y, float width, float height);
	void DrawLine(float x1, float y1, float x2, float y2);

	void Resync(uint32_t fromFrame);

private:
	PlayerType mMyPlayerType;

	/// Client input history �����ϰ� desync�� ����
	struct InputElem
	{
		InputElem()
			:mInputFrame(0), mPosDiff(0), mShoot(false)
		{}

		InputElem(uint32_t ifr, float posdiff, bool shoot) 
			: mInputFrame(ifr), mPosDiff(posdiff), mShoot(shoot)
		{}

		uint32_t mInputFrame;
		float mPosDiff;
		bool mShoot;
	};

	/// �ϴ� ���� ������ �켱�̹Ƿ� �̷��� ����
	std::map<uint32_t, InputElem> mInputHistory;
	uint32_t mRecentInputFrame;
	

	GameStatus mGameStatus; ///< for client only
	unsigned int mRecentWorldFrame;

	std::string mTextMessage;
	char mTextLine[128];

};

extern std::unique_ptr<GUIController> GGuiController;