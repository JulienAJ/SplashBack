#ifndef VPROGRESSBAR_HPP
#define VPROGRESSBAR_HPP

#include <irrlicht/irrlicht.h>
using namespace irr;

class VProgressBar
{
	private:
	video::IVideoDriver* driver;
	video::SColor color;
	video::ITexture* texture;
	gui::IGUIFont* font;
	s32 top;
	s32 left;
	s32 width;
	s32 height;
	s32 max;
	f32 rate;
	s32 current;
	s32 critical;
	bool overBar;

	public:

	VProgressBar(IrrlichtDevice* device, s32 top, s32 left, s32 width, s32 height, s32 max, video::ITexture* texture)
	{
		this->driver = device->getVideoDriver();
		this->top = top;
		this->left = left;
		this->width = width;
		this->height = height;
		this->max = max;
		this->current = 0;
		this->critical = 0;
		this->rate = ((f32)height-2)/(f32)max;
		this->color = video::SColor(255,255,255,255);
		this->texture = texture;
		this->font = device->getGUIEnvironment()->getFont("media/ScoreFont.png");
		this->overBar = false;
	}

	~VProgressBar()
	{}

	void setColor(video::SColor C)
	{
		color = C;
	}

	void setTexture(io::path Tpath)
	{
		texture = driver->getTexture(Tpath);
	}

	void setTexture(video::ITexture* T)
	{
		texture = T;
	}

	void setFont(gui::IGUIFont* F)
	{
		font = F;
	}

	void setCritical(s32 value)
	{
		critical = value;
	}

	void setValue(s32 value)
	{
		if(overBar || value <= max)
		{
			current = value;
			draw();
		}
	}

	void setMax(s32 value)
	{
		max = value;
		rate = ((f32)height-2)/(f32)max;
	}

	void setOverBar(bool value)
	{
		overBar = value;
	}

	void draw()
	{
		//Container
		driver->draw2DLine(core::vector2d<s32>(left,top), core::vector2d<s32>(left,top+height), color);
		driver->draw2DLine(core::vector2d<s32>(left+width, top), core::vector2d<s32>(left+width,top+height), color);
		driver->draw2DLine(core::vector2d<s32>(left,top+height), core::vector2d<s32>(left+width,top+height), color);

		//Texture
		s32 value = current;
		if(overBar && current > max)
			value = max;

		driver->draw2DImage(texture, core::vector2d<s32>(left+1, top+height-value*rate),
				core::rect<s32>(0, 0, width-2, value*rate));

		//Score
		wchar_t currentString[16];
		swprintf(currentString, sizeof(currentString)/sizeof(*currentString), L"%d", current);
		s32 blue = 255;
		s32 green = 255;
		if(current <= critical)
		{
			green = 0;
			blue = 0;
		}
		font->draw(currentString, core::rect<s32>(0, top+height+10, left*2+2+width, top+height+60), video::SColor(255,255,green,blue), true, true);
	}

	void add(s32 value)
	{
		if(current+value > max && !overBar)
			return;

		current += value;
	}

	void substract(s32 value)
	{
		if(current != 0)
			current -= value;
	}

	s32 getValue()
	{
		return current;
	}

	bool full()
	{
		return(current == max);
	}

	bool empty()
	{
		return(current == 0);
	}

};
#endif
