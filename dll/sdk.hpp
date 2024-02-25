#pragma once
#include "offset.hpp"
#include "structs.hpp"
#include "memory.hpp"

class AHUD
{
public:
	void drawline(float StartScreenX,
        float StartScreenY,
        float EndScreenX,
        float EndScreenY,
        flinearcolor LineColor,
        float LineThickness)
	{
        return reinterpret_cast<void(*)(AHUD*, float, float, float, float, flinearcolor, float)>(base + offset::drawline)(this, StartScreenX, StartScreenY, EndScreenX, EndScreenY, LineColor, LineThickness);
	}

    void drawtext(const fstring& Text,
        flinearcolor TextColor,
        float ScreenX,
        float ScreenY,
        void* Font,
        float Scale,
        bool bScalePosition)
    {
        return reinterpret_cast<void(*)(AHUD*, const fstring&, flinearcolor, float, float, void*, float, bool)>(base + offset::drawline)(this, Text, TextColor, ScreenX, ScreenY, Font, Scale, bScalePosition);
    }


    void drawrect(flinearcolor RectColor,
        float ScreenX,
        float ScreenY,
        float ScreenW,
        float ScreenH)
    {
        return reinterpret_cast<void(*)(AHUD*, flinearcolor, float, float, float, float)>(base + offset::drawline)(this, RectColor, ScreenX, ScreenY, ScreenW, ScreenH);
    }
};
