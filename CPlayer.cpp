//-----------------------------------------------------------------------------
// File: CPlayer.cpp
//
// Desc: This file stores the player object class. This class performs tasks
//       such as player movement, some minor physics as well as rendering.
//
// Original design by Adam Hoult & Gary Simmons. Modified by Mihai Popescu.
//-----------------------------------------------------------------------------

//-----------------------------------------------------------------------------
// CPlayer Specific Includes
//-----------------------------------------------------------------------------
#include "CPlayer.h"
#include <iostream>
using namespace std;
bool col_crate[7];

//-----------------------------------------------------------------------------
// Name : CPlayer () (Constructor)
// Desc : CPlayer Class Constructor
//-----------------------------------------------------------------------------
CPlayer::CPlayer(const BackBuffer *pBackBuffer)
{
	// m_pSprite = new Sprite("data/planeimg.bmp", "data/planemask.bmp");
	m_pSprite = new Sprite("data/planeimgandmask.bmp", RGB(0xff, 0x00, 0xff));
	m_pSprite2 = new Sprite("data/planeimgandmask.bmp", RGB(0xff, 0x00, 0xff));

	m_pSprite_rotate = new Sprite("data/plane_rotate.bmp", RGB(0xff, 0x00, 0xff));

	for (int i = 0; i <= 6; i++)
		crate.push_back(new Sprite("data/Grid_Crate_Mask.bmp", RGB(0xff, 0x3E, 0xF4)));

	bgY = 0;
	lives = 8;
	score = 0;

	m_pSprite->setBackBuffer(pBackBuffer);
	for (int i = 0; i <= 6; i++)
		crate.at(i)->setBackBuffer(pBackBuffer);

	m_pSprite_rotate->setBackBuffer(pBackBuffer);

	m_eSpeedState = SPEED_STOP;
	m_fTimer = 0;

	// Animation frame crop rectangle
	RECT r;
	r.left = 0;
	r.top = 0;
	r.right = 128;
	r.bottom = 128;

	m_pExplosionSprite = new AnimatedSprite("data/explosion.bmp", "data/explosionmask.bmp", r, 16);
	m_pExplosionSprite2 = new AnimatedSprite("data/explosion.bmp", "data/explosionmask.bmp", r, 16);
	m_pExplosionSprite->setBackBuffer(pBackBuffer);
	m_pExplosionSprite2->setBackBuffer(pBackBuffer);
	m_bExplosion = false;
	m_iExplosionFrame = 0;
}

//-----------------------------------------------------------------------------
// Name : ~CPlayer () (Destructor)
// Desc : CPlayer Class Destructor
//-----------------------------------------------------------------------------
CPlayer::~CPlayer()
{
	delete m_pSprite;
	delete m_pSprite2;
	delete m_pSprite_rotate;
	delete m_pExplosionSprite;
	for (int i = 0; i <= 6; i++)
		delete crate.at(i);
}

