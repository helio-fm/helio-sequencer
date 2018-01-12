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

typedef struct
{
    String projectId;
    String projectKey;
    String projectTitle;
    int64 lastModifiedTime;
} RemoteProjectDescription;

class RequestProjectsListThread : private Thread
{
public:
    
    RequestProjectsListThread();
    
    ~RequestProjectsListThread() override;

    class Listener
    {
    public:
        virtual ~Listener() {}
    private:
        virtual void listRequestOk(const String &userEmail, Array<RemoteProjectDescription> list) = 0;
        virtual void listRequestAuthorizationFailed() = 0;
        virtual void listRequestConnectionFailed() = 0;
        friend class RequestProjectsListThread;
    };

    void requestListAndEmail(RequestProjectsListThread::Listener *authListener);
    
private:
    
    void run() override;

    URL url;

    RequestProjectsListThread::Listener *listener;
    
    ReadWriteLock listLock;
    
    Array<RemoteProjectDescription> projectsList;
    
    String userEmail;
    
    friend class AuthManager;
    
};
