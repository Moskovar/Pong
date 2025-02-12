#pragma once
#include "Paddle.h"

class Player
{
	public:
		Player(short gameID, short id, short side);
		~Player();

		Paddle* getPaddle() { return paddle; }
		NetworkPaddle getNP() { return { Header::NP, gameID, id, (int)(paddle->getPosition().z * 1000) }; }
		short getSide() { return side; }

		void update(NetworkPaddle& np) { if(paddle) paddle->setPositionZ((GLfloat)(np.z / 1000.0f)); }

	private:
		short gameID = -1, id = -1, side = 0;
		Paddle* paddle = nullptr;
};

