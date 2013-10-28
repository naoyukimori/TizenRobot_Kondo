/*
 * Pattern.cpp
 *
 *  Created on: Oct 20, 2013
 *      Author: kien
 */

#include <FApp.h>
#include "Pattern.h"

using namespace Tizen::Base;
using namespace Tizen::App;

Pattern::Pattern()
	: _pName()
	, _pFileName()
	, _pPathToMedia()
	, _pDuration(0)
	, _pDescription()
	, _pMotionNumber(1)
	, _pDelay(0)
{
	std::stringstream str;
	std::string s1 = "5f0066";
	str << s1;
	int value;
	str >> std::hex >> value;
}

Pattern::Pattern(String name, String fileName, String desc, String type,
			unsigned int motionNumber, unsigned int delay, unsigned int duration)
{
	_pName = name;
	_pFileName = fileName;
	_pDescription = desc;
	_pType = type;
	_pMotionNumber = motionNumber;
	_pDelay = delay;
	_pDuration = duration;
	_pPathToMedia = App::GetInstance()->GetAppResourcePath() + "Video/" + _pType + L"/" + _pFileName;
}

Pattern::~Pattern()
{
}

String
Pattern::GetName(void)
{
	return _pName;
}

void
Pattern::SetName(String name)
{
	_pName = name;
}

String
Pattern::GetMediaFilePath(void)
{
	return _pPathToMedia;
}

String
Pattern::GetMediaFileName(void)
{
	return _pFileName;
}

void
Pattern::SetMediaFileName(String fileName)
{
	_pFileName = fileName;
	_pPathToMedia = App::GetInstance()->GetAppResourcePath() + _pType + L"/" + _pFileName;
}

String
Pattern::GetDescription(void)
{
	return _pDescription;
}

void
Pattern::SetDescription(String desc)
{
	_pDescription = desc;
}

String
Pattern::GetType(void)
{
	return _pType;
}

void
Pattern::SetType(String type)
{
	_pType = type;
}

unsigned int
Pattern::GetMotionNumber(void)
{
	return _pMotionNumber;
}

void
Pattern::SetMotionNumber(unsigned int number)
{
	_pMotionNumber = number;
}

unsigned int
Pattern::GetDelay(void)
{
	return _pDelay;
}

void
Pattern::SetDelay(unsigned int delay)
{
	_pDelay = delay;
}
