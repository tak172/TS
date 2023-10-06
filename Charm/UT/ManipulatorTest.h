#pragma once

#include "../StatBuilder/Manipulator.h"

class ManipulatorTest : public Manipulator
{
public:
    ManipulatorTest( CGraphicView * gview, MultiEditor * meditor, boost::mutex * mtx, ScrollGlider * sglider ) :
		Manipulator( gview, meditor, mtx, sglider ), sysKeys( Manipulator::SYSKEYS::NONEKEYS ), txtMergingMode( false ){}
    virtual SYSKEYS GetSysKeys() const override { return sysKeys; }
    void SetSysKeys( SYSKEYS skeys ) { sysKeys = skeys; }
	void SetTxtMerging( bool txtMerging ) { txtMergingMode = txtMerging; }
	std::wstring ProblemTxt() const { return problemTxt; }

private:
    SYSKEYS sysKeys;
	std::wstring problemTxt;
	bool txtMergingMode;
	virtual bool IsTxtMerging() const override { return txtMergingMode; }
};