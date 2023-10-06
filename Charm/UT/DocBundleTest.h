#pragma once

class DocBundleTest : public DocBundle
{
public:
	using DocBundle::KeeperExist;
	DocBundleTest( IGraphic * pGraphic, ILogic * pLogic ) : DocBundle( pGraphic, pLogic ){}
};