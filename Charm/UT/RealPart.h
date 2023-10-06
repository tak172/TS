#pragma once

#include "InfraPart.h"

class RealPart : public InfraPart
{
public:
	RealPart();

private:
	void CreateLigatne(); //Лигатне
	void CreateRiga(); //Рига
	void CreateLiepaja(); //Лиепая
	void CreateVangazi(); //Вангази
	void CreateSloka(); //Слока
	void CreateKarsava(); //Карсава
	void CreateKraslava(); //Краслава
	void CreateKrauja(); //Крауя
	void CreateMezvidi(); //Мезвиди
	void CreateSkirotava(); //Шкиротава
	void CreateKemeri(); //Кемери
	void CreateRezekne2(); //Резекне 2
	void CreateJelgava(); //Елгава
	void CreateBale(); //Бале
	void SetDetails(); //установка доп.атрибутов (номера путей, парков, координаты стыков)
	void SetStripDetails();
	void SetSemaDetails();
};