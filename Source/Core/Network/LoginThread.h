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

class LoginThread : private Thread
{
public:
    
    LoginThread();
    ~LoginThread() override;
    
    class Listener
    {
    public:
        virtual ~Listener() {}
    private:
        virtual void loginOk(const String &userEmail) = 0;
        virtual void loginAuthorizationFailed() = 0;
        virtual void loginConnectionFailed() = 0;
        friend class LoginThread;
    };
    
    void login(LoginThread::Listener *authListener,
               String userEmail, String userPasswordHash);
    
private:
    
    void run() override;
    
    URL url;
    String email;
    String passwordHash;
    
    LoginThread::Listener *listener;
    
    friend class AuthManager;
    
};
