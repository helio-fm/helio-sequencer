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
#include "SpectralLogo.h"

#define DEFAULT_LOGO_SIZE 280

LogoFader::LogoFader()
{
    this->addAndMakeVisible(this->gfx = new SpectralLogo());
    this->setSize(DEFAULT_LOGO_SIZE, DEFAULT_LOGO_SIZE);
}

LogoFader::~LogoFader()
{
    this->fader.cancelAllAnimations(false);
    this->stopTimer();
    this->gfx = nullptr;
}

void LogoFader::resized()
{
    this->gfx->setBounds(this->getLocalBounds());
}

void LogoFader::startFade()
{
    this->fader.cancelAllAnimations(true);

    this->gfx->setAlpha(0.f);
    this->fadingDummy.setAlpha(0.f);
    this->startTimerHz(60);

    this->fader.animateComponent(&this->fadingDummy,
        this->fadingDummy.getBounds(),
        1.f, 2500, false, 0.f, 0.f);
}

void LogoFader::timerCallback()
{
    this->gfx->setAlpha(0.33f + this->fadingDummy.getAlpha() / 3.f);
    if (this->fadingDummy.getAlpha() == 1.f)
    {
        this->stopTimer();
    }
}
