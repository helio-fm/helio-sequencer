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

#pragma once

// A simple CachedComponentImage for labels.
// This cache assumes that label's size is fixed,
// so it doesn't have to re-cache it on every setBounds.

struct CachedLabelImage : public CachedComponentImage
{
    CachedLabelImage(Label &c) noexcept : owner(c), scale(1.0f) {}

    void paint(Graphics &g) override
    {
        this->scale = g.getInternalContext().getPhysicalPixelScaleFactor();
        auto compBounds = this->owner.getLocalBounds();
        auto imageBounds = compBounds * this->scale;

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
            lg.addTransform(AffineTransform::scale(this->scale));

            lg.setFill(Colours::transparentBlack);
            lg.fillRect(compBounds, true);
            lg.setFill(Colours::black);

            this->owner.paintEntireComponent(imG, true);
            this->text = this->owner.getText();
        }

        g.setColour(Colours::black.withAlpha(this->owner.getAlpha()));
        g.drawImageTransformed(this->image,
            AffineTransform::scale(compBounds.getWidth() / (float)imageBounds.getWidth(),
                compBounds.getHeight() / (float)imageBounds.getHeight()), false);
    }

    bool invalidateAll() override { return false; }
    bool invalidate(const Rectangle<int>& area) override { return false; }

    // Do nothing, this is called on every setVisible
    // (image will be released anyway in a destructor)
    void releaseResources() override {}

private:

    Image image;
    String text;
    Label &owner;
    float scale;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(CachedLabelImage)
};
