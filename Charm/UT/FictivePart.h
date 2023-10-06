#pragma once

#include "InfraPart.h"

class FictivePart : public InfraPart
{
public:
	FictivePart( const std::wstring & axis_name );

private:
	typedef ADProperties::Attrubute ADAttribute;
	const std::wstring & axis_name; //система отсчета
	void CreateFirstPart(); //первая часть тестируемых объектов
	void CreateSecondPart(); //вторая часть тестируемых объектов
	void CreateThirdPart(); //третья часть тестируемых объектов
	void CreateFourthPart(); //четвертая часть тестируемых объектов
	void CreateFifthPart(); //пятая часть тестируемых объектов
	void CreateWrongWayPart(); //часть для тестирования события выхода на неправильный путь перегона
	void CreatePocketPart(); //перегон с карманами
	void CreateBlockPostPart(); //блок-посты
	void CreateTechnicalStationPart(); //техническая станция
	void CreateLongSpan(); //очень длинный перегон
	void SetDetails(); //установка доп.атрибутов (номера путей, парков, координаты стыков)
	void SetStripDetails();
	void SetJointDetails();
	void SetSemaDetails();
	void SetSpanDetails();
	void SetMostSpanDetails();
	void SetIWSpanDetails();
	void SetJointCoords(); //координаты стыков
};
