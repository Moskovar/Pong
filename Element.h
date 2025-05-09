#pragma once
#include "Model.h"
#include "uti.hpp"

#define NUM_BONES  100

class Element
{
public:
	Element() {};
	Element(short id, glm::vec3 position, const std::string& filePath);
	Element(short id, glm::mat4 modelMtx, const std::string& filePath);
	Element(short id, glm::vec3 position, Model* model);

	~Element();

	//--- Getters ---//
	Model*		getModel()			{ return model;				}
	glm::vec3	getPosition()		{ return position;			}
	glm::vec3*	getPositionP()		{ return &position;			}
	GLfloat		getX()				{ return position.x;		}
	GLfloat		getY()				{ return position.y;		}
	GLfloat		getZ()				{ return position.z;		}
	GLfloat		getYaw()			{ return yaw;				}
	GLfloat*	getPYaw()			{ return &yaw;				}
	OBB&		getRHitbox()		{ return hitbox;			}
	glm::vec3   getMaxPoint()		{ return hitbox.maxPoint;	}
	glm::vec3   getMinPoint()		{ return hitbox.minPoint;	}
	bool		isMoving()			{ return moving;			}
	bool		isFalling()			{ return falling;			}
	glm::mat4	getModelMtx()		{ return modelMatrix;		}
	uint8_t		getDirectionValue() { return movingValue;		}
	GLfloat		getMoveSpeed()		{ return moveSpeed;			}
	glm::vec3	getRotations()		{ return rotations;			}

	glm::mat4	getAnticipatedMove(GLfloat deltaTime);
	OBB			getAnticipatedMoveHitbox(GLfloat deltaTime);
	OBB			getOBBAtPos(glm::vec3 pos);
	OBB			getAnticipatedFallHitbox(GLfloat deltaTime);


	//--- Setters ---//
	void setAnimationID(uint8_t id)			{ animationID = id;												}
	void setPosition(glm::vec3 position)	{ this->position = position; updateModelMatrixFromPosition();	}
	void setPositionZ(GLfloat z)			{ this->position.z = z;  updateModelMatrixFromPosition();		}
	void setPositionX(GLfloat x)			{ this->position.x = x;  updateModelMatrixFromPosition();		}
	void setYaw(GLfloat yaw);
	void setMove(bool state);
	void setFall(bool state);
	void setModelMtx(glm::mat4 modelMatrix);
	void setMoveSpeed(GLfloat moveSpeed) { this->moveSpeed = moveSpeed; }

	void moveForward(GLfloat z);
	void moveZ(GLfloat deltaTime);
	void moveAtY(GLfloat positionY);
	void moveAtZ(GLfloat positionZ);
	void translate(glm::vec3 translation);
	void turn(GLfloat yaw);
	void turn(GLfloat yaw, glm::vec3 axes, bool recalculateHitbox = true);
	void resetRotations();
	void updatePosition(bool recalculateHitbox = true);
	void updateModelMatrixFromPosition();

	void clear();

	void render(const GLuint& modelLoc, const GLuint& bonesTransformsLoc);

protected:
	uint8_t id = -1, animationID = 1, movingValue = 0;//entier non sign� de 0 � 255 -> � mettre dans une autre classe qui g�re les entr�es claviers ?

	//--- POSITION ---//
	Model* model = nullptr;
	glm::mat4 modelMatrix = glm::mat4(1.0f);
	glm::vec3 position = glm::vec3(0.0f, 0.0f, 0.0f);
	GLfloat yaw = -90.0f;//� descendre dans entity ? avec render en m�thode abstraite ?

	glm::vec3 rotations = glm::vec3(0.0f, 0.0f, 0.0f);
	GLfloat moveSpeed = 10.0f;
	bool moving = false, falling = false;
	std::chrono::high_resolution_clock::time_point startTime;//pour l'animation de l'�l�ment (chaque �l�ment a son start time (quand il est cr��e et reset quand il change d'animation))

	OBB hitbox;

	void calculateHitBox();
};

