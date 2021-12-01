#pragma once

class CBlender_sslr : public IBlender
{
public:
	virtual		LPCSTR		getComment() { return "SSLR"; }
	virtual		BOOL		canBeDetailed() { return FALSE; }
	virtual		BOOL		canBeLMAPped() { return FALSE; }

	virtual		void		Compile(CBlender_Compile& C);

	CBlender_sslr();
	virtual ~CBlender_sslr();
};