/*
    This file is part of Helio Workstation.

    Helio is free software: you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation, either version 3 of the License, or
    (at your option) any later version.

    Helio is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with Helio. If not, see <http://www.gnu.org/licenses/>.
*/

#include "Common.h"
#include "LogoFader.h"

#include "LogoImage.h"
#include "SpectralLogo.h"

#define DEFAULT_LOGO_SIZE 280
#define OLD_LOGO_SIZE 350

LogoFader::LogoFader(bool useOldLogo)
{
    if (useOldLogo) {
        this->logoSize = OLD_LOGO_SIZE;
        this->addAndMakeVisible(this->gfx = new LogoImage());
    } else {
        this->logoSize = DEFAULT_LOGO_SIZE;
        this->addAndMakeVisible(this->gfx = new SpectralLogo());
    }
    
    this->setSize(this->logoSize, this->logoSize);
}

LogoFader::~LogoFader()
{
    this->gfx = nullptr;
}

void LogoFader::resized()
{
    this->gfx->setBounds(0, 0, this->logoSize, this->logoSize);
}

void LogoFader::startFade()
{
    this->fader.cancelAllAnimations(true);

    this->startTimer(1000 / 50);
    this->fadingDummy.setAlpha(0.4f);
    this->gfx->setAlpha(0.55f);

    this->fader.animateComponent(&this->fadingDummy,
                                 this->fadingDummy.getBounds(),
                                 1.f,
                                 5000,
                                 false,
                                 1.f,
                                 1.f);
}

void LogoFader::timerCallback()
{
    this->gfx->setAlpha(this->fadingDummy.getAlpha());
    
    if (this->gfx->getAlpha() == 1.f)
    {
        this->stopTimer();
    }
}
