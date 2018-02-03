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

class HelioApiRequest final
{
public:

    typedef Function<void(int, int)> ProgressCallback;

    HelioApiRequest(URL url, ProgressCallback progressCallback = nullptr);

    struct Response
    {
        Response();
        Response(const Response & other) :
            result(result), statusCode(statusCode),
            headers(headers), jsonBody(jsonBody) {}
        Result result;
        int statusCode;
        StringPairArray headers;
        var jsonBody;
    };

    Response request() const;

    URL url;
    ProgressCallback progressCallback;

private:

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(HelioApiRequest)
};
