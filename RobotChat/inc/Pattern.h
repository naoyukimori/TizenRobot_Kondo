/*
 * Pattern.h
 *
 *  Created on: Oct 20, 2013
 *      Author: kien
 */

#ifndef PATTERN_H_
#define PATTERN_H_

#include <FBase.h>
#include <FWebJson.h>

using namespace Tizen::Base;
using namespace Tizen::Base::Collection;

class Pattern
{
public:
	Pattern(void);
	~Pattern(void);

	String* GetName(void);
	void SetName(String* name);
	String GetMediaFilePath(void);
	String* GetMediaFileName(void);
	void SetMediaFileName(String* fileName);
	String* GetDescription(void);
	void SetDescription(String* desc);
	String* GetType(void);
	void SetType(String* type);
	unsigned int GetMotionNumber(void);
	void SetMotionNumber(unsigned int number);
	unsigned int GetDelay(void);
	void SetDelay(unsigned int delay);
	unsigned int GetDuration(void);
	void SetDuration(unsigned int duration);

protected:
	String* _pName;
	String* _pFileName;
	String* _pType;
	unsigned int _pDuration;
	String* _pDescription;
	unsigned int _pMotionNumber;
	unsigned int _pDelay;
};


#endif /* PATTERN_H_ */