void CPlayer::Update(float dt)
{

	for (int i = 0; i <= 6; i++)
		crate.at(i)->update(dt);
	for (int i = 1; i <= 6; i++)
	{

		crate.at(i)->mPosition.x = i * 200;
		crate.at(i)->mPosition.y = 150;
	}

	m_pSprite2->mPosition.x = 400;
	m_pSprite2->mPosition.y = 450;
	for (int i = 0; i <= 6; i++)
	{
		if (crate.at(i)->mPosition.y >= 420)
			//	crate.at(i)->mVelocity.y = 0;
			if (col_crate[i])
			{
				score++;
			}
	}

	// Update sprite
	m_pSprite->update(dt);
	m_pSprite_rotate->update(dt);

	m_pSprite_rotate->mVelocity.x = 5;
	m_pSprite_rotate->mVelocity.y = -10;

	for (int i = 0; i <= 6; i++)
	{

		crate.at(i)->mVelocity.y = 30;
		crate.at(i)->mVelocity.x = 5;
	}

	// Get velocity
	double v = m_pSprite->mVelocity.Magnitude();

	// NOTE: for each async sound played Windows creates a thread for you
	// but only one, so you cannot play multiple sounds at once.
	// This creation/destruction of threads also leads to bad performance
	// so this method is not recommanded to be used in complex projects.

	// update internal time counter used in sound handling (not to overlap sounds)
	m_fTimer += dt;

	// A FSM is used for sound manager
	switch (m_eSpeedState)
	{

	case SPEED_STOP:
		if (v > 35.0f)
		{
			m_eSpeedState = SPEED_START;
			PlaySound("data/jet-start.wav", NULL, SND_FILENAME | SND_ASYNC);
			m_fTimer = 0;
		}
		break;
	case SPEED_START:
		if (v < 25.0f)
		{
			m_eSpeedState = SPEED_STOP;
			PlaySound("data/jet-stop.wav", NULL, SND_FILENAME | SND_ASYNC);
			m_fTimer = 0;
		}
		else if (m_fTimer > 1.f)
		{
			PlaySound("data/jet-cabin.wav", NULL, SND_FILENAME | SND_ASYNC);
			m_fTimer = 0;
		}
		break;
	}

	// NOTE: For sound you also can use MIDI but it's Win32 API it is a bit hard
	// see msdn reference: http://msdn.microsoft.com/en-us/library/ms711640.aspx
	// In this case you can use a C++ wrapper for it. See the following article:
	// http://www.codeproject.com/KB/audio-video/midiwrapper.aspx (with code also)
}
void CPlayer::Draw2()
{

	m_pSprite_rotate->draw();
}
void CPlayer::DrawCrate()
{
	for (int i = 0; i <= 6; i++)
	{
		if (!col_crate[i])
			crate.at(i)->draw();
	}
}

void CPlayer::Draw()
{
	if (!m_bExplosion && lives)
	{
		m_pSprite->draw();
	}
	else
	{
		m_pExplosionSprite->draw();
	}
}

void CPlayer::Move(ULONG ulDirection)
{
	// block the plane to be out of the screen
	if (m_pSprite->mPosition.y - m_pSprite->height() / 2 <= 0)
	{
		m_pSprite->mVelocity.y = 0;
		bgY++;
	}
	if (m_pSprite->mPosition.y - m_pSprite->height() / 2 >= 420)
	{
		m_pSprite->mVelocity.y = 0;
		bgY--;
	}
	if (m_pSprite->mPosition.x - m_pSprite->width() / 2 <= 0)
		m_pSprite->mVelocity.x = 0;
	if (m_pSprite->mPosition.x - m_pSprite->width() / 2 >= 685)
		m_pSprite->mVelocity.x = 0;
	//.
	for (int i = 0; i <= 6; i++)
		if (m_pSprite->mPosition.y - m_pSprite->height() / 2 <= crate.at(i)->mPosition.y)
			col_crate[i] = true;

	if (ulDirection & CPlayer::DIR_LEFT)
		m_pSprite->mVelocity.x -= .1;

	if (ulDirection & CPlayer::DIR_RIGHT)
		m_pSprite->mVelocity.x += .1;

	if (ulDirection & CPlayer::DIR_FORWARD)
		m_pSprite->mVelocity.y -= .1;

	if (ulDirection & CPlayer::DIR_BACKWARD)
		m_pSprite->mVelocity.y += .1;
}

Vec2 &CPlayer::Position2()
{
	return m_pSprite_rotate->mPosition;
}
Vec2 &CPlayer::Position()
{
	return m_pSprite->mPosition;
}

Vec2 &CPlayer::Velocity()
{
	return m_pSprite->mVelocity;
}

void CPlayer::Explode()
{
	m_pExplosionSprite->mPosition = m_pSprite->mPosition;
	// m_pExplosionSprite2->mPosition = m_pSprite2->mPosition;
	m_pExplosionSprite->SetFrame(0);
	//	m_pExplosionSprite2->SetFrame(0);

	PlaySound("data/explosion.wav", NULL, SND_FILENAME | SND_ASYNC);
	m_bExplosion = true;
}

bool CPlayer::AdvanceExplosion()
{
	if (m_bExplosion)
	{
		m_pExplosionSprite->SetFrame(m_iExplosionFrame++);
		// m_pExplosionSprite2->SetFrame(m_iExplosionFrame++);
		if (m_iExplosionFrame == m_pExplosionSprite->GetFrameCount())
		{
			m_bExplosion = false;
			m_iExplosionFrame = 0;
			m_pSprite->mVelocity = Vec2(0, 0);
			m_eSpeedState = SPEED_STOP;
			return false;
		}
	}

	return true;
}