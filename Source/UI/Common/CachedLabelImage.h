/*
    This file is part of Helio music sequencer.

    This program is free software: you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation, either version 3 of the License, or
    (at your option) any later version.

    This program is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with this program. If not, see <http://www.gnu.org/licenses/>.
*/

#pragma once

#include "Config.h"

// A simple CachedComponentImage for labels.
// This cache assumes that label's size is fixed,
// so it doesn't have to re-cache it on every setBounds.

struct CachedLabelImage final : public CachedComponentImage
{
    explicit CachedLabelImage(Label &c) noexcept : owner(c) {}

    void paint(Graphics &g) override
    {
        const auto scale = this->uiScaleFactor * g.getInternalContext().getPhysicalPixelScaleFactor();
        auto compBounds = this->owner.getLocalBounds();
        auto imageBounds = compBounds * scale;

        if (this->image.isNull())
        {
            this->image = Image(Image::ARGB,
                jmax(1, imageBounds.getWidth()),
                jmax(1, imageBounds.getHeight()),
                true);

            this->text.clear();
        }

        if (this->text != this->owner.getText())
        {
            Graphics imG(this->image);
            auto &lg = imG.getInternalContext();
            lg.addTransform(AffineTransform::scale(scale));

            lg.setFill(Colours::transparentBlack);
            lg.fillRect(compBounds, true);
            lg.setFill(Colours::black);

            this->owner.paintEntireComponent(imG, true);
            this->text = this->owner.getText();
        }

        g.setOpacity(this->owner.getAlpha());
        g.drawImageTransformed(this->image,
            AffineTransform::scale(compBounds.getWidth() / (float)imageBounds.getWidth(),
                compBounds.getHeight() / (float)imageBounds.getHeight()), false);
    }

    // will be called on setBounds(), but we don't need to invalidate:
    bool invalidateAll() override { return false; }
    bool invalidate(const Rectangle<int> &area) override { return false; }

    // this class does not track color or font changes,
    // because they should be rare, pls do it manually:
    bool forceInvalidate()
    {
        this->text = {};
        this->image = {};
        return false;
    }

    // Do nothing, this is called on every setVisible
    // (image will be released anyway in the destructor)
    void releaseResources() override {}

private:

    Image image;
    String text;
    Label &owner;

    const float uiScaleFactor = App::Config().getUiFlags()->getUiScaleFactor();

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(CachedLabelImage)
};
