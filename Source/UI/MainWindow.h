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

class MainLayout;

#if JUCE_WINDOWS || JUCE_LINUX
#   define HELIO_HAS_CUSTOM_TITLEBAR 1
#else
#   define HELIO_HAS_CUSTOM_TITLEBAR 0
#endif

class MainWindow final :
    public DocumentWindow,
    public FileDragAndDropTarget,
    public DragAndDropContainer
{
public:

    MainWindow();
    ~MainWindow() override;
    
    static bool isRunningOnPhone();
    static bool isRunningOnTablet();
    static bool isRunningOnDesktop();
    
    void closeButtonPressed() override;

    void setOpenGLRendererEnabled(bool shouldBeEnabled);
    static bool isOpenGLRendererEnabled() noexcept;

    //===------------------------------------------------------------------===//
    // Drag'n'drop
    //===------------------------------------------------------------------===//

    bool isInterestedInFileDrag(const StringArray &files) override { return true; }
    void filesDropped(const StringArray &filenames, int mouseX, int mouseY) override {}

private:

#if HELIO_HAS_CUSTOM_TITLEBAR
    BorderSize<int> getBorderThickness() override;
#endif

    void attachOpenGLContext();
    void detachOpenGLContext();

    void dismissLayoutComponent();
    void createLayoutComponent();

    ScopedPointer<MainLayout> layout;

    friend class App;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(MainWindow)
};
