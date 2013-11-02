/*
 * Pattern.cpp
 *
 *  Created on: Oct 20, 2013
 *      Author: kien
 */

#include "Pattern.h"

using namespace Tizen::Base;
using namespace Tizen::App;

Pattern::Pattern()
	: _pName(null)
	, _pFileName(null)
	, _pDuration(1000)
	, _pDescription(null)
	, _pMotionNumber(1)
	, _pDelay(0)
{

}

Pattern::~Pattern()
{
}

String*
Pattern::GetName(void)
{
	return _pName;
}

void
Pattern::SetName(String* name)
{
	_pName = name;
}

String
Pattern::GetMediaFilePath(void)
{
	if (_pType && _pFileName)
		return L"Video/" + *_pType + L"/" + *_pFileName;
		//return App::GetInstance()->GetAppResourcePath() + L"Video/" + *_pType + L"/" + *_pFileName;
	return null;
}

String*
Pattern::GetMediaFileName(void)
{
	return _pFileName;
}

void
Pattern::SetMediaFileName(String* fileName)
{
	_pFileName = fileName;
}

String*
Pattern::GetDescription(void)
{
	return _pDescription;
}

void
Pattern::SetDescription(String* desc)
{
	_pDescription = desc;
}

String*
Pattern::GetType(void)
{
	return _pType;
}

void
Pattern::SetType(String* type)
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

unsigned int
Pattern::GetDuration(void)
{
	return _pDuration;
}

void
Pattern::SetDuration(unsigned int duration)
{
	_pDuration = duration;
}
