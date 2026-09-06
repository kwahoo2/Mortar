/* Copyright (c) 2026, Adrian Przekwas
 * All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without modification,
 * are permitted provided that the following conditions are met:
 *
 * 1. Redistributions of source code must retain the above copyright notice, this
 * list of conditions and the following disclaimer.
 *
 * 2. Redistributions in binary form must reproduce the above copyright notice,
 * this list of conditions and the following disclaimer in the documentation and/or
 * other materials provided with the distribution.
 *
 * 3. Neither the name of the copyright holder nor the names of its contributors
 * may be used to endorse or promote products derived from this software without
 * specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" AND
 * ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED
 * WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
 * DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE FOR
 * ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES
 * (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES;
 * LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON
 * ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
 * (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS
 * SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 */

#include <iostream>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <cstdint>
#include "MotorWorker.h"

#define DRIVER_PORT 2137

struct RemoteCommand {
    uint8_t id;      // 0x00 - ALT, 0x01 - AZI
    uint8_t command; // 0x00 - set target value
    union {
        int64_t intVal;
        double doubleVal;
    } val;
};

int main(int argc, char* argv[]) {
    MotorWorker motorworker;

    int server_fd;
    struct sockaddr_in address;
    int opt = 1;

    std::string allowedIP = "";
    if (argc > 1) {
        allowedIP = argv[1]; // use the first argument to limit allowed IP - for safety
        std::cout << "Allowed IP: " << allowedIP << std::endl;
    } else {
        std::cout << "No IP restriction. All IPs are allowed." << std::endl;
    }

    if ((server_fd = socket(AF_INET, SOCK_STREAM, 0)) == 0) {
        std::cerr << "Socket creation failed!" << std::endl;
        return -1;
    }

    if (setsockopt(server_fd, SOL_SOCKET, SO_REUSEADDR, &opt, sizeof(opt))) {
        std::cerr << "Setsockopt failed!" << std::endl;
        close(server_fd);
        motorworker.stop();
        motorworker.join();
        return -1;
    }

    address.sin_family = AF_INET;
    address.sin_addr.s_addr = INADDR_ANY; // accept any IP TODO dangerous
    address.sin_port = htons(DRIVER_PORT);

    if (bind(server_fd, (struct sockaddr *)&address, sizeof(address)) < 0) {
        std::cerr << "Bind failed!" << std::endl;
        close(server_fd);
        motorworker.stop();
        motorworker.join();
        return -1;
    }

    if (listen(server_fd, 3) < 0) {
        std::cerr << "Listen failed!" << std::endl;
        close(server_fd);
        motorworker.stop();
        motorworker.join();
        return -1;
    }

    std::cout << "Server listening on port " << DRIVER_PORT << "..." << std::endl;
    while (true) {
        int new_socket;
        struct sockaddr_in client_address;
        socklen_t client_addrlen = sizeof(client_address);
        if ((new_socket = accept(server_fd, (struct sockaddr *)&client_address, (socklen_t*)&client_addrlen)) < 0) {
            std::cerr << "Accept failed!" << std::endl;
            continue;
        }

        char clientIP[INET_ADDRSTRLEN];
        inet_ntop(AF_INET, &(client_address.sin_addr), clientIP, INET_ADDRSTRLEN);
        std::string clientIPStr(clientIP);

        if (!allowedIP.empty() && allowedIP != clientIPStr) {
            std::cerr << "Connection from disallowed IP: " << clientIPStr << std::endl;
            close(new_socket);
            continue;
        }

        std::cout << "Connection accepted from: " << clientIPStr << std::endl;

        RemoteCommand remoteCommand;
        while (true)
        {
            ssize_t bytes_read = recv(new_socket, &remoteCommand, sizeof(remoteCommand), 0);
            if (bytes_read <= 0) {
                std::cerr << "Connection closed or error!" << std::endl;
                break;
            }
            switch (remoteCommand.command)
            {
                case 0x00:
                    if (remoteCommand.id == 0 ) { std::cout << "Alt position: " << remoteCommand.val.doubleVal << std::endl;
                        motorworker.setPositionAlt(remoteCommand.val.doubleVal);
                    }
                    else if (remoteCommand.id == 1) { std::cout << "Azi position: " << remoteCommand.val.doubleVal << std::endl;
                        motorworker.setPositionAzi(remoteCommand.val.doubleVal);
                    }
                    break;
                case 0x01:
                    std::cout << "Driver id: " << remoteCommand.val.intVal << std::endl;
                    motorworker.stop();
                    motorworker.join();
                    motorworker.setDriver(static_cast<int>(remoteCommand.val.intVal));
                    motorworker.start();
                    break;
                case 0x02:
                    if (remoteCommand.id == 0 ) { std::cout << "Alt speed: " << remoteCommand.val.intVal << std::endl;
                        motorworker.setMaxSpeedAlt(static_cast<int>(remoteCommand.val.intVal));
                    }
                    else if (remoteCommand.id == 1) { std::cout << "Azi speed: " << remoteCommand.val.intVal << std::endl;
                        motorworker.setMaxSpeedAzi(static_cast<int>(remoteCommand.val.intVal));
                    }
                    break;
                case 0x03:
                    std::cout << "Disable steppers: " << remoteCommand.val.intVal << std::endl;
                     motorworker.disableSteppers(remoteCommand.val.intVal != 0);
                    break;
                case 0x04:
                    std::cout << "Set paused: " << remoteCommand.val.intVal << std::endl;
                    motorworker.setPaused(remoteCommand.val.intVal != 0);
                    break;
                case 0x05:
                    std::cout << "Hold PWM: " << remoteCommand.val.intVal << std::endl;
                    motorworker.setHoldPWM(static_cast<int>(remoteCommand.val.intVal));
                    break;
                case 0x06:
                    std::cout << "Run PWM: " << remoteCommand.val.intVal << std::endl;
                    motorworker.setRunPWM(static_cast<int>(remoteCommand.val.intVal));
                    break;
                case 0x07:
                    std::cout << "Fast decay: " << remoteCommand.val.intVal << std::endl;
                    motorworker.setFastDecay(remoteCommand.val.intVal != 0);
                    break;
                case 0x08:
                    std::cout << "Shutter mode: " << remoteCommand.val.intVal << std::endl;
                    motorworker.enableShutterMode(remoteCommand.val.intVal != 0);
                    break;
                default:
                    break;
            }
        }
        close(new_socket);
        std::cout << "Waiting for the next connection..." << std::endl;
    }
    motorworker.stop();
    motorworker.join();
    close(server_fd);
    std::cout << "Server closed." << std::endl;

    return 0;
}
