#include "Player.h"

Player::Player(short gameID, short id, short side)
{
	std::cout << "Player created GameID: " << gameID << std::endl;
	this->gameID	= gameID;
	this->id		= id;
	this->side		= side;

	if (side == -1)
	{
		paddle = new Paddle(id, glm::vec3(-70.0f, 0.0f, 0.0f), "models/fbx/paddle.fbx");
	}
	else
	{
		std::cout << "Right side created" << std::endl;
		paddle = new Paddle(id, glm::vec3(70.0f, 0.0f, 0.0f), "models/fbx/paddle.fbx");
		paddle->turn(180);
	}
}

Player::~Player()
{
	if (paddle)
	{
		delete paddle;
		paddle = nullptr;
	}
}
