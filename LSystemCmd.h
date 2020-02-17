#ifndef CreateLSystemCmd_H_
#define CreateLSystemCmd_H_
#pragma once
#include <maya/MPxCommand.h>
#include <string>
#include <LSystem.h>
#include <maya/MArgList.h>
#include <maya/MArgDatabase.h>
#include <maya/MSyntax.h>

class LSystemCmd : public MPxCommand
{
public:
    LSystemCmd();
    virtual ~LSystemCmd();
    
	static void* creator() 
	{ 
		return new LSystemCmd(); 
	}
    
	virtual MStatus doIt( const MArgList& args );
	
	static MSyntax newSyntax();
};

#endif